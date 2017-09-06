/**********************************************************************/
/** scene.cpp by Alex Koukoulas (C) 2017 All Rights Reserved         **/
/** File Description:                                                **/
/**********************************************************************/

#pragma once

// Local Headers
#include "camera.h"
#include "scene.h"
#include "rendering/models/model.h"
#include "rendering/textureloader.h"
#include "rendering/renderer.h"
#include "rendering/shaders/shader.h"
#include "rendering/shaders/default3dshader.h"

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

void Scene::InsertObect(std::shared_ptr<Model> model)
{	
	const auto& modelTrans = model->GetTransform().translation;
	if (modelTrans.x < -(CELL_COLS * CELL_SIZE) / 2 ||
		modelTrans.x >  (CELL_COLS * CELL_SIZE) / 2 ||
		modelTrans.z < -(CELL_ROWS * CELL_SIZE) / 2 ||
		modelTrans.z >  (CELL_COLS * CELL_SIZE) / 2)
	{
		_outOfBoundsObjects.push_back(model);
		return;
	}

	const auto cellCol = static_cast<INT>((model->GetTransform().translation.x + (CELL_COLS * CELL_SIZE) / 2) / CELL_SIZE);
	const auto cellRow = static_cast<INT>((model->GetTransform().translation.z + (CELL_ROWS * CELL_SIZE) / 2) / CELL_SIZE);

	_sceneGraph[cellRow][cellCol]._residents.push_back(model);
}

void Scene::Update(const FLOAT deltaTime)
{	
	// Transit objects ready to be inserted into the actual scene graph
	std::unordered_map<UINT, std::shared_ptr<Model>> residentsToBeAdded;

	// Update out of bounds objects and add them to the transit list
	// if they cross the scene graph's bounds
	const auto outOfBoundsCount = _outOfBoundsObjects.size();
	for (auto i = 0U; i < outOfBoundsCount; ++i)
	{
		// Update entities
		// ...
		
		auto& model = _outOfBoundsObjects[i];
		const auto& modelTrans = _outOfBoundsObjects[i]->GetTransform().translation;

		if (modelTrans.x > -(CELL_COLS * CELL_SIZE) / 2 &&
			modelTrans.x <  (CELL_COLS * CELL_SIZE) / 2 &&
			modelTrans.z > -(CELL_ROWS * CELL_SIZE) / 2 &&
			modelTrans.z <  (CELL_COLS * CELL_SIZE) / 2)
		{			
			const auto cellCol = static_cast<INT>((model->GetTransform().translation.x + (CELL_COLS * CELL_SIZE) / 2) / CELL_SIZE);
			const auto cellRow = static_cast<INT>((model->GetTransform().translation.z + (CELL_ROWS * CELL_SIZE) / 2) / CELL_SIZE);

			_outOfBoundsObjects.erase(_outOfBoundsObjects.begin() + i);
			residentsToBeAdded[cellRow * CELL_ROWS + cellCol] = model;
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
				// Update entities
				// ...

				auto& cell = _sceneGraph[y][x];
				auto& model = cell._residents[i];
				const auto& modelTrans = model->GetTransform().translation;

				if (modelTrans.x < -(CELL_COLS * CELL_SIZE) / 2 ||
					modelTrans.x >  (CELL_COLS * CELL_SIZE) / 2 ||
					modelTrans.z < -(CELL_ROWS * CELL_SIZE) / 2 ||
					modelTrans.z >  (CELL_COLS * CELL_SIZE) / 2)
				{
					_outOfBoundsObjects.push_back(model);
					cell._residents.erase(cell._residents.begin() + i);
					continue;
				}

				const auto cellCol = static_cast<INT>((model->GetTransform().translation.x + (CELL_COLS * CELL_SIZE) / 2) / CELL_SIZE);
				const auto cellRow = static_cast<INT>((model->GetTransform().translation.z + (CELL_ROWS * CELL_SIZE) / 2) / CELL_SIZE);

				if (cellCol != x || cellRow != y)
				{
					residentsToBeAdded[cellRow * CELL_ROWS + cellCol] = model;
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

#if defined (DEBUG) || defined (_DEBUG)
void Scene::Render()
{
	_renderer.SetShader(Shader::ShaderType::DEFAULT_3D);
	for (auto y = 0U; y < CELL_ROWS; ++y)
	{
		for (auto x = 0U; x < CELL_COLS; ++x)
		{
			_sceneCellModel->GetTransform().translation.x = _sceneGraph[y][x]._x;
			_sceneCellModel->GetTransform().translation.z = _sceneGraph[y][x]._z;
			_sceneCellModel->GetTransform().scale = XMFLOAT3(CELL_SIZE, CELL_SIZE, CELL_SIZE);

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
#else
void Scene::Render()
{
	for (auto y = 0U; y < CELL_ROWS; ++y)
	{
		for (auto x = 0U; x < CELL_COLS; ++x)
		{
			
		}
	}
}
#endif

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