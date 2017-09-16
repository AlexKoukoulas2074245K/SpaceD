/**********************************************************************/
/** gameentity.cpp by Alex Koukoulas (C) 2017 All Rights Reserved    **/
/** File Description:                                                **/
/**********************************************************************/

#pragma once

// Local Headers
#include "gameentity.h"
#include "../rendering/models/model.h"
#include "../rendering/renderer.h"
#include "../rendering/lightdef.h"

// Remote Headers
#include <sstream>

GameEntity::GameEntity(const std::string& modelName, Scene& scene, Renderer& renderer)	
	: _scene(scene)
	, _renderer(renderer)
	, _shouldBeDestroyedWhenOutOfBounds(false)
{
	LoadModel(modelName);
}

GameEntity::~GameEntity()
{
}

void GameEntity::Update(const FLOAT deltaTime)
{
	
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

	std::stringstream matAmbDescStream;
	matAmbDescStream << "Material Ambient: " << GetMaterial()._ambient.x << ", " << GetMaterial()._ambient.y << ", " << GetMaterial()._ambient.z << ", " << GetMaterial()._ambient.w;

	std::stringstream matDiffDescStream;
	matDiffDescStream << "Material Diffuse: " << GetMaterial()._diffuse.x << ", " << GetMaterial()._diffuse.y << ", " << GetMaterial()._diffuse.z << ", " << GetMaterial()._diffuse.w;

	std::stringstream matSpecDescStream;
	matSpecDescStream << "Material Specular: " << GetMaterial()._specular.x << ", " << GetMaterial()._specular.y << ", " << GetMaterial()._specular.z << ", " << GetMaterial()._specular.w;

	std::stringstream matReflectDescStream;
	matReflectDescStream << "Material Reflect: " << GetMaterial()._reflect.x << ", " << GetMaterial()._reflect.y << ", " << GetMaterial()._reflect.z << ", " << GetMaterial()._reflect.w;

	descVector.push_back(transDescStream.str());
	descVector.push_back(rotDescStream.str());
	descVector.push_back(scaleDescStream.str());
	descVector.push_back(dimsDescStream.str());
	descVector.push_back(matAmbDescStream.str());
	descVector.push_back(matDiffDescStream.str());
	descVector.push_back(matSpecDescStream.str());
	descVector.push_back(matReflectDescStream.str());


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

const Material& GameEntity::GetMaterial() const
{
	return _model->GetMaterial();
}

bool GameEntity::ShouldBeDestroyWhenOutOfBounds() const
{
	return _shouldBeDestroyedWhenOutOfBounds;
}

void GameEntity::LoadModel(const std::string& modelName)
{
	_model = std::make_unique<Model>(modelName);
	_model->LoadModelComponents(_renderer.GetDevice());
}