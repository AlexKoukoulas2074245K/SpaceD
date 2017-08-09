/************************************************************************/
/** renderingcontext.h by Alex Koukoulas (C) 2017 All Rights Reserved  **/
/** File Description:                                                  **/
/************************************************************************/

#pragma once

// Remote Headers
#include <d3d11.h>
#include <wrl\client.h>

// Local Headers

// Com pointer convenience macro
#define comptr Microsoft::WRL::ComPtr

// Forward Declarations
class ClientWindow;

class RenderingContext final
{
public:
	RenderingContext(ClientWindow& clientWindow);
	~RenderingContext();

	void OnResize();
	void ClearViews();
	void Present();

	comptr<ID3D11Device> getDevice() const;
	comptr<ID3D11DeviceContext> getDeviceContext() const;

private:
	void InitD3D();

private:
	comptr<ID3D11Device> _device;
	comptr<ID3D11DeviceContext> _deviceContext;
	comptr<IDXGISwapChain> _swapChain;
	comptr<ID3D11RenderTargetView> _renderTargetView;
	comptr<ID3D11DepthStencilView> _depthStencilView;
	comptr<ID3D11Texture2D> _depthStencilBuffer;
	D3D11_VIEWPORT _screenViewport;

	ClientWindow& _clientWindow;

	UINT _msaaQuality;
};