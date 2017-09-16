/**********************************************************************/
/** shipgameentity.h by Alex Koukoulas (C) 2017 All Rights Reserved  **/
/** File Description:                                                **/
/**********************************************************************/

#pragma once

// Local Headers
#include "gameentity.h"

// Remote Headers

class ShipGameEntity: public GameEntity
{
public:
	ShipGameEntity(const std::string& shipName, Scene& scene, Renderer& renderer);
	virtual ~ShipGameEntity();

	virtual void Update(const FLOAT deltaTime);

	virtual std::string GetBriefDescription() const;
	virtual std::vector<std::string> GetDetailedDescription() const;

protected:
	XMFLOAT3 _velocity;

};