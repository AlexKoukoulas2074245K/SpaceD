/*****************************************************************************/
/** trainingbotgameentity.h by Alex Koukoulas (C) 2017 All Rights Reserved  **/
/** File Description:                                                       **/
/*****************************************************************************/

#pragma once

// Local Headers
#include "gameentity.h"

// Remote Headers

class TrainingBotGameEntity: public GameEntity
{
public:
	TrainingBotGameEntity(Scene& scene, Renderer& renderer, const XMFLOAT3& pos);
	~TrainingBotGameEntity();

	void Update(const FLOAT deltaTime, const std::vector<std::shared_ptr<GameEntity>>& nearbyEntities);

private:
	enum AnimationState
	{
		IDLE, ROT_LEFT, ROT_RIGHT
	};

private:
	UINT _attackTimer;	
	AnimationState _animState;
};