/************************************************************************/
/** shipgameentity.cpp by Alex Koukoulas (C) 2017 All Rights Reserved  **/
/** File Description:                                                  **/
/************************************************************************/

#pragma once

// Local Headers
#include "shipgameentity.h"
#include "../camera.h"
#include "../inputhandler.h"
#include "../rendering/models/model.h"

// Remote Headers

ShipGameEntity::ShipGameEntity(const std::string& shipName, Renderer& renderer)
	: GameEntity(shipName, renderer)
	, _velocity(0.0f, 0.0f, 0.0f)	
{
}

ShipGameEntity::~ShipGameEntity()
{
}

static float targetRad = 0.0f;


void ShipGameEntity::Update(const FLOAT deltaTime)
{
	_model->GetTransform()._translation.x += _velocity.x;
	_model->GetTransform()._translation.z += _velocity.z;
}

std::string ShipGameEntity::GetBriefDescription() const
{
	return GameEntity::GetBriefDescription();
}

std::vector<std::string> ShipGameEntity::GetDetailedDescription() const
{
	return GameEntity::GetDetailedDescription();
}