/**********************************************************************/
/** scene.cpp by Alex Koukoulas (C) 2017 All Rights Reserved         **/
/** File Description:                                                **/
/**********************************************************************/

#pragma once

// Local Headers
#include "camera.h"
#include "scene.h"
#include "gameentities/gameentity.h"
#include "rendering/models/model.h"
#include "rendering/textureloader.h"
#include "rendering/renderer.h"
#include "rendering/shaders/shader.h"
#include "rendering/shaders/default3dshader.h"
#include "rendering/shaders/default3dwithlightingshader.h"

// Remote Headers
#include <unordered_map>

const FLOAT Scene::CELL_SIZE = 15.0f;

Scene::Scene(Renderer& renderer, Camera& camera)
	: _renderer(renderer)
	, _camera(camera)
{
	ConstructScene();
	_sceneCellModel = std::make_unique<Model>("debug_scene_cell");
	_sceneCellModel->LoadModelComponents(_renderer.GetDevice());

	_defaultCellTexture = TextureLoader::Get().LoadTexture("../res/models/debug_scene_cell/debug_scene_cell.png", _renderer.GetDevice());
	_activatedCellTexture = TextureLoader::Get().LoadTexture("../res/models/debug_scene_cell/debug_scene_cell_activated.png", _renderer.GetDevice());
}

Scene::~Scene()
{
}

void Scene::InsertEntity(std::shared_ptr<GameEntity> entity)
{		
	if (IsOutOfBounds(*entity))
	{
		_outOfBoundsObjects.push_back(entity);
		return;
	}

	const auto cellCoords = GetCellCoords(*entity);
	
	_sceneGraph[cellCoords._row][cellCoords._col]._residents.push_back(entity);
}

void Scene::InsertPointLight(std::shared_ptr<PointLight> pointLight)
{
	_pointLights.push_back(pointLight);
}

void Scene::InsertDirectionalLight(std::shared_ptr<DirectionalLight> directionalLight)
{
	_directionalLights.push_back(directionalLight);
}

void Scene::Update(const FLOAT deltaTime)
{	
	// Transit objects ready to be inserted into the actual scene graph
	std::unordered_map<UINT, std::shared_ptr<GameEntity>> residentsToBeAdded;

	// Update out of bounds objects and add them to the transit list
	// if they cross the scene graph's bounds
	const auto outOfBoundsCount = _outOfBoundsObjects.size();
	for (auto i = 0U; i < outOfBoundsCount; ++i)
	{
		auto& entity = _outOfBoundsObjects[i];
		entity->Update(deltaTime);

		if (!IsOutOfBounds(*entity))
		{			
			const auto cellCoords = GetCellCoords(*entity);
			_outOfBoundsObjects.erase(_outOfBoundsObjects.begin() + i);
			residentsToBeAdded[cellCoords._row * CELL_ROWS + cellCoords._col] = entity;
		}
	}

	// Update objects in sceneGraph and possibly move them to another cell or to the out of bounds list
	// if the necessary conditions are met
	for (auto y = 0U; y < CELL_ROWS; ++y)
	{
		for (auto x = 0U; x < CELL_COLS; ++x)
		{
			const auto residentCount = _sceneGraph[y][x]._residents.size();
			for (auto i = 0U; i < residentCount; ++i)
			{
				auto& cell = _sceneGraph[y][x];
				auto& entity = cell._residents[i];
				entity->Update(deltaTime);

				if (IsOutOfBounds(*entity))
				{
					_outOfBoundsObjects.push_back(entity);
					cell._residents.erase(cell._residents.begin() + i);
					continue;
				}

				const auto cellCoords = GetCellCoords(*entity);

				if (cellCoords._col != x || cellCoords._row != y)
				{
					residentsToBeAdded[cellCoords._row * CELL_ROWS + cellCoords._col] = entity;
					cell._residents.erase(cell._residents.begin() + i);
				}
			}
		}
	}

	// Add the residents in transit
	for (auto mapIter = residentsToBeAdded.begin(); mapIter != residentsToBeAdded.end(); ++mapIter)
	{
		_sceneGraph[mapIter->first / CELL_ROWS][mapIter->first % CELL_COLS]._residents.push_back(mapIter->second);
	}

	residentsToBeAdded.clear();
}

void Scene::Render()
{
#if defined (DEBUG) || defined (_DEBUG)

	// Debug Scene Graph Rendering
	_renderer.SetShader(Shader::ShaderType::DEFAULT_3D);
	for (auto y = 0U; y < CELL_ROWS; ++y)
	{
		for (auto x = 0U; x < CELL_COLS; ++x)
		{
			_sceneCellModel->GetTransform()._translation.x = _sceneGraph[y][x]._x;
			_sceneCellModel->GetTransform()._translation.z = _sceneGraph[y][x]._z;
			_sceneCellModel->GetTransform()._scale = XMFLOAT3(CELL_SIZE, CELL_SIZE, CELL_SIZE);

			if (_sceneGraph[y][x]._residents.size() > 0)
			{
				_sceneCellModel->SetTexture(_activatedCellTexture);
			}
			else
			{
				_sceneCellModel->SetTexture(_defaultCellTexture);
			}

			Default3dShader::ConstantBuffer cb;
			cb.gWorld = _sceneCellModel->CalculateWorldMatrix();
			cb.gWorldInvTranspose = math::InverseTranspose(cb.gWorld);
			cb.gWorldViewProj = cb.gWorld * _camera.GetViewMatrix() * _camera.GetProjectionMatrix();

			_renderer.RenderModel(*_sceneCellModel, &cb);
		}
	}
#endif

	_renderer.SetShader(Shader::ShaderType::DEFAULT_3D_WITH_LIGHTING);

	// Accumulate Lights
	Default3dWithLightingShader::ConstantBuffer cb = {};

	const auto directionalLightCount = _directionalLights.size();
	const auto pointLightCount = _pointLights.size();

	for (auto i = 0U; i < directionalLightCount; ++i)
	{
		cb.gDirectionalLights[cb.gDirectionalLightCount++] = *_directionalLights[i];
	}

	for (auto i = 0U; i < pointLightCount; ++i)
	{
		auto pointLight = _pointLights[i];
		cb.gPointLights[cb.gPointLightCount++] = *pointLight;

#if defined(DEBUG) || defined(_DEBUG)
		// Render Debug Point Light
		_renderer.RenderPointLight(pointLight->Position, pointLight->Range, _camera.GetViewMatrix(), _camera.GetProjectionMatrix());
#endif
	}
	
	for (auto y = 0U; y < CELL_ROWS; ++y)
	{
		for (auto x = 0U; x < CELL_COLS; ++x)
		{
			const auto residentCount = _sceneGraph[y][x]._residents.size();
			for (auto i = 0U; i < residentCount; ++i)
			{
				const auto& entity = _sceneGraph[y][x]._residents[i];
				const auto worldMatrix = entity->GetModel().CalculateWorldMatrix();

				auto wvp = worldMatrix * _camera.GetViewMatrix() * _camera.GetProjectionMatrix();
				
				cb.gEyePosW = _camera.GetPos();
				cb.gMaterial = entity->GetModel().GetMaterial();
				cb.gDirectionalLightCount = _directionalLights.size();
				cb.gPointLightCount = _pointLights.size();
				cb.gWorld = worldMatrix;
				cb.gWorldInvTranspose = math::InverseTranspose(worldMatrix);
				cb.gWorldViewProj = wvp;

				if (_camera.isVisible(entity->GetModel()))
				{
					_renderer.RenderModel(entity->GetModel(), &cb);
				}
			}
		}
	}

}

void Scene::ConstructScene()
{
	_outOfBoundsObjects.clear();
	_pointLights.clear();

	for (auto y = 0U; y < CELL_ROWS; ++y)
	{
		for (auto x = 0U; x < CELL_COLS; ++x)
		{
			_sceneGraph[y][x]._x = x * CELL_SIZE - (CELL_COLS * CELL_SIZE)/2;
			_sceneGraph[y][x]._z = y * CELL_SIZE - (CELL_ROWS * CELL_SIZE)/2;
		}
	}
}

bool Scene::IsOutOfBounds(const GameEntity& entity) const
{
	const auto& entityTranslation = entity.GetTranslation();

	return entityTranslation.x + CELL_SIZE/2 < -(CELL_COLS * CELL_SIZE) / 2 ||
		   entityTranslation.x + CELL_SIZE/2 >  (CELL_COLS * CELL_SIZE) / 2 ||
		   entityTranslation.z + CELL_SIZE/2 < -(CELL_ROWS * CELL_SIZE) / 2 ||
		   entityTranslation.z + CELL_SIZE/2 >  (CELL_COLS * CELL_SIZE) / 2;
}

Scene::CellCoords Scene::GetCellCoords(const GameEntity& entity) const
{
	const auto cellCol = static_cast<INT>(((entity.GetTranslation().x + CELL_SIZE/2) + (CELL_COLS * CELL_SIZE) / 2) / CELL_SIZE);
	const auto cellRow = static_cast<INT>(((entity.GetTranslation().z + CELL_SIZE/2) + (CELL_ROWS * CELL_SIZE) / 2) / CELL_SIZE);
	return CellCoords(cellCol, cellRow);
}