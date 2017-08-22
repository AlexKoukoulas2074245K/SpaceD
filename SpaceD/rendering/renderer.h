/********************************************************************/
/** renderer.h by Alex Koukoulas (C) 2017 All Rights Reserved      **/
/** File Description:                                              **/
/********************************************************************/

#pragma once

// Local Headers
#include "d3dcommon.h"
#include "shaders/shader.h"

// Remote Headers
#include <memory>
#include <vector>

// Forward declarations
class RenderingContext;
class ClientWindow;
class Model;

class Renderer final
{
public:
	Renderer(ClientWindow& clientWindow);
	~Renderer();

	void OnResize();
	void ClearViews();
	void Present();

	void SetShader(const Shader::ShaderType shader);
	void RenderModel(const Model& model, const void* constantBufferData);

	comptr<ID3D11Device> GetDevice() const;
	comptr<ID3D11DeviceContext> GetDeviceContext() const;

private:
	void LoadShaders();

private:
	std::unique_ptr<RenderingContext> _renderingContext;
	std::vector<std::unique_ptr<Shader>> _shaders;
	Shader::ShaderType _activeShaderType;

};