/********************************************************************/
/** gameentity.h by Alex Koukoulas (C) 2017 All Rights Reserved    **/
/** File Description:                                              **/
/********************************************************************/

#pragma once

// Local Headers
#include "../util/math.h"

// Remote Headers
#include <string>
#include <vector>
#include <memory>

// Forward Declarations
class Model;
class DebugPrompt;
class Renderer;

class GameEntity
{
	friend class DebugPrompt;
public:
	GameEntity(const std::string& modelName, Renderer& renderer);
	virtual ~GameEntity();

	virtual void Update(const FLOAT deltaTime);

	virtual std::string GetBriefDescription() const;
	virtual std::vector<std::string> GetDetailedDescription() const;

	const Model& GetModel() const;
	const math::Transform& GetTransform() const;
	const math::Dimensions& GetDimensions() const;
	const XMFLOAT3& GetTranslation() const;
	const XMFLOAT3& GetScale() const;
	const XMFLOAT3& GetRotation() const;

private:
	void LoadModel(const std::string& modelName, Renderer& renderer);

protected:
	std::unique_ptr<Model> _model;		

};