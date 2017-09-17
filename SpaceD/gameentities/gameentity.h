/********************************************************************/
/** gameentity.h by Alex Koukoulas (C) 2017 All Rights Reserved    **/
/** File Description:                                              **/
/********************************************************************/

#pragma once

// Local Headers
#include "../util/math.h"
#include "../rendering/lightdef.h"

// Remote Headers
#include <string>
#include <vector>
#include <memory>

// Forward Declarations
class Model;
class DebugPrompt;
class Renderer;
class Scene;

class GameEntity
{
	friend class DebugPrompt;
	
public:
	GameEntity(const std::string& modelName, const bool isProjectile, const bool isEnemy, Scene& scene, Renderer& renderer);
	virtual ~GameEntity();

	virtual void Update(const FLOAT deltaTime, const std::vector<std::shared_ptr<GameEntity>>& nearbyEntities);

	virtual std::string GetBriefDescription() const;
	virtual std::vector<std::string> GetDetailedDescription() const;

	const Model& GetModel() const;
	const math::Transform& GetTransform() const;
	const math::Dimensions& GetDimensions() const;
	const XMFLOAT3& GetTranslation() const;
	const XMFLOAT3& GetScale() const;
	const XMFLOAT3& GetRotation() const;
	const Material& GetMaterial() const;

	bool ShouldBeDestroyWhenOutOfBounds() const;
	bool IsProjectile() const;
	bool IsEnemy() const;

private:
	void LoadModel(const std::string& modelName);

protected:
	Scene& _scene;
	Renderer& _renderer;
	std::unique_ptr<Model> _model;	
	bool _shouldBeDestroyedWhenOutOfBounds;
	bool _isProjectile;
	bool _isEnemy;
};