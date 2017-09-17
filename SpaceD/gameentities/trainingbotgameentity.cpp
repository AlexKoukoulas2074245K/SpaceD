/*******************************************************************************/
/** trainingbotgameentity.cpp by Alex Koukoulas (C) 2017 All Rights Reserved  **/
/** File Description:                                                         **/
/*******************************************************************************/

#pragma once

// Local Headers
#include "trainingbotgameentity.h"
#include "projectilegameentity.h"
#include "../rendering/models/model.h"
#include "../scene.h"

// Remote Headers

// Constants
static const UINT ATTACK_TIMER = 60U;

TrainingBotGameEntity::TrainingBotGameEntity(Scene& scene, Renderer& renderer, const XMFLOAT3& pos)
	: GameEntity("enemy_training_bot", false, true, scene, renderer)
	, _attackTimer(ATTACK_TIMER)
	, _animState(AnimationState::IDLE)
{
	_model->GetTransform()._translation.x = pos.x;
	_model->GetTransform()._translation.y = pos.y;
	_model->GetTransform()._translation.z = pos.z;
}

TrainingBotGameEntity::~TrainingBotGameEntity()
{
}

void TrainingBotGameEntity::Update(const FLOAT deltaTime, const std::vector<std::shared_ptr<GameEntity>>& nearbyEntities)
{
	if (GetTranslation().z < -37.0f)
	{
		_model->GetTransform()._translation.z += deltaTime * 2;
	}

	for (auto nearbyEntity: nearbyEntities)	
	{
		if (nearbyEntity->IsProjectile() && !nearbyEntity->IsEnemy())
		{
			if (_model->CollidesWith(nearbyEntity->GetModel()))
			{
				// Damage calculation
				_scene.RemoveEntity(nearbyEntity);
			}			
		}
	}
	
	static float _animTargetRotAngle = 0.0f;

	switch (_animState)
	{
		case IDLE:
		{
			if (--_attackTimer == 0)
			{
				_attackTimer = ATTACK_TIMER;
				_scene.InsertEntity(std::make_shared<ProjectileGameEntity>("projectile_dps_basic", 0, false, XMFLOAT3(GetTranslation().x, GetTranslation().y, GetTranslation().z + GetDimensions()._depth / 1.4f), _scene, _renderer));
				_animState = AnimationState::ROT_RIGHT;
				_animTargetRotAngle = GetRotation().z - math::PI/2;
			}
		} break;

		case ROT_LEFT: break;
		case ROT_RIGHT: 
		{
			if (math::Lerp(GetRotation().z, _animTargetRotAngle, 6 * deltaTime, _model->GetTransform()._rotation.z))
			{
				_animState = AnimationState::IDLE;
			}
		} break;
	}

	
}