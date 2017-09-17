/********************************************************************/
/** scene.h by Alex Koukoulas (C) 2017 All Rights Reserved         **/
/** File Description:                                              **/
/********************************************************************/

#pragma once

// Local Headers
#include "rendering/d3dcommon.h"
#include "rendering/lightdef.h"
#include "util/math.h"

// Remote Headers
#include <memory>
#include <vector>

class Renderer;
class Camera;
class Model;
class GameEntity;
class InputHandler;
class ClientWindow;
class DebugPrompt;

class Scene final
{
public:	
	friend class DebugPrompt;

	Scene(Renderer& renderer, Camera& camera, const InputHandler& inputHandler, const ClientWindow& window);
	~Scene();

	void InsertEntity(std::shared_ptr<GameEntity> entity);
	void InsertPointLight(std::shared_ptr<PointLight> pointLight);
	void InsertDirectionalLight(std::shared_ptr<DirectionalLight> directionalLight);

	std::shared_ptr<GameEntity> GetEntityByIndex(const UINT entityIndex) const;	
	std::shared_ptr<PointLight> GetPointLightByIndex(const UINT pointLightIndex) const;
	std::shared_ptr<DirectionalLight> GetDirectionalLightByIndex(const UINT directionalLightIndex) const;

	void RemoveEntity(std::shared_ptr<GameEntity> gameEntity);
	void RemoveEntityByIndex(const UINT entityIndex);
	void RemovePointLightByIndex(const UINT pointLightIndex);
	void RemoveDirectionalLightByIndex(const UINT dirLightIndex);

	void Update(const FLOAT deltaTime);
	void Render();

private:
	static const UINT CELL_ROWS = 6U;
	static const UINT CELL_COLS = 6U;
	static const FLOAT CELL_SIZE;

private:
	struct CellCoords
	{
		INT _col;
		INT _row;

		CellCoords(const INT col, const INT row)
			: _col(col)
			, _row(row)
		{
		}
	};

	struct Cell
	{
		FLOAT _x;
		FLOAT _z;

		std::vector<std::shared_ptr<GameEntity>> _residents;

		Cell()
			: _x(0.0f)
			, _z(0.0f)
		{
		}

		Cell(const FLOAT x, const FLOAT z)
			: _x(x)
			, _z(z)
		{
		}
	};

private:
	void ConstructScene();
	
	void UpdateCamera(const FLOAT deltaTime);
	void UpdateEntities(const FLOAT deltaTime);
	void UpdateBackground(const FLOAT deltaTime);

	void DebugRenderScene();
	void DebugRenderLights();
	void RenderEntities();
public:
	bool IsOutOfBounds(const GameEntity& entity) const;
	CellCoords GetCellCoords(const GameEntity& entity) const;

private:
	Renderer& _renderer;
	Camera& _camera;
	const InputHandler& _inputHandler;
	const ClientWindow& _window;

	Cell _sceneGraph[CELL_ROWS][CELL_COLS];
	std::vector<std::shared_ptr<GameEntity>> _outOfBoundsObjects;
	std::vector<std::shared_ptr<PointLight>> _pointLights;
	std::vector<std::shared_ptr<DirectionalLight>> _directionalLights;
	std::unique_ptr<Model> _background;
	std::unique_ptr<Model> _sceneCellModel;

	comptr<ID3D11ShaderResourceView> _defaultCellTexture;
	comptr<ID3D11ShaderResourceView> _activatedCellTexture;

	XMFLOAT2 _backgroundOffset;
};