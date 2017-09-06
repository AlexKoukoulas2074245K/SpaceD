/**********************************************************************/
/** camera.cpp by Alex Koukoulas (C) 2017 All Rights Reserved  **/
/** File Description:                                                **/
/**********************************************************************/

#pragma once

// Local Headers
#include "camera.h"
#include "rendering/models/model.h"
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
	: _pos(0.0f, 90.0f, -5.0f)
	, _forward(Camera::DEFAULT_FORWARD)
	, _right(Camera::DEFAULT_RIGHT)
	, _up(Camera::DEFAULT_UP)
	, _yaw(0.0f)
	, _pitch(-math::PI/2.0f)
	, _roll(0.0f)
	, _viewMatrix(XMMatrixIdentity())
	, _projMatrix(XMMatrixIdentity())
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

bool Camera::isVisible(const Model& model)
{
	const auto pos3D = model.GetTransform().translation;
	const auto pos4D = XMFLOAT4(pos3D.x, pos3D.y, pos3D.z, 1.0f);
	const auto posVec = XMLoadFloat4(&pos4D);

	for (const auto& plane: _frustum._planes)
	{
		if (XMVectorGetX(XMPlaneDotCoord(plane, posVec)) < -model.GetBiggestDimensionRad())
		{
			return false;
		}
	}

	return true;
}

void Camera::Update(const ClientWindow& window)
{
	CalculateViewAndProjection(window);
	CalculateFrustum();
}

const XMMATRIX& Camera::GetViewMatrix() const
{	
	return _viewMatrix;
}

const math::Frustum& Camera::GetFrustum() const
{
	return _frustum;
}

const XMMATRIX& Camera::GetProjectionMatrix() const
{
	return _projMatrix;	
}

const XMFLOAT3& Camera::GetPos() const
{
	return _pos;
}

void Camera::CalculateViewAndProjection(const ClientWindow& window)
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

	_viewMatrix = XMLoadFloat4x4(&viewMatrix);
	_projMatrix = XMMatrixPerspectiveFovLH(DEFAULT_FOV, window.GetAspectRatio(), DEFAULT_ZNEAR, DEFAULT_ZFAR);
}

void Camera::CalculateFrustum()
{
	const auto r = DEFAULT_ZFAR / (DEFAULT_ZFAR - DEFAULT_ZNEAR);

	auto projMatrix = _projMatrix;
	auto viewMatrix = _viewMatrix;

	projMatrix._33 = r;
	projMatrix._43 = -r * DEFAULT_ZNEAR;

	const auto viewproj = viewMatrix * projMatrix;

	// Calculate near plane of frustum.
	XMFLOAT4 nearPlane = {};
	nearPlane.x = viewproj._14 + viewproj._13;
	nearPlane.y = viewproj._24 + viewproj._23;
	nearPlane.z = viewproj._34 + viewproj._33;
	nearPlane.w = viewproj._44 + viewproj._43;

	auto nearPlaneVec = XMLoadFloat4(&nearPlane);
	nearPlaneVec = XMPlaneNormalize(nearPlaneVec);

	_frustum._planes[0] = nearPlaneVec;

	// Calculate far plane of frustum.
	XMFLOAT4 farPlane = {};
	farPlane.x = viewproj._14 - viewproj._13;
	farPlane.y = viewproj._24 - viewproj._23;
	farPlane.z = viewproj._34 - viewproj._33;
	farPlane.w = viewproj._44 - viewproj._43;

	auto farPlaneVec = XMLoadFloat4(&farPlane);
	farPlaneVec = XMPlaneNormalize(farPlaneVec);

	_frustum._planes[1] = farPlaneVec;

	// Calculate left plane of frustum.
	XMFLOAT4 leftPlane = {};
	leftPlane.x = viewproj._14 + viewproj._11;
	leftPlane.y = viewproj._24 + viewproj._21;
	leftPlane.z = viewproj._34 + viewproj._31;
	leftPlane.w = viewproj._44 + viewproj._41;

	auto leftPlaneVec = XMLoadFloat4(&leftPlane);
	leftPlaneVec = XMPlaneNormalize(leftPlaneVec);

	_frustum._planes[2] = leftPlaneVec;

	// Calculate right plane of frustum.    
	XMFLOAT4 rightPlane = {};
	rightPlane.x = viewproj._14 - viewproj._11;
	rightPlane.y = viewproj._24 - viewproj._21;
	rightPlane.z = viewproj._34 - viewproj._31;
	rightPlane.w = viewproj._44 - viewproj._41;

	auto rightPlaneVec = XMLoadFloat4(&rightPlane);
	rightPlaneVec = XMPlaneNormalize(rightPlaneVec);

	_frustum._planes[3] = rightPlaneVec;

	// Calculate top plane of frustum.    
	XMFLOAT4 topPlane = {};
	topPlane.x = viewproj._14 - viewproj._12;
	topPlane.y = viewproj._24 - viewproj._22;
	topPlane.z = viewproj._34 - viewproj._32;
	topPlane.w = viewproj._44 - viewproj._42;

	auto topPlaneVec = XMLoadFloat4(&topPlane);
	topPlaneVec = XMPlaneNormalize(topPlaneVec);

	_frustum._planes[4] = topPlaneVec;

	// Calculate bottom plane of frustum.
	XMFLOAT4 botPlane = {};
	botPlane.x = viewproj._14 + viewproj._12;
	botPlane.y = viewproj._24 + viewproj._22;
	botPlane.z = viewproj._34 + viewproj._32;
	botPlane.w = viewproj._44 + viewproj._42;

	auto botPlaneVec = XMLoadFloat4(&botPlane);
	botPlaneVec = XMPlaneNormalize(botPlaneVec);

	_frustum._planes[5] = botPlaneVec;
}