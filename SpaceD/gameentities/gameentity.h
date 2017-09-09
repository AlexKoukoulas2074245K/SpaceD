/********************************************************************/
/** gameentity.h by Alex Koukoulas (C) 2017 All Rights Reserved    **/
/** File Description:                                              **/
/********************************************************************/

#pragma once

// Local Headers
#include "../util/math.h"

// Remote Headers
#include <string>
#include <memory>

// Forward Declarations
class Model;
class Renderer;
class Camera;
class InputHandler;

class GameEntity
{
public:
	GameEntity(const std::string& modelName, const Camera& camera, const InputHandler& inputHandler, Renderer& renderer);
	virtual ~GameEntity();

	virtual void Update(const FLOAT deltaTime);

	const Model& GetModel() const;
	const math::Transform& GetTransform() const;
	const math::Dimensions& GetDimensions() const;
	const XMFLOAT3& GetTranslation() const;
	const XMFLOAT3& GetScale() const;
	const XMFLOAT3& GetRotation() const;

private:
	void LoadModel(const std::string& modelName);

protected:
	std::unique_ptr<Model> _model;	
	const Camera& _camera;
	const InputHandler& _inputHandler;
	Renderer& _renderer;
};