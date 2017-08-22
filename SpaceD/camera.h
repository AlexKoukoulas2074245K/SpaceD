/********************************************************************/
/** camera.h by Alex Koukoulas (C) 2017 All Rights Reserved        **/
/** File Description:                                              **/
/********************************************************************/

#pragma once

// Local Headers
#include "rendering/d3dcommon.h"
#include "util/math.h"

// Remote Headers

class ClientWindow;

class Camera
{
public:
	enum Direction
	{
		FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN
	};

public:
	Camera();
	~Camera();

	void MoveCamera(const Direction direction, const FLOAT amount);
	void RotateCamera(const Direction direction, const FLOAT amount);
	void PanCamera(const Direction direction, const FLOAT amount);

	void CalculateViewMatrix(XMMATRIX& outViewMatrix) const;
	void CalculateProjectionMatrix(const ClientWindow& clientWindow, XMMATRIX& outProjMatrix) const;

	const XMFLOAT3& GetPos() const;

private:
	static const XMFLOAT3 DEFAULT_FORWARD;
	static const XMFLOAT3 DEFAULT_UP;
	static const XMFLOAT3 DEFAULT_RIGHT;

	static const FLOAT DEFAULT_FOV;
	static const FLOAT DEFAULT_ZNEAR;
	static const FLOAT DEFAULT_ZFAR;
private:
	XMFLOAT3 _pos;
	XMFLOAT3 _forward;
	XMFLOAT3 _right;
	XMFLOAT3 _up;

	FLOAT _yaw;
	FLOAT _pitch;
	FLOAT _roll;
};