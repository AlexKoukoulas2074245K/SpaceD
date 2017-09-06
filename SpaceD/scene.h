/********************************************************************/
/** scene.h by Alex Koukoulas (C) 2017 All Rights Reserved         **/
/** File Description:                                              **/
/********************************************************************/

#pragma once

// Local Headers
#include "rendering/d3dcommon.h"
#include "util/math.h"

// Remote Headers
#include <memory>
#include <vector>

class Model;
class Renderer;
class Camera;

class Scene final
{
public:
	Scene(Renderer& renderer, Camera& ref);
	~Scene();

	void InsertObect(std::shared_ptr<Model> model);
	void Update(const FLOAT deltaTime);
	void Render();

private:
	void ConstructScene();

private:
	static const UINT CELL_ROWS = 8U;
	static const UINT CELL_COLS = 8U;
	static const FLOAT CELL_SIZE;

private:
	struct Cell
	{
		FLOAT _x;
		FLOAT _z;

		std::vector<std::shared_ptr<Model>> _residents;

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
	Renderer& _renderer;
	Camera& _camera;

	Cell _sceneGraph[CELL_ROWS][CELL_COLS];
	
	std::unique_ptr<Model> _sceneCellModel;

	comptr<ID3D11ShaderResourceView> _defaultCellTexture;
	comptr<ID3D11ShaderResourceView> _activatedCellTexture;
};