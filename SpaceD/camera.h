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
class Game;

class Camera
{
public:
	friend class Game;

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

	bool isVisible(const XMFLOAT3& pos, const FLOAT rad);

	// The calculation methods should be used before the accessors below 
	void CalculateViewAndProjection(const ClientWindow& window);
	void CalculateFrustum();

	const XMMATRIX& GetViewMatrix() const;
	const XMMATRIX& GetProjectionMatrix() const;
	const math::Frustum& GetFrustum() const;

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

	XMMATRIX _viewMatrix;
	XMMATRIX _projMatrix;

	math::Frustum _frustum;
};