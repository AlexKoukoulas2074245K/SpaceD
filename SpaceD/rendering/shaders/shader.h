/********************************************************************/
/** shader.h by Alex Koukoulas (C) 2017 All Rights Reserved        **/
/** File Description:                                              **/
/********************************************************************/

#pragma once

// Local Headers
#include "../d3dcommon.h"

// Remote Headers
#include <string>

class Shader
{
public:
	enum ShaderType
	{
		DEFAULT_3D = 0,
		DEFAULT_3D_WITH_LIGHTING = 1,
		SHADER_COUNT = 2
	};

public:
	Shader(const std::string& name, comptr<ID3D11Device> device);
	virtual ~Shader();

	comptr<ID3D11VertexShader> getVertexShader() const;
	comptr<ID3D11PixelShader> getPixelShader() const;
	comptr<ID3D11InputLayout> getInputLayout() const;
	comptr<ID3D11Buffer> getConstantBuffer() const;
	comptr<ID3D10Blob> getVertexShaderBlob() const;
	comptr<ID3D10Blob> getPixelShaderBlob() const;

protected:
	virtual void PrepareConstantBuffersAndLayout(comptr<ID3D11Device> device) = 0;

private:
	void Compile(comptr<ID3D11Device> device);

protected:
	static const std::string SHADER_DIRECTORY_PATH;
	
	static const std::string SHADER_VERTEX_EXT;
	static const std::string SHADER_VERTEX_ENTRY_NAME;
	static const std::string SHADER_VERTEX_PROFILE_NAME;

	static const std::string SHADER_PIXEL_EXT;
	static const std::string SHADER_PIXEL_ENTRY_NAME;
	static const std::string SHADER_PIXEL_PROFILE_NAME;

protected:
	const std::string& _name;

	comptr<ID3D11VertexShader> _vShader;
	comptr<ID3D11PixelShader> _pShader;
	comptr<ID3D11InputLayout> _inputLayout;
	comptr<ID3D11Buffer> _constantBuffer;
	comptr<ID3D10Blob> _vsBlob;
	comptr<ID3D10Blob> _psBlob;
};