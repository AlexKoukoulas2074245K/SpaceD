/************************************************************************/
/** renderingcontext.h by Alex Koukoulas (C) 2017 All Rights Reserved  **/
/** File Description:                                                  **/
/************************************************************************/

#pragma once

// Remote Headers
#include <memory>

// Local Headers
#include "d3dcommon.h"
#include "vertex.h"

// Forward Declarations
class Renderer;
class ClientWindow;
class Model;

class RenderingContext final
{
	friend class Renderer;

public:
	~RenderingContext();

private:
	RenderingContext(ClientWindow& clientWindow);
	
	void InitD3D();
	void OnResize();
	void SetWireframe(const bool wireframe);

private:
	comptr<ID3D11Device> _device;
	comptr<ID3D11DeviceContext> _deviceContext;
	comptr<IDXGISwapChain> _swapChain;
	comptr<ID3D11RenderTargetView> _renderTargetView;
	comptr<ID3D11DepthStencilView> _depthStencilView;
	comptr<ID3D11Texture2D> _depthStencilBuffer;
	comptr<ID3D11SamplerState> _samplerState;
	comptr<ID3D11BlendState> _blendState;
	comptr<ID3D11RasterizerState> _defaultRastState;
	comptr<ID3D11RasterizerState> _wireframeRastState;

	D3D11_VIEWPORT _screenViewport;

	ClientWindow& _clientWindow;

	UINT _msaaQuality;
	
};