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

const FLOAT Scene::CELL_SIZE = 10.0f;

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
	const auto cellCol = static_cast<INT>((model->GetTransform().translation.x + (CELL_COLS * CELL_SIZE) / 2) / CELL_SIZE);
	const auto cellRow = static_cast<INT>((model->GetTransform().translation.z + (CELL_ROWS * CELL_SIZE) / 2) / CELL_SIZE);

	_sceneGraph[cellRow][cellCol]._residents.push_back(model);
}

void Scene::Update(const FLOAT deltaTime)
{	
	std::unordered_map<UINT, std::shared_ptr<Model>> _displacedResidents;
	
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

				const auto cellCol = static_cast<INT>((model->GetTransform().translation.x + (CELL_COLS * CELL_SIZE) / 2) / CELL_SIZE);
				const auto cellRow = static_cast<INT>((model->GetTransform().translation.z + (CELL_ROWS * CELL_SIZE) / 2) / CELL_SIZE);

				if (cellCol != x || cellRow != y)
				{
					_displacedResidents[cellRow * CELL_ROWS + cellCol] = model;
					cell._residents.erase(cell._residents.begin() + i);
				}
			}
		}
	}

	for (auto mapIter = _displacedResidents.begin(); mapIter != _displacedResidents.end(); ++mapIter)
	{
		_sceneGraph[mapIter->first / CELL_ROWS][mapIter->first % CELL_COLS]._residents.push_back(mapIter->second);
	}

	_displacedResidents.clear();
}

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