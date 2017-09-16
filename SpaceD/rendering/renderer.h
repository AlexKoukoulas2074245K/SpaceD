/********************************************************************/
/** renderer.h by Alex Koukoulas (C) 2017 All Rights Reserved      **/
/** File Description:                                              **/
/********************************************************************/

#pragma once

// Local Headers
#include "d3dcommon.h"
#include "../util/math.h"
#include "shaders/shader.h"

// Remote Headers
#include <memory>
#include <vector>

// Forward declarations
class RenderingContext;
class ClientWindow;
class FontEngine;
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
	void RenderText(const FLOAT text, const XMFLOAT2& pos, const XMFLOAT4& color);
	void RenderText(const INT text, const XMFLOAT2& pos, const XMFLOAT4& color);
	void RenderText(const std::string& text, const XMFLOAT2& pos, const XMFLOAT4& color);
	void RenderDebugSphere(const XMFLOAT3& pos, const XMFLOAT3& scale, const XMMATRIX& viewMatrix, const XMMATRIX& projMatrix);
	void RenderPointLight(const XMFLOAT3& pos, const FLOAT range, const XMMATRIX& viewMatrix, const XMMATRIX& projMatrix);
	void RenderModel(const Model& model, const void* constantBufferData);

	void SetDepthStencilEnabled(const bool depthStencilEnabled);

	comptr<ID3D11Device> GetDevice() const;
	comptr<ID3D11DeviceContext> GetDeviceContext() const;

private:
	void LoadShaders();
	void LoadFonts();
	void LoadDebugAssets();

private:
	std::unique_ptr<Model> _debugSphereModel;
	std::unique_ptr<FontEngine> _fontEngine;
	std::unique_ptr<RenderingContext> _renderingContext;
	std::vector<std::unique_ptr<Shader>> _shaders;
	Shader::ShaderType _activeShaderType;
	ClientWindow& _clientWindow;

};