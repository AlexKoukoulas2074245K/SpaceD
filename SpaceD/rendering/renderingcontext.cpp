/*************************************************************************/
/** renderingcontext.cpp by Alex Koukoulas (C) 2017 All Rights Reserved **/
/** File Description:                                                   **/
/*************************************************************************/

// Remote Headers
#include <cassert>
#include <dxgi.h>
#include <dxerr.h>
#include <sstream>
#include <stdio.h>
#include <vector>

// Local Headers
#include "renderingcontext.h"
#include "../util/clientwindow.h"

// Linking to external libs
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxerr.lib")
#pragma comment(lib, "legacy_stdio_definitions.lib")

// D3D HR Debugging Macro
#if defined(DEBUG) | defined(_DEBUG)
    #ifndef HR
        #define HR(x)                                               \
        {                                                           \
            HRESULT hr = (x);                                       \
            if(FAILED(hr))                                          \
            {                                                       \
                DXTraceW(__FILE__, (DWORD)__LINE__, hr, L#x, true); \
            }                                                       \
        }
    #endif // End of HR macro definition
#else // Debug Not defined
    #ifndef HR
        #define HR(x)(x)
    #endif
#endif // End of HR Debug Macro



RenderingContext::RenderingContext(ClientWindow& clientWindow)
	: _device(0)
	, _deviceContext(0)
	, _swapChain(0)
	, _renderTargetView(0)
	, _depthStencilView(0)
	, _depthStencilBuffer(0)
	, _clientWindow(clientWindow)
	, _msaaQuality(0)
{
	InitD3D();
}

RenderingContext::~RenderingContext()
{
}

void RenderingContext::OnResize()
{
	assert(_device.Get());
	assert(_deviceContext.Get());
	assert(_swapChain.Get());

	// Release the old views, as they hold references to the buffers we
	// will be destroying.  Also release the old depth/stencil buffer.
	_renderTargetView.ReleaseAndGetAddressOf();
	_depthStencilView.ReleaseAndGetAddressOf();
	_depthStencilBuffer.ReleaseAndGetAddressOf();

	// Resize the swap chain and recreate the render target view.
	HR(_swapChain->ResizeBuffers(1, _clientWindow.GetWidth(), _clientWindow.GetHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	ID3D11Texture2D* backBuffer;
	HR(_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
	HR(_device->CreateRenderTargetView(backBuffer, 0, _renderTargetView.GetAddressOf()));
	backBuffer->Release();

	// Create the depth/stencil buffer and view.
	D3D11_TEXTURE2D_DESC dsd;
	dsd.Width  = _clientWindow.GetWidth();
	dsd.Height = _clientWindow.GetHeight();

	dsd.ArraySize = 1;
	dsd.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsd.MipLevels = 1;

	dsd.SampleDesc.Count   = 1;
	dsd.SampleDesc.Quality = 0;

	dsd.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
	dsd.CPUAccessFlags = 0;
	dsd.MiscFlags      = 0;
	dsd.Usage          = D3D11_USAGE_DEFAULT;

	HR(_device->CreateTexture2D(&dsd, 0, _depthStencilBuffer.GetAddressOf()));
	HR(_device->CreateDepthStencilView(_depthStencilBuffer.Get(), 0, _depthStencilView.GetAddressOf()));

	// Bind the render target view and depth/stencil view to the pipeline.
	_deviceContext->OMSetRenderTargets(1, _renderTargetView.GetAddressOf(), _depthStencilView.Get());

	// Set the viewport transform.
	_screenViewport.TopLeftX = 0;
	_screenViewport.TopLeftY = 0;
	_screenViewport.Width    = static_cast<float>(_clientWindow.GetWidth());
	_screenViewport.Height   = static_cast<float>(_clientWindow.GetHeight());
	_screenViewport.MinDepth = 0.0f;
	_screenViewport.MaxDepth = 1.0f;

	_deviceContext->RSSetViewports(1, &_screenViewport);
}

void RenderingContext::ClearViews()
{
	float bkgcol[4] = {0.0f, 0.0f, 1.0f, 0.0f};
	_deviceContext->ClearRenderTargetView(_renderTargetView.Get(), bkgcol);
	_deviceContext->ClearDepthStencilView(_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void RenderingContext::Present()
{
	HR(_swapChain->Present(0, 0));
}

comptr<ID3D11Device> RenderingContext::getDevice() const
{
	return _device;
}

comptr<ID3D11DeviceContext> RenderingContext::getDeviceContext() const
{
	return _deviceContext;
}

void RenderingContext::InitD3D()
{
	UINT deviceFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	deviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Enumerate and output adapter descriptions
	UINT i = 0;
	std::vector<IDXGIAdapter*> adapters;
	IDXGIAdapter* adapter;
	IDXGIFactory* factory;
	CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);

	while (factory->EnumAdapters(i++, &adapter) != DXGI_ERROR_NOT_FOUND)
	{
		adapters.push_back(adapter);
		DXGI_ADAPTER_DESC adapterDesc;
		HR(adapter->GetDesc(&adapterDesc));

		std::wostringstream adapterDescStream;
		adapterDescStream << "At position " << i - 1 
			              << " found adapter: " << adapterDesc.Description 
			              << " | Video Memory: " << adapterDesc.DedicatedVideoMemory/1000000 << "Mb"
			              << "\n";
		OutputDebugStringW(adapterDescStream.str().c_str());
	}

	// Cleanup adapters
	for (auto* adapterEntry: adapters)
	{
		adapterEntry->Release();
	}

	// Create device and immediate context
	D3D_FEATURE_LEVEL featureLevel;
	HR(D3D11CreateDevice(0, // use default adapter at index 0
					     D3D_DRIVER_TYPE_HARDWARE,
		                 0,
		                 deviceFlags,
		                 0, 0, // default feature levels
		                 D3D11_SDK_VERSION,
		                 &_device,
		                 &featureLevel,
                         &_deviceContext));
 		 
	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, "Direct3D 11.0 not supported", 0, MB_ICONERROR);
		PostQuitMessage(-1);
	}

	// Check MSAA 4x support
	HR(_device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &_msaaQuality));
	std::ostringstream msaaQualityStream;
	msaaQualityStream << "Supported MSAA: " << _msaaQuality << "\n";
	OutputDebugString(msaaQualityStream.str().c_str());
	assert(_msaaQuality > 0);

	// Fill out Swap chain description
	DXGI_SWAP_CHAIN_DESC scd;
	scd.BufferCount = 1;
	
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.Width  = _clientWindow.GetWidth();
	scd.BufferDesc.Height = _clientWindow.GetHeight();

	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferDesc.RefreshRate.Numerator   = 60;

	scd.BufferDesc.Scaling          = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = _clientWindow.GetHandle();

	scd.SampleDesc.Count   = 1;
	scd.SampleDesc.Quality = 0;

	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Windowed   = true;

	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // can use DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH here.
	// when the application is switching to full - screen mode, it will choose a display mode 
	// that best matches the current back buffer settings.If this flag is not specified, then when 
	// the application is switching to full - screen mode, it will use the current desktop display mode. 
	// In our sampleframework, we do not specify this flag, as using the current desktop display mode in full - screen mode works fine for our demos
	// (most desktop displays are set to the optimal resolution of the monitor).

	// Find and use the original DXGI Factory to create the swap chain
	IDXGIDevice* dxgiDevice = 0;
	HR(_device.Get()->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

	IDXGIAdapter* dxgiAdapter = 0;
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

	IDXGIFactory* dxgiFactory = 0;
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

	HR(dxgiFactory->CreateSwapChain(_device.Get(), &scd, &_swapChain));

	dxgiFactory->Release();
	dxgiAdapter->Release();
	dxgiDevice->Release();

	OnResize();
}
