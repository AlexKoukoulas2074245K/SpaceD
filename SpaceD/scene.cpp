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

void Scene::InsertObect(std::shared_ptr<GameEntity> entity)
{		
	if (IsOutOfBounds(*entity))
	{
		_outOfBoundsObjects.push_back(entity);
		return;
	}

	const auto cellCoords = GetCellCoords(*entity);
	
	_sceneGraph[cellCoords._row][cellCoords._col]._residents.push_back(entity);
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

			//_renderer.RenderModel(*_sceneCellModel, &cb);
		}
	}
	
	// Render Debug Lights
	for (auto y = 0U; y < 4; ++y)
	{
		for (auto x = 0U; x < 4; ++x)
		{
			_renderer.RenderDebugSphere(XMFLOAT3(-20.0f + x * 10.0f, 0.0f, -20.0f + y * 10.0f), XMFLOAT3(16.0f, 16.0f, 16.0f), _camera.GetViewMatrix(), _camera.GetProjectionMatrix());
		}
	}

#endif

	_renderer.SetShader(Shader::ShaderType::DEFAULT_3D_WITH_LIGHTING);
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

				Default3dWithLightingShader::ConstantBuffer cb;
				cb.gEyePosW = _camera.GetPos();

				cb.gMaterial.Ambient = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
				cb.gMaterial.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
				cb.gMaterial.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);

				cb.gDirLight.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
				cb.gDirLight.Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
				cb.gDirLight.Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
				cb.gDirLight.Direction = XMFLOAT3(0.0f, 1.0f, -1.0f);

				cb.gPointLightCount = Default3dWithLightingShader::MAX_POINT_LIGHTS;
				for (auto y = 0U; y < 4; ++y)
			    {
					for (auto x = 0U; x < 4; ++x)
					{
						cb.gPointLights[y * 4 + x].Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
						cb.gPointLights[y * 4 + x].Diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
						cb.gPointLights[y * 4 + x].Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
						cb.gPointLights[y * 4 + x].Att = XMFLOAT3(0.0f, 0.0f, 0.1f);
						cb.gPointLights[y * 4 + x].Position = XMFLOAT3(-20.0f + x * 10.0f, 0.0f, -20.0f + y * 10.0f);
						cb.gPointLights[y * 4 + x].Range = 8.0f;
					}
				}
				
				cb.gWorld = worldMatrix;
				cb.gWorldInvTranspose = math::InverseTranspose(worldMatrix);
				cb.gWorldViewProj = wvp;

				//_renderer->RenderDebugSphere(cb.gPointLight.Position, XMFLOAT3(4.0f, 4.0f, 4.0f), viewMatrix, projMatrix);

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