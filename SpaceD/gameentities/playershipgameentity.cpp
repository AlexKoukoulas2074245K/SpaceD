/******************************************************************************/
/** playershipgameentity.cpp by Alex Koukoulas (C) 2017 All Rights Reserved  **/
/** File Description:                                                        **/
/******************************************************************************/

#pragma once

// Local Headers
#include "playershipgameentity.h"
#include "projectilegameentity.h"
#include "../camera.h"
#include "../inputhandler.h"
#include "../scene.h"
#include "../rendering/models/model.h"

// Remote Headers


// Constants
static const UINT PROJECTILE_SPAWN_TIMER = 10;

PlayerShipGameEntity::PlayerShipGameEntity(Scene& scene, Renderer& renderer, const Camera& camera, const InputHandler& inputHandler)
	: GameEntity("ship_dps", false, false, scene, renderer)
	, _camera(camera)
	, _inputHandler(inputHandler)
	, _animState(AnimationState::IDLE)
	, _animTargetRotAngle(0.0f)
	, _projectileSpawnTimer(PROJECTILE_SPAWN_TIMER)
	, _velocity(0.0f, 0.0f, 0.0f)
{
}

PlayerShipGameEntity::~PlayerShipGameEntity()
{
}

void PlayerShipGameEntity::Update(const FLOAT deltaTime, const std::vector<std::shared_ptr<GameEntity>>& nearbyEntities)
{
	// Calculate velocity and position
	auto viewProj = _camera.GetViewMatrix() * _camera.GetProjectionMatrix();

	const XMFLOAT4 trans4(GetTranslation().x, GetTranslation().y, GetTranslation().z, 1.0f);
	auto transVec = XMLoadFloat4(&trans4);
	transVec = XMVector4Normalize(XMVector4Transform(transVec, viewProj));
	transVec *= 2;

	const auto ndcCoords = _inputHandler.GetMouseNDCCoords();
	const auto nDiff = math::Absf(ndcCoords.x, XMVectorGetX(transVec));

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

	const auto nDiffY = math::Absf(ndcCoords.y, XMVectorGetY(transVec));

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

	for (auto nearbyEntity: nearbyEntities)
	{
		if (nearbyEntity->IsProjectile() && nearbyEntity->IsEnemy())
		{
			if (_model->CollidesWith(nearbyEntity->GetModel()))
			{
				// Damage calculation
				_scene.RemoveEntity(nearbyEntity);
			}
		}
	}

	// Spawn projectiles
	if (_inputHandler.IsButtonDown(InputHandler::Button::LMBUTTON))
	{		
		if (--_projectileSpawnTimer == 0)
		{
			_scene.InsertEntity(std::make_shared<ProjectileGameEntity>("projectile_dps_basic", 0, true, XMFLOAT3(GetTranslation().x, GetTranslation().y, GetTranslation().z - GetDimensions()._depth/1.4f), _scene, _renderer));
			_projectileSpawnTimer = PROJECTILE_SPAWN_TIMER;
		}
	}

	// Animate
	switch (_animState)
	{
	    case IDLE: break;
		case ROT_LEFT:
		{
			if (math::Lerp(GetRotation().z, _animTargetRotAngle, 6 * deltaTime, _model->GetTransform()._rotation.z))
			{
				_animState = AnimationState::IDLE;
			}
		} break;
		case ROT_RIGHT:
		{
			if (math::Lerp(GetRotation().z, _animTargetRotAngle, 6 * deltaTime, _model->GetTransform()._rotation.z))
			{
				_animState = AnimationState::IDLE;
			}
		} break;
	}	

	_model->GetTransform()._translation.x += _velocity.x;
	_model->GetTransform()._translation.z += _velocity.z;
}