/****************************************************************************/
/** playershipgameentity.h by Alex Koukoulas (C) 2017 All Rights Reserved  **/
/** File Description:                                                      **/
/****************************************************************************/

#pragma once

// Local Headers
#include "shipgameentity.h"

// Remote Headers
class Camera;
class InputHandler;

class PlayerShipGameEntity final: public ShipGameEntity 
{
public:
	PlayerShipGameEntity(Renderer& renderer, const Camera& camera, const InputHandler& inputHandler);
	~PlayerShipGameEntity();

	void Update(const FLOAT deltaTime) override;

private:
	enum AnimationState
	{
		IDLE, ROT_LEFT, ROT_RIGHT
	};

private:
	AnimationState _animState;
    FLOAT _animTargetRotAngle;

	const Camera& _camera;
	const InputHandler& _inputHandler;
};