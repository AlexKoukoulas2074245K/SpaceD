/**********************************************************************/
/** gameentity.cpp by Alex Koukoulas (C) 2017 All Rights Reserved    **/
/** File Description:                                                **/
/**********************************************************************/

#pragma once

// Local Headers
#include "gameentity.h"
#include "../camera.h"
#include "../inputhandler.h"
#include "../rendering/models/model.h"
#include "../rendering/renderer.h"

// Remote Headers
#include <sstream>

GameEntity::GameEntity(const std::string& modelName, const Camera& camera, const InputHandler& inputHandler, Renderer& renderer)
	: _camera(camera)
	, _inputHandler(inputHandler)
    , _renderer(renderer)
{
	LoadModel(modelName);
}

GameEntity::~GameEntity()
{
}

static bool rotLeft = false;
static bool rotRight = false;
static float targetRad = 0.0f;
static float shipVelY;
static float shipVelX;

void GameEntity::Update(const FLOAT deltaTime)
{
	auto viewProj = _camera.GetViewMatrix() * _camera.GetProjectionMatrix();
	
	XMFLOAT4 trans4(GetTranslation().x, GetTranslation().y, GetTranslation().z, 1.0f);
	auto transVec = XMLoadFloat4(&trans4);
	transVec = XMVector4Normalize(XMVector4Transform(transVec, viewProj));
	transVec *= 2;

	const auto ndcCoords = _inputHandler.GetMouseNDCCoords();
	const auto nDiff = math::absf(ndcCoords.x, XMVectorGetX(transVec));

	if (nDiff < 0.0005f)
	{
		shipVelX = 0.0f;
	}
	else if (ndcCoords.x < XMVectorGetX(transVec))
	{
		shipVelX = -200 * deltaTime * nDiff;
	}
	else if (ndcCoords.x > XMVectorGetX(transVec))
	{
		shipVelX = 200 * deltaTime * nDiff;
	}

	const auto nDiffY = math::absf(ndcCoords.y, XMVectorGetY(transVec));

	if (nDiffY < 0.0005f)
	{
		shipVelY = 0.0f;
	}
	else if (ndcCoords.y < XMVectorGetY(transVec))
	{
		shipVelY = 200 * deltaTime * nDiffY;
	}
	else if (ndcCoords.y > XMVectorGetY(transVec))
	{
		shipVelY = -200 * deltaTime * nDiffY;
	}

	_model->GetTransform()._translation.x += shipVelX;
	_model->GetTransform()._translation.z += shipVelY;

	if (shipVelX > 0.3f)
	{
		if (rotLeft || rotRight)
			return;

		rotLeft = true;
		targetRad = GetRotation().z - math::PI;
	}
	if (shipVelX < -0.3f)
	{
		if (rotLeft || rotRight)
			return;

		rotRight = true;
		targetRad = GetRotation().z + math::PI;
	}

	if (rotLeft)
	{
		if (math::lerp(GetRotation().z, targetRad, 6 * deltaTime, _model->GetTransform()._rotation.z))
		{
			rotLeft = false;
		}
	}
	else if (rotRight)
	{
		if (math::lerp(GetRotation().z, targetRad, 6 * deltaTime, _model->GetTransform()._rotation.z))
		{
			rotRight = false;
		}
	}
}

std::string GameEntity::GetBriefDescription() const
{
	std::stringstream descStream;
	descStream << "GameEntity at " << GetTranslation().x << ", " << GetTranslation().y << ", " << GetTranslation().z;
	return descStream.str();
}

std::vector<std::string> GameEntity::GetDetailedDescription() const
{
	std::vector<std::string> descVector;

	std::stringstream transDescStream;
	transDescStream << "Translation: " << GetTranslation().x << ", " << GetTranslation().y << ", " << GetTranslation().z;

	std::stringstream rotDescStream;
	rotDescStream   << "Rotation:    " << GetRotation().x << ", " << GetRotation().y << ", " << GetRotation().z;

	std::stringstream scaleDescStream;
	scaleDescStream << "Scale:       " << GetScale().x << ", " << GetScale().y << ", " << GetScale().z;

	std::stringstream dimsDescStream;
	dimsDescStream  << "Dimensions:  " << GetDimensions()._width << ", " << GetDimensions()._height << ", " << GetDimensions()._depth;

	descVector.push_back(transDescStream.str());
	descVector.push_back(rotDescStream.str());
	descVector.push_back(scaleDescStream.str());
	descVector.push_back(dimsDescStream.str());

	return descVector;
}

const Model& GameEntity::GetModel() const
{
	return *_model;
}

const math::Transform& GameEntity::GetTransform() const
{
	return _model->GetTransform();
}

const math::Dimensions& GameEntity::GetDimensions() const
{
	return _model->GetDimensions();
}

const XMFLOAT3& GameEntity::GetTranslation() const
{
	return _model->GetTransform().GetTranslation();
}

const XMFLOAT3& GameEntity::GetScale() const
{
	return _model->GetTransform().GetScale();
}

const XMFLOAT3& GameEntity::GetRotation() const
{
	return _model->GetTransform().GetRotation();
}

void GameEntity::LoadModel(const std::string& modelName)
{
	_model = std::make_unique<Model>(modelName);
	_model->LoadModelComponents(_renderer.GetDevice());
}