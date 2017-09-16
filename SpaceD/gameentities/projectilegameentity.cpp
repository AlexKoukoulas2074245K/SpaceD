/******************************************************************************/
/** projectilegameentity.cpp by Alex Koukoulas (C) 2017 All Rights Reserved  **/
/** File Description:                                                        **/
/******************************************************************************/

#pragma once

// Local Headers
#include "projectilegameentity.h"
#include "../rendering/models/model.h"

// Remote Headers

ProjectileGameEntity::ProjectileGameEntity(const std::string& projectileName, const INT damage, const XMFLOAT3& pos, Scene& scene, Renderer& renderer)
	: GameEntity(projectileName, scene, renderer)
	, _velocity(0.0f, 0.0f, 0.0f)
	, _damage(0)
{
	_model->GetTransform()._translation.x = pos.x;
	_model->GetTransform()._translation.z = pos.z;
	_model->GetTransform()._scale = XMFLOAT3(2.0f, 2.0f, 2.0f);
	_shouldBeDestroyedWhenOutOfBounds = true;
}

ProjectileGameEntity::~ProjectileGameEntity()
{
}

void ProjectileGameEntity::Update(const FLOAT deltaTime)
{
	_velocity.z += -2.0f * deltaTime;
	
	_model->GetTransform()._translation.z += _velocity.z;
	_model->GetTransform()._rotation.z += deltaTime * 2;
}

std::string ProjectileGameEntity::GetBriefDescription() const
{
	return GameEntity::GetBriefDescription();
}

std::vector<std::string> ProjectileGameEntity::GetDetailedDescription() const
{
	return GameEntity::GetDetailedDescription();
}