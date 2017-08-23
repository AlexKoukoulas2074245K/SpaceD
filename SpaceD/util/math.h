/********************************************************************/
/** math.h by Alex Koukoulas (C) 2017 All Rights Reserved          **/
/** File Description:                                              **/
/********************************************************************/

#pragma once

// Local Headers

// Remote Headers
#include <xnamath.h>

namespace math
{
	static const FLOAT PI = 3.1415926f;

	struct Transform
	{
		XMFLOAT3 translation;
		XMFLOAT3 rotation;
		XMFLOAT3 scale;

		Transform()
			: translation(XMFLOAT3(0.0f, 0.0f, 0.0f))
			, rotation(XMFLOAT3(0.0f, 0.0f, 0.0f))
			, scale(XMFLOAT3(1.0f, 1.0f, 1.0f))
		{

		}
	};

	static XMMATRIX InverseTranspose(CXMMATRIX M)
	{
		// Inverse-transpose is just applied to normals.  So zero out 
		// translation row so that it doesn't get into our inverse-transpose
		// calculation--we don't want the inverse-transpose of the translation.
		XMMATRIX A = M;
		A.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		XMVECTOR det = XMMatrixDeterminant(A);
		return XMMatrixTranspose(XMMatrixInverse(&det, A));
	}

	static bool lerp(const FLOAT current, const FLOAT target, const FLOAT dt, FLOAT& outResult)
	{
		float fdiff = target - current;
		if (fdiff > dt) 
		{
			outResult += dt;
			return false; 
		}
		else if (fdiff < -dt)
		{
			outResult -= dt; 
			return false; 
		}
		else 
		{
			outResult = target; 
		    return true;
		}
	}
}
