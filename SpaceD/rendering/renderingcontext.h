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
class ClientWindow;
class Shader;
class Model;

class RenderingContext final
{
public:
	RenderingContext(ClientWindow& clientWindow);
	~RenderingContext();

	void OnResize();
	void ClearViews();
	void Present();
	
	void RenderModel(const Model& model);

	comptr<ID3D11Device> GetDevice() const;
	comptr<ID3D11DeviceContext> GetDeviceContext() const;

private:
	void InitD3D();
	void PrepareShaders();
	
private:
	std::unique_ptr<Shader> _default3dShader;

	comptr<ID3D11Device> _device;
	comptr<ID3D11DeviceContext> _deviceContext;
	comptr<IDXGISwapChain> _swapChain;
	comptr<ID3D11RenderTargetView> _renderTargetView;
	comptr<ID3D11DepthStencilView> _depthStencilView;
	comptr<ID3D11Texture2D> _depthStencilBuffer;
	comptr<ID3D11SamplerState> _samplerState;
	comptr<ID3D11BlendState> _blendState;

	D3D11_VIEWPORT _screenViewport;

	ClientWindow& _clientWindow;

	UINT _msaaQuality;
	
};