/********************************************************************/
/** math.h by Alex Koukoulas (C) 2017 All Rights Reserved          **/
/** File Description:                                              **/
/********************************************************************/

#pragma once

// Local Headers

// Remote Headers
#include <Windows.h>
#include <xnamath.h>
#include <vector>

typedef float FLOAT;

namespace math
{
	static const FLOAT PI = 3.1415926f;

	struct Transform
	{
		XMFLOAT3 _translation;
		XMFLOAT3 _rotation;
		XMFLOAT3 _scale;

		Transform()
			: _translation(0.0f, 0.0f, 0.0f)
			, _rotation(0.0f, 0.0f, 0.0f)
			, _scale(1.0f, 1.0f, 1.0f)
		{

		}

		const XMFLOAT3& GetTranslation() const { return _translation; }
		const XMFLOAT3& GetScale() const { return _scale; }
		const XMFLOAT3& GetRotation() const { return _rotation; }
	};

	struct Dimensions
	{
		FLOAT _width, _height, _depth;

		Dimensions(const FLOAT width, const FLOAT height, const FLOAT depth)
			: _width(width)
			, _height(height)
			, _depth(depth)
		{

		}

		Dimensions() 
			: _width(0.0f)
			, _height(0.0f)
			, _depth(0.0f)
		{

		}
	};

	struct Ray
	{
		Ray(const XMFLOAT3& pos, const XMFLOAT3& dir)
			: _position(pos)
			, _direction(dir)
		{
		}

		XMFLOAT3 _position;
		XMFLOAT3 _direction;
	};

	struct Frustum
	{
		Frustum()
		{
			ZeroMemory(_planes, ARRAYSIZE(_planes));
		}

		XMVECTOR _planes[6];
	};

	static XMFLOAT2 MouseToNDC(const INT mouseX, const INT mouseY, const INT windowWidth, const INT windowHeight)
	{
		FLOAT pointX, pointY;
		pointX = ((2.0f * mouseX) / (FLOAT)windowWidth) - 1.0f;
		pointY = -(((2.0f * mouseY) / (FLOAT)windowHeight) - 1.0f);
		
		return XMFLOAT2(pointX, pointY);
	}

	static Ray MouseToRay(const XMMATRIX& viewMatrix, const XMMATRIX& projMatrix, INT mouseX, INT mouseY, INT windowWidth, INT windowHeight, const XMFLOAT3& camPos)
	{
		// Move mouse coords in normalized screen coords (-1, 1)
		FLOAT pointX, pointY;
		pointX = ((2.0f * mouseX) / (FLOAT)windowWidth) - 1.0f;
		pointY = -(((2.0f * mouseY) / (FLOAT)windowHeight) - 1.0f);

		// Unproject mouse coords from projection
		pointX /= projMatrix._11;
		pointY /= projMatrix._22;
		
		// Get the inverse view matrix		
		const auto inverseView = XMMatrixInverse(0, viewMatrix);

		// Calculate ray direction in view
	    XMFLOAT3 direction, origin;
		direction.x = (pointX * inverseView._11) + (pointY * inverseView._21) + inverseView._31;
		direction.y = (pointX * inverseView._12) + (pointY * inverseView._22) + inverseView._32;
		direction.z = (pointX * inverseView._13) + (pointY * inverseView._23) + inverseView._33;

		// Get origin of picking ray from the cam pos
		origin = camPos;

		return Ray(origin, direction);
	}

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

	static bool Lerp(const FLOAT current, const FLOAT target, const FLOAT dt, FLOAT& outResult)
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

	static FLOAT Absf(const FLOAT a, const FLOAT b)
	{
		const auto dff = a - b;
		if (dff > 0) return dff;
		return -dff;
	}

	static FLOAT Max2f(const FLOAT a, const FLOAT b)
	{
		return a > b ? a : b;
	}

	static FLOAT Max3f(const FLOAT a, const FLOAT b, const FLOAT c)
	{
		return Max2f(a, Max2f(b, c));
	}	

	static FLOAT DistanceNoSqrt(const XMFLOAT3 pos1, const XMFLOAT3 pos2)
	{
		return (pos1.x - pos2.x) * (pos1.x - pos2.x) + 
			   (pos1.y - pos2.y) * (pos1.y - pos2.y) +
			   (pos1.z - pos2.z) * (pos1.z - pos2.z);
	}

	static FLOAT Distance(const XMFLOAT3 pos1, const XMFLOAT3 pos2)
	{
		return sqrtf(DistanceNoSqrt(pos1, pos2));
	}

	static bool NonZeroTexCoords(const std::vector<XMFLOAT2>& texcoordVec)
	{
		for (const auto& texcoord: texcoordVec)
		{
			if (texcoord.x != 0.0f || texcoord.y != 0.0f)
			{
				return true;
			}
		}

		return false;
	}
}
