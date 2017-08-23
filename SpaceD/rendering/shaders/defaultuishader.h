/***********************************************************************/
/** defaultuishader.h by Alex Koukoulas (C) 2017 All Rights Reserved  **/
/** File Description:                                                 **/
/***********************************************************************/

#pragma once

// Local Headers
#include "shader.h"
#include "../../util/math.h"

// Remote Headers

class DefaultUiShader: Shader
{
	friend class Renderer;

public:
	struct ConstantBuffer
	{
		XMMATRIX gWorld;
	};

public:
	~DefaultUiShader();

private:
	DefaultUiShader(comptr<ID3D11Device> device);

protected:
	void PrepareConstantBuffersAndLayout(comptr<ID3D11Device> device) override;
};