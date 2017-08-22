/**********************************************************************/
/** camera.cpp by Alex Koukoulas (C) 2017 All Rights Reserved  **/
/** File Description:                                                **/
/**********************************************************************/

#pragma once

// Local Headers
#include "camera.h"
#include "util/clientwindow.h"
#include <D3DX11.h>
// Remote Headers
#include <cmath>

const XMFLOAT3 Camera::DEFAULT_FORWARD(0.0f, 0.0f, -1.0f);
const XMFLOAT3 Camera::DEFAULT_RIGHT(1.0f, 0.0f, 0.0f);
const XMFLOAT3 Camera::DEFAULT_UP(0.0f, 1.0f, 0.0f);

const FLOAT Camera::DEFAULT_FOV(math::PI * 0.25f);
const FLOAT Camera::DEFAULT_ZNEAR(1.0f);
const FLOAT Camera::DEFAULT_ZFAR(1000.0f);

Camera::Camera()
	: _pos(0.0f, 0.0f, 10.0f)
	, _forward(Camera::DEFAULT_FORWARD)
	, _right(Camera::DEFAULT_RIGHT)
	, _up(Camera::DEFAULT_UP)
	, _yaw(0.0f)
	, _pitch(0.0f)
	, _roll(0.0f)
{

}

Camera::~Camera()
{
}

void Camera::MoveCamera(const Direction direction, const FLOAT amount)
{
	switch (direction)
	{
		case FORWARD:
		{
			_pos.x -= std::sinf(_yaw)   * amount;
			_pos.y += std::sinf(_pitch) * amount;
			_pos.z -= std::cosf(_yaw)   * amount;
		} break;

		case BACKWARD:
		{
			_pos.x += std::sinf(_yaw)   * amount;
			_pos.y -= std::sinf(_pitch) * amount;
			_pos.z += std::cosf(_yaw)   * amount;
		} break;

		case UP:
		{
			_pos.y += amount;
		} break;

		case DOWN:
		{
			_pos.y -= amount;
		} break;

		case LEFT:
		{
			_pos.x -= std::sinf(_yaw + math::PI / 2) * amount;
			_pos.z -= std::cosf(_yaw + math::PI / 2) * amount;
		} break;

		case RIGHT:
		{
			_pos.x += std::sinf(_yaw + math::PI / 2) * amount;
			_pos.z += std::cosf(_yaw + math::PI / 2) * amount;
		} break;
	}
}

void Camera::RotateCamera(const Direction direction, const FLOAT amount)
{
	switch (direction)
	{
		case FORWARD:  _roll += amount; break;
		case BACKWARD: _roll -= amount; break;
		case UP:       _pitch += amount; break;
		case DOWN:     _pitch -= amount; break;
		case LEFT:     _yaw += amount; break;
		case RIGHT:    _yaw -= amount; break;
	}
}

void Camera::PanCamera(const Direction direction, const FLOAT amount)
{
	switch (direction)
	{
		case FORWARD:
		{
			_pos.x -= std::sinf(_yaw) * amount;
			_pos.z -= std::cosf(_yaw) * amount;
		} break;

		case BACKWARD:
		{
			_pos.x += std::sinf(_yaw) * amount;
			_pos.z += std::cosf(_yaw) * amount;
		} break;

		case UP:
		{
			_pos.y -= std::sinf(_pitch) * amount;
		} break;

		case DOWN:
		{
			_pos.y += std::sinf(_pitch) * amount;
		} break;
 
		case LEFT:
		{
			_pos.x -= std::sinf(_yaw + math::PI / 2) * amount;
			_pos.z -= std::cosf(_yaw + math::PI / 2) * amount;
		} break;

		case RIGHT:
		{
			_pos.x += std::sinf(_yaw + math::PI / 2) * amount;
			_pos.z += std::cosf(_yaw + math::PI / 2) * amount;
		} break;
	}
}

void Camera::CalculateViewMatrix(XMMATRIX& outViewMatrix) const
{	

	auto up_vec = XMVectorSet(DEFAULT_UP.x, DEFAULT_UP.y, DEFAULT_UP.z, 1.0f);
	auto forward_vec = XMVectorSet(DEFAULT_FORWARD.x, DEFAULT_FORWARD.y, DEFAULT_FORWARD.z, 1.0f);
	auto right_vec = XMVectorSet(DEFAULT_RIGHT.x, DEFAULT_RIGHT.y, DEFAULT_RIGHT.z, 1.0f);

	auto yawMatrix = XMMatrixRotationAxis(up_vec, _yaw);
	forward_vec = XMVector3TransformCoord(forward_vec, yawMatrix);
	right_vec = XMVector3TransformCoord(right_vec, yawMatrix);

	auto pitchMatrix = XMMatrixRotationAxis(right_vec, _pitch);
	forward_vec = XMVector3TransformCoord(forward_vec, pitchMatrix);
	up_vec = XMVector3TransformCoord(up_vec, pitchMatrix);

	auto rollMatrix = XMMatrixRotationAxis(forward_vec, _roll);
	right_vec = XMVector3TransformCoord(right_vec, rollMatrix);
	up_vec = XMVector3TransformCoord(up_vec, rollMatrix);

	XMFLOAT3 right, up, forward;
	XMStoreFloat3(&right, right_vec);
	XMStoreFloat3(&up, up_vec);
	XMStoreFloat3(&forward, forward_vec);

	auto pos_vec = XMLoadFloat3(&_pos);

	XMFLOAT4X4 viewMatrix;
	viewMatrix._11 = right.x;                                         viewMatrix._12 = up.x;                                         viewMatrix._13 = forward.x;                                         viewMatrix._14 = 0.0f;
	viewMatrix._21 = right.y;                                         viewMatrix._22 = up.y;                                         viewMatrix._23 = forward.y;                                         viewMatrix._24 = 0.0f;
	viewMatrix._31 = right.z;                                         viewMatrix._32 = up.z;                                         viewMatrix._33 = forward.z;                                         viewMatrix._34 = 0.0f;
	viewMatrix._41 = XMVectorGetX(-XMVector3Dot(right_vec, pos_vec)); viewMatrix._42 = XMVectorGetY(-XMVector3Dot(up_vec, pos_vec)); viewMatrix._43 = XMVectorGetZ(-XMVector3Dot(forward_vec, pos_vec)); viewMatrix._44 = 1.0f;
	
	outViewMatrix = XMLoadFloat4x4(&viewMatrix);
}

void Camera::CalculateProjectionMatrix(const ClientWindow& clientWindow, XMMATRIX& outProjMatrix) const
{
	outProjMatrix = XMMatrixPerspectiveFovLH(DEFAULT_FOV, clientWindow.GetAspectRatio(), DEFAULT_ZNEAR, DEFAULT_ZFAR);
}

const XMFLOAT3& Camera::GetPos() const
{
	return _pos;
}
