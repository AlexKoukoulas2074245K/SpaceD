/***********************************************************************************/
/** default3dwithlightingshader.h by Alex Koukoulas (C) 2017 All Rights Reserved  **/
/** File Description:                                                             **/
/***********************************************************************************/

#pragma once

// Local Headers
#include "shader.h"
#include "../lightdef.h"
#include "../../util/math.h"

// Remote Headers

// Forward declare friends
class Renderer;

class Default3dWithLightingShader: public Shader
{
	friend class Renderer;

public:
	static const UINT MAX_POINT_LIGHTS = 16U;
	static const UINT MAX_DIRECTIONAL_LIGHTS = 4U;

public:
	struct ConstantBuffer
	{
		XMMATRIX gWorld;
		XMMATRIX gWorldInvTranspose;
		XMMATRIX gWorldViewProj;
		Material gMaterial;
		DirectionalLight gDirectionalLights[MAX_DIRECTIONAL_LIGHTS];
		PointLight gPointLights[MAX_POINT_LIGHTS];
		SpotLight gSpotLight;
		XMFLOAT3 gEyePosW;
		int gDirectionalLightCount;
		int gPointLightCount;
		XMFLOAT3 gPad;
	};

public:
	~Default3dWithLightingShader();

private:
	Default3dWithLightingShader(comptr<ID3D11Device> device);

protected:
	void PrepareConstantBuffersAndLayout(comptr<ID3D11Device> device) override;
};