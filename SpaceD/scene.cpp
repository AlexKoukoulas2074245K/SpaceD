/**********************************************************************/
/** scene.cpp by Alex Koukoulas (C) 2017 All Rights Reserved         **/
/** File Description:                                                **/
/**********************************************************************/

#pragma once

// Local Headers
#include "camera.h"
#include "scene.h"
#include "inputhandler.h"
#include "gameentities/gameentity.h"
#include "rendering/models/model.h"
#include "rendering/textureloader.h"
#include "rendering/renderer.h"
#include "rendering/shaders/shader.h"
#include "rendering/shaders/default3dshader.h"
#include "rendering/shaders/default3dwithlightingshader.h"
#include "rendering/shaders/defaultuishader.h"

// Remote Headers
#include <unordered_map>

const FLOAT Scene::CELL_SIZE = 15.0f;

Scene::Scene(Renderer& renderer, Camera& camera, const InputHandler& inputHandler, const ClientWindow& clientWindow)
	: _renderer(renderer)
	, _camera(camera)
	, _inputHandler(inputHandler)
	, _window(clientWindow)
	, _backgroundOffset(0.0f, 0.0f)
{
	ConstructScene();
	_sceneCellModel = std::make_unique<Model>("debug_scene_cell");
	_sceneCellModel->LoadModelComponents(_renderer.GetDevice());

	_defaultCellTexture = TextureLoader::Get().LoadTexture("../res/models/debug_scene_cell/debug_scene_cell.png", _renderer.GetDevice());
	_activatedCellTexture = TextureLoader::Get().LoadTexture("../res/models/debug_scene_cell/debug_scene_cell_activated.png", _renderer.GetDevice());

	_background = std::make_unique<Model>("background_space");
	_background->LoadModelComponents(_renderer.GetDevice());

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
	if (_pointLights.size() + 1 <= Default3dWithLightingShader::MAX_POINT_LIGHTS)
	{
		_pointLights.push_back(pointLight);
	}
}

void Scene::InsertDirectionalLight(std::shared_ptr<DirectionalLight> directionalLight)
{
	if (_directionalLights.size() + 1 <= Default3dWithLightingShader::MAX_DIRECTIONAL_LIGHTS)
	{
		_directionalLights.push_back(directionalLight);
	}
}

std::shared_ptr<GameEntity> Scene::GetEntityByIndex(const UINT entityIndex) const
{
	auto indexCounter = 0;

	for (auto y = 0U; y < CELL_ROWS; ++y)
	{
		for (auto x = 0U; x < CELL_COLS; ++x)
		{
			const auto residentCount = _sceneGraph[y][x]._residents.size();
			for (auto i = 0U; i < residentCount; ++i)
			{
				if (indexCounter++ == entityIndex)
				{
					return _sceneGraph[y][x]._residents[i];
				}
			}
		}
	}
	return nullptr;
}

std::shared_ptr<PointLight> Scene::GetPointLightByIndex(const UINT pointLightIndex) const
{
	if (pointLightIndex < _pointLights.size())
	{
		return _pointLights[pointLightIndex];
	}
	return nullptr;
}

std::shared_ptr<DirectionalLight> Scene::GetDirectionalLightByIndex(const UINT directionalLightIndex) const
{
	if (directionalLightIndex < _directionalLights.size())
	{
		return _directionalLights[directionalLightIndex];
	}
	return nullptr;
}

void Scene::RemoveEntityByIndex(const UINT entityIndex)
{
	auto indexCounter = 0;

	for (auto y = 0U; y < CELL_ROWS; ++y)
	{
		for (auto x = 0U; x < CELL_COLS; ++x)
		{
			const auto residentCount = _sceneGraph[y][x]._residents.size();
			for (auto i = 0U; i < residentCount; ++i)
			{
				if (indexCounter++ == entityIndex)
				{
					_sceneGraph[y][x]._residents.erase(_sceneGraph[y][x]._residents.begin() + i);
					return;
				}
			}
		}
	}	
}

void Scene::RemovePointLightByIndex(const UINT pointLightIndex)
{
	if (pointLightIndex < _pointLights.size())
	{
		_pointLights.erase(_pointLights.begin() + pointLightIndex);	
	}
}

void Scene::RemoveDirectionalLightByIndex(const UINT dirLightIndex)
{
	if (dirLightIndex < _directionalLights.size())
	{
		_directionalLights.erase(_directionalLights.begin() + dirLightIndex);
	}	
}

void Scene::Update(const FLOAT deltaTime)
{	
	UpdateCamera(deltaTime);
	UpdateEntities(deltaTime);
	UpdateBackground(deltaTime);
}

void Scene::Render()
{
#if defined(DEBUG) || defined(_DEBUG)
 	//DebugRenderScene();
	DebugRenderLights();	
#endif
	RenderEntities();
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

void Scene::UpdateCamera(const FLOAT deltaTime)
{		
	if (_inputHandler.IsKeyDown(InputHandler::LEFT)) _camera.RotateCamera(Camera::LEFT, deltaTime * 4);
	if (_inputHandler.IsKeyDown(InputHandler::RIGHT)) _camera.RotateCamera(Camera::RIGHT, deltaTime * 4);
	if (_inputHandler.IsKeyDown(InputHandler::UP)) _camera.RotateCamera(Camera::UP, deltaTime * 4);
	if (_inputHandler.IsKeyDown(InputHandler::DOWN)) _camera.RotateCamera(Camera::DOWN, deltaTime * 4);
	if (_inputHandler.IsKeyDown(InputHandler::W)) _camera.MoveCamera(Camera::FORWARD, deltaTime * 4);
	if (_inputHandler.IsKeyDown(InputHandler::A)) _camera.MoveCamera(Camera::LEFT, deltaTime * 4);
	if (_inputHandler.IsKeyDown(InputHandler::S)) _camera.MoveCamera(Camera::BACKWARD, deltaTime * 4);
	if (_inputHandler.IsKeyDown(InputHandler::D)) _camera.MoveCamera(Camera::RIGHT, deltaTime * 4);
	if (_inputHandler.IsKeyDown(InputHandler::Q)) _camera.MoveCamera(Camera::UP, deltaTime * 4);
	if (_inputHandler.IsKeyDown(InputHandler::E)) _camera.MoveCamera(Camera::DOWN, deltaTime * 4);

	_camera.Update(_window);
}

void Scene::UpdateEntities(const FLOAT deltaTime)
{
	// Transit objects ready to be inserted into the actual scene graph
	std::unordered_map<UINT, std::shared_ptr<GameEntity>> residentsToBeAdded;

	// Update out of bounds objects and add them to the transit list
	// if they cross the scene graph's bounds
	auto outOfBoundsCount = _outOfBoundsObjects.size();
	for (auto i = 0U; i < outOfBoundsCount; ++i)
	{
		auto& entity = _outOfBoundsObjects[i];
		entity->Update(deltaTime);

		if (!IsOutOfBounds(*entity))
		{
			const auto cellCoords = GetCellCoords(*entity);
			_outOfBoundsObjects.erase(_outOfBoundsObjects.begin() + i);
			outOfBoundsCount--;
			residentsToBeAdded[cellCoords._row * CELL_ROWS + cellCoords._col] = entity;
		}
	}

	// Update objects in sceneGraph and possibly move them to another cell or to the out of bounds list
	// if the necessary conditions are met
	for (auto y = 0U; y < CELL_ROWS; ++y)
	{
		for (auto x = 0U; x < CELL_COLS; ++x)
		{
			auto residentCount = _sceneGraph[y][x]._residents.size();
			for (auto i = 0U; i < residentCount; ++i)
			{
				auto& cell = _sceneGraph[y][x];
				auto entity = cell._residents[i];
				entity->Update(deltaTime);

				// Recalculate resident count due to possible entity spawns from previous entity update
				residentCount = _sceneGraph[y][x]._residents.size();

				if (IsOutOfBounds(*entity))
				{
					if (!entity->ShouldBeDestroyWhenOutOfBounds())
					{
						_outOfBoundsObjects.push_back(entity);
					}
					
					cell._residents.erase(cell._residents.begin() + i);
					residentCount--;
					continue;
				}

				const auto cellCoords = GetCellCoords(*entity);

				if (cellCoords._col != x || cellCoords._row != y)
				{
					residentsToBeAdded[cellCoords._row * CELL_ROWS + cellCoords._col] = entity;
					cell._residents.erase(cell._residents.begin() + i);
					residentCount--;
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

void Scene::UpdateBackground(const FLOAT deltaTime)
{
	_backgroundOffset.y -= 0.005f * deltaTime;
}

void Scene::DebugRenderScene()
{
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
}

void Scene::DebugRenderLights()
{
	_renderer.SetShader(Shader::ShaderType::DEFAULT_3D);

	const auto pointLightCount = _pointLights.size();

	for (auto i = 0U; i < pointLightCount; ++i)
	{
		auto pointLight = _pointLights[i];

		_renderer.RenderPointLight(pointLight->_position, pointLight->_range, _camera.GetViewMatrix(), _camera.GetProjectionMatrix());
	}
}

void Scene::RenderEntities()
{
	_renderer.SetDepthStencilEnabled(false);
	_renderer.SetShader(Shader::ShaderType::DEFAULT_UI);

	DefaultUiShader::ConstantBuffer bkgCb;
	bkgCb.gColorEnabled = false;
	bkgCb.gWorld = _background->CalculateWorldMatrix();
	bkgCb.gSrollTexCoordsEnabled = true;
	bkgCb.gTexCoordOffsets = XMFLOAT2(_backgroundOffset.x, _backgroundOffset.y);

	_renderer.RenderModel(*_background, &bkgCb);

	_renderer.SetDepthStencilEnabled(true);
	_renderer.SetShader(Shader::ShaderType::DEFAULT_3D_WITH_LIGHTING);

	// Accumulate Lights
	Default3dWithLightingShader::ConstantBuffer cb = {};

	const auto directionalLightCount = _directionalLights.size() > Default3dWithLightingShader::MAX_DIRECTIONAL_LIGHTS ? 
		                               Default3dWithLightingShader::MAX_DIRECTIONAL_LIGHTS : _directionalLights.size();

	const auto pointLightCount = _pointLights.size() > Default3dWithLightingShader::MAX_POINT_LIGHTS ? 
		                         Default3dWithLightingShader::MAX_POINT_LIGHTS : _pointLights.size();

	for (auto i = 0U; i < directionalLightCount; ++i)
	{
		cb.gDirectionalLights[cb.gDirectionalLightCount++] = *_directionalLights[i];
	}

	for (auto i = 0U; i < pointLightCount; ++i)
	{		
		cb.gPointLights[cb.gPointLightCount++] = *_pointLights[i];
	}

	// Render entities
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