/***********************************************************************************/
/** default3dwithlightingshader.h by Alex Koukoulas (C) 2017 All Rights Reserved  **/
/** File Description:                                                             **/
/***********************************************************************************/

#pragma once

// Local Headers
#include "shader.h"
#include "../../util/math.h"

// Remote Headers

// Forward declare friends
class Renderer;

class Default3dWithLightingShader: public Shader
{
	friend class Renderer;

public:
	static const UINT MAX_POINT_LIGHTS = 16U;

public:
	struct DirectionalLight
	{
		DirectionalLight() { ZeroMemory(this, sizeof(this)); }

		XMFLOAT4 Ambient;
		XMFLOAT4 Diffuse;
		XMFLOAT4 Specular;
		XMFLOAT3 Direction;
		float Pad; // Pad the last float so we can set an array of lights if we wanted.
	};

	struct PointLight
	{
		PointLight() { ZeroMemory(this, sizeof(this)); }

		XMFLOAT4 Ambient;
		XMFLOAT4 Diffuse;
		XMFLOAT4 Specular;

		// Packed into 4D vector: (Position, Range)
		XMFLOAT3 Position;
		float Range;

		// Packed into 4D vector: (A0, A1, A2, Pad)
		XMFLOAT3 Att;
		float Pad; // Pad the last float so we can set an array of lights if we wanted.
	};

	struct SpotLight
	{
		SpotLight() { ZeroMemory(this, sizeof(this)); }

		XMFLOAT4 Ambient;
		XMFLOAT4 Diffuse;
		XMFLOAT4 Specular;

		// Packed into 4D vector: (Position, Range)
		XMFLOAT3 Position;
		float Range;

		// Packed into 4D vector: (Direction, Spot)
		XMFLOAT3 Direction;
		float Spot;

		// Packed into 4D vector: (Att, Pad)
		XMFLOAT3 Att;
		float Pad; // Pad the last float so we can set an array of lights if we wanted.
	};

	struct Material
	{
		Material() { ZeroMemory(this, sizeof(this)); }

		XMFLOAT4 Ambient;
		XMFLOAT4 Diffuse;
		XMFLOAT4 Specular; // w = SpecPower
		XMFLOAT4 Reflect;
	};

	struct ConstantBuffer
	{
		XMMATRIX gWorld;
		XMMATRIX gWorldInvTranspose;
		XMMATRIX gWorldViewProj;
		Material gMaterial;
		DirectionalLight gDirLight;
		PointLight gPointLights[MAX_POINT_LIGHTS];
		SpotLight gSpotLight;
		XMFLOAT3 gEyePosW;
		int gPointLightCount;
	};

public:
	~Default3dWithLightingShader();

private:
	Default3dWithLightingShader(comptr<ID3D11Device> device);

protected:
	void PrepareConstantBuffersAndLayout(comptr<ID3D11Device> device) override;
};