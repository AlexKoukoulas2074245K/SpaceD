/**********************************************************************/
/** shader.cpp by Alex Koukoulas (C) 2017 All Rights Reserved        **/
/** File Description:                                                **/
/**********************************************************************/

#pragma once

// Local Headers
#include "shader.h"

// Remote Headers
#include <d3dx11.h>

// Constant members
const std::string Shader::SHADER_DIRECTORY_PATH = "../res/shaders/";

const std::string Shader::SHADER_VERTEX_EXT          = ".vs";
const std::string Shader::SHADER_VERTEX_ENTRY_NAME   = "VS";
const std::string Shader::SHADER_VERTEX_PROFILE_NAME = "vs_5_0";

const std::string Shader::SHADER_PIXEL_EXT          = ".ps";
const std::string Shader::SHADER_PIXEL_ENTRY_NAME   = "PS";
const std::string Shader::SHADER_PIXEL_PROFILE_NAME = "ps_5_0"; 

Shader::Shader(const std::string& name, comptr<ID3D11Device> device)
	: _name(name)
	, _vShader(0)
    , _pShader(0)
	, _inputLayout(0)
    , _constantBuffer(0)
    , _vsBlob(0)
    , _psBlob(0)
{
	Compile(device);
}

Shader::~Shader()
{
}

comptr<ID3D11VertexShader> Shader::getVertexShader() const
{
	return _vShader;
}

comptr<ID3D11PixelShader> Shader::getPixelShader() const
{
	return _pShader;
}

comptr<ID3D11InputLayout> Shader::getInputLayout() const
{
	return _inputLayout;
}

comptr<ID3D11Buffer> Shader::getConstantBuffer() const
{
	return _constantBuffer;
}

comptr<ID3D10Blob> Shader::getVertexShaderBlob() const
{
	return _vsBlob;
}

comptr<ID3D10Blob> Shader::getPixelShaderBlob() const
{
	return _psBlob;
}

void Shader::Compile(comptr<ID3D11Device> device)
{
	ID3D10Blob* errorMessage;

	HRESULT result = D3DX11CompileFromFile(std::string(SHADER_DIRECTORY_PATH + _name + SHADER_VERTEX_EXT).c_str(), 0, 0, SHADER_VERTEX_ENTRY_NAME.c_str(), SHADER_VERTEX_PROFILE_NAME.c_str(), D3D10_SHADER_ENABLE_STRICTNESS, 0, 0, &_vsBlob, &errorMessage, 0);

	if (FAILED(result))
	{
		if (errorMessage)
		{
			MessageBoxA(0, (char*)errorMessage->GetBufferPointer(), 0, 0);
		}
		else
		{
			DXTrace(__FILE__, (DWORD)__LINE__, result, "D3DX11CompileFromFile", true);
		}
	}

	if (errorMessage)
	{
		errorMessage->Release();
	}

	HR(device->CreateVertexShader(_vsBlob->GetBufferPointer(), _vsBlob->GetBufferSize(), 0, &_vShader));

	result = D3DX11CompileFromFile(std::string(SHADER_DIRECTORY_PATH + _name + SHADER_PIXEL_EXT).c_str(), 0, 0, SHADER_PIXEL_ENTRY_NAME.c_str(), SHADER_PIXEL_PROFILE_NAME.c_str(), D3D10_SHADER_ENABLE_STRICTNESS, 0, 0, &_psBlob, &errorMessage, 0);

	if (FAILED(result))
	{
		if (errorMessage)
		{
			MessageBoxA(0, (char*)errorMessage->GetBufferPointer(), 0, 0);
		}
		else
		{
			DXTrace(__FILE__, (DWORD)__LINE__, result, "D3DX11CompileFromFile", true);
		}
	}

	HR(device->CreatePixelShader(_psBlob->GetBufferPointer(), _psBlob->GetBufferSize(), 0, &_pShader));

	if (errorMessage)
	{
		errorMessage->Release();
	}
}