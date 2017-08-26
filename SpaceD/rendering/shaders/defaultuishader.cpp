/**********************************************************************/
/** defaultuishader.cpp by Alex Koukoulas (C) 2017 All Rights Reserved  **/
/** File Description:                                                **/
/**********************************************************************/

#pragma once

// Local Headers
#include "defaultuishader.h"

// Remote Headers
DefaultUiShader::~DefaultUiShader()
{
}

DefaultUiShader::DefaultUiShader(comptr<ID3D11Device> device)
	: Shader("defaultui", device)
{
	PrepareConstantBuffersAndLayout(device);
}

void DefaultUiShader::PrepareConstantBuffersAndLayout(comptr<ID3D11Device> device)
{
	D3D11_BUFFER_DESC cbd = {};
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.ByteWidth = sizeof(ConstantBuffer);

	device->CreateBuffer(&cbd, 0, &_constantBuffer);

	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	HR(device->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), _vsBlob->GetBufferPointer(), _vsBlob->GetBufferSize(), &_inputLayout));
}