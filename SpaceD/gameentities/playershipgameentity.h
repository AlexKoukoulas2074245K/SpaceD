/****************************************************************************/
/** playershipgameentity.h by Alex Koukoulas (C) 2017 All Rights Reserved  **/
/** File Description:                                                      **/
/****************************************************************************/

#pragma once

// Local Headers
#include "gameentity.h"

// Remote Headers
class Camera;
class InputHandler;

class PlayerShipGameEntity final: public GameEntity 
{
public:
	PlayerShipGameEntity(Scene& scene, Renderer& renderer, const Camera& camera, const InputHandler& inputHandler);
	~PlayerShipGameEntity();

	void Update(const FLOAT deltaTime, const std::vector<std::shared_ptr<GameEntity>>& nearbyEntities);

private:
	enum AnimationState
	{
		IDLE, ROT_LEFT, ROT_RIGHT
	};

private:
	AnimationState _animState;
	XMFLOAT3 _velocity;
    FLOAT _animTargetRotAngle;
	UINT _projectileSpawnTimer;
	const Camera& _camera;
	const InputHandler& _inputHandler;
};