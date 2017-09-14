/******************************************************************************/
/** playershipgameentity.cpp by Alex Koukoulas (C) 2017 All Rights Reserved  **/
/** File Description:                                                        **/
/******************************************************************************/

#pragma once

// Local Headers
#include "playershipgameentity.h"
#include "../camera.h"
#include "../inputhandler.h"
#include "../rendering/models/model.h"

// Remote Headers

PlayerShipGameEntity::PlayerShipGameEntity(Renderer& renderer, const Camera& camera, const InputHandler& inputHandler)
	: ShipGameEntity("ship_dps", renderer)
	, _camera(camera)
	, _inputHandler(inputHandler)
	, _animState(AnimationState::IDLE)
	, _animTargetRotAngle(0.0f)
{
}

PlayerShipGameEntity::~PlayerShipGameEntity()
{
}

void PlayerShipGameEntity::Update(const FLOAT deltaTime)
{
	auto viewProj = _camera.GetViewMatrix() * _camera.GetProjectionMatrix();

	const XMFLOAT4 trans4(GetTranslation().x, GetTranslation().y, GetTranslation().z, 1.0f);
	auto transVec = XMLoadFloat4(&trans4);
	transVec = XMVector4Normalize(XMVector4Transform(transVec, viewProj));
	transVec *= 2;

	const auto ndcCoords = _inputHandler.GetMouseNDCCoords();
	const auto nDiff = math::absf(ndcCoords.x, XMVectorGetX(transVec));

	if (nDiff < 0.0005f)
	{
		_velocity.x = 0.0f;
	}
	else if (ndcCoords.x < XMVectorGetX(transVec))
	{
		_velocity.x = -200 * deltaTime * nDiff;
	}
	else if (ndcCoords.x > XMVectorGetX(transVec))
	{
		_velocity.x = 200 * deltaTime * nDiff;
	}

	const auto nDiffY = math::absf(ndcCoords.y, XMVectorGetY(transVec));

	if (nDiffY < 0.0005f)
	{
		_velocity.z = 0.0f;
	}
	else if (ndcCoords.y < XMVectorGetY(transVec))
	{
		_velocity.z = 200 * deltaTime * nDiffY;
	}
	else if (ndcCoords.y > XMVectorGetY(transVec))
	{
		_velocity.z = -200 * deltaTime * nDiffY;
	}

	switch (_animState)
	{
		case IDLE:
		{
			if (_velocity.x > 0.3f)
			{
				_animState = AnimationState::ROT_LEFT;
				_animTargetRotAngle = GetRotation().z - math::PI;
			}
			else if (_velocity.x < -0.3f)
			{
				_animState = AnimationState::ROT_RIGHT;
				_animTargetRotAngle = GetRotation().z + math::PI;
			}
		} break;
		case ROT_LEFT:
		{
			if (math::lerp(GetRotation().z, _animTargetRotAngle, 6 * deltaTime, _model->GetTransform()._rotation.z))
			{
				_animState = AnimationState::IDLE;
			}
		} break;
		case ROT_RIGHT:
		{
			if (math::lerp(GetRotation().z, _animTargetRotAngle, 6 * deltaTime, _model->GetTransform()._rotation.z))
			{
				_animState = AnimationState::IDLE;
			}
		} break;
	}	

	ShipGameEntity::Update(deltaTime);
}