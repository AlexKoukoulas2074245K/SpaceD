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

class Scene final
{
public:
	Scene(Renderer& renderer, Camera& ref);
	~Scene();

	void InsertEntity(std::shared_ptr<GameEntity> entity);
	void InsertPointLight(std::shared_ptr<PointLight> pointLight);
	void InsertDirectionalLight(std::shared_ptr<DirectionalLight> directionalLight);

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

	bool IsOutOfBounds(const GameEntity& entity) const;
	CellCoords GetCellCoords(const GameEntity& entity) const;

private:
	Renderer& _renderer;
	Camera& _camera;

	Cell _sceneGraph[CELL_ROWS][CELL_COLS];
	std::vector<std::shared_ptr<GameEntity>> _outOfBoundsObjects;
	std::vector<std::shared_ptr<PointLight>> _pointLights;
	std::vector<std::shared_ptr<DirectionalLight>> _directionalLights;

	std::unique_ptr<Model> _sceneCellModel;

	comptr<ID3D11ShaderResourceView> _defaultCellTexture;
	comptr<ID3D11ShaderResourceView> _activatedCellTexture;
};