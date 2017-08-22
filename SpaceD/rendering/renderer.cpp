/**********************************************************************/
/** renderer.cpp by Alex Koukoulas (C) 2017 All Rights Reserved  **/
/** File Description:                                                **/
/**********************************************************************/

#pragma once

// Local Headers
#include "renderer.h"
#include "renderingcontext.h"
#include "shaders/default3dshader.h"
#include "shaders/default3dwithlightingshader.h"
#include "../util/clientwindow.h"
#include "model.h"

// Remote Headers

Renderer::Renderer(ClientWindow& clientWindow)
	: _renderingContext(new RenderingContext(clientWindow))
	, _activeShaderType(Shader::ShaderType::DEFAULT_3D_WITH_LIGHTING)
{
	LoadShaders();
}

Renderer::~Renderer()
{
}

void Renderer::OnResize()
{
	_renderingContext->OnResize();
}

void Renderer::ClearViews()
{
	float bkgcol[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	_renderingContext->_deviceContext->ClearRenderTargetView(_renderingContext->_renderTargetView.Get(), bkgcol);
	_renderingContext->_deviceContext->ClearDepthStencilView(_renderingContext->_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Renderer::Present()
{
	HR(_renderingContext->_swapChain->Present(0, 0));
}

void Renderer::SetShader(const Shader::ShaderType shader)
{
	_activeShaderType = shader;
}

void Renderer::RenderModel(const Model& model, const void* constantBufferData)
{	
	auto& activeShader = _shaders[_activeShaderType];
	_renderingContext->_deviceContext->UpdateSubresource(activeShader->getConstantBuffer().Get(), 0, 0, constantBufferData, 0, 0);

	auto stride = sizeof(Vertex);
	auto offset = 0U;

	// Input Assembly Stage
	_renderingContext->_deviceContext->IASetInputLayout(activeShader->getInputLayout().Get());
	_renderingContext->_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_renderingContext->_deviceContext->IASetVertexBuffers(0, 1, model.GetVertexBuffer().GetAddressOf(), &stride, &offset);
	_renderingContext->_deviceContext->IASetIndexBuffer(model.GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

	// Vertex Shader Stage
	_renderingContext->_deviceContext->VSSetShader(activeShader->getVertexShader().Get(), 0, 0);
	_renderingContext->_deviceContext->VSSetConstantBuffers(0, 1, activeShader->getConstantBuffer().GetAddressOf());

	// Pixel Shader Stage
	_renderingContext->_deviceContext->PSSetShader(activeShader->getPixelShader().Get(), 0, 0);
	_renderingContext->_deviceContext->PSSetShaderResources(0, 1, model.GetTexture().GetAddressOf());
	_renderingContext->_deviceContext->PSSetConstantBuffers(0, 1, activeShader->getConstantBuffer().GetAddressOf());

	_renderingContext->_deviceContext->DrawIndexed(model.GetIndexCount(), 0, 0);
}

comptr<ID3D11Device> Renderer::GetDevice() const
{
	return _renderingContext->_device;
}

comptr<ID3D11DeviceContext> Renderer::GetDeviceContext() const
{
	return _renderingContext->_deviceContext;
}

void Renderer::LoadShaders()
{
	_shaders.clear();
	_shaders.resize(Shader::ShaderType::SHADER_COUNT);
	_shaders[Shader::ShaderType::DEFAULT_3D] = std::move(std::unique_ptr<Shader>(new Default3dShader(_renderingContext->_device)));
	_shaders[Shader::ShaderType::DEFAULT_3D_WITH_LIGHTING] = std::move(std::unique_ptr<Shader>(new Default3dWithLightingShader(_renderingContext->_device)));
	
}