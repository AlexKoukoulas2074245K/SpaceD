/***********************************************************************/
/** default3dshader.h by Alex Koukoulas (C) 2017 All Rights Reserved  **/
/** File Description:                                                 **/
/***********************************************************************/

#pragma once

// Local Headers
#include "shader.h"
#include "../../util/math.h"

// Remote Headers

// Forward declare friends
class Renderer;

class Default3dShader: public Shader
{
	friend class Renderer;

public:
	struct ConstantBuffer
	{
		XMMATRIX gWorld;
		XMMATRIX gWorldInvTranspose;
		XMMATRIX gWorldViewProj;
	};

public:
	~Default3dShader();

private:
	Default3dShader(comptr<ID3D11Device> device);

protected:
	void PrepareConstantBuffersAndLayout(comptr<ID3D11Device> device) override;

};