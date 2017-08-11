/***********************************************************************/
/** default3dshader.h by Alex Koukoulas (C) 2017 All Rights Reserved  **/
/** File Description:                                                 **/
/***********************************************************************/

#pragma once

// Local Headers
#include "shader.h"

// Remote Headers
#include "../../util/math.h"

class Default3dShader: public Shader
{
public:
	struct ConstantBuffer
	{
		XMMATRIX gWorld;
		XMMATRIX gWorldInvTranspose;
		XMMATRIX gWorldViewProj;
		//Material gMaterial;
		//DirectionalLight gDirLights[3];
		//PointLight gPointLight;
		//SpotLight gSpotLight;
		//XMFLOAT3 gEyePosW;
		//int gLightCount;
	};

public:
	Default3dShader(comptr<ID3D11Device> device);
	~Default3dShader();

protected:
	void PrepareConstantBuffersAndLayout(comptr<ID3D11Device> device) override;

};