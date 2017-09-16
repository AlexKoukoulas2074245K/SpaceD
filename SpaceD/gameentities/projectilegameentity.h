/*****************************************************************************/
/** projectilegameentity.h by Alex Koukoulas (C) 2017 All Rights Reserved   **/
/** File Description:                                                       **/
/*****************************************************************************/

#pragma once

// Local Headers
#include "gameentity.h"

// Remote Headers

class ProjectileGameEntity: public GameEntity
{
public:
	ProjectileGameEntity(const std::string& projectileName, const INT damage, const XMFLOAT3& pos, Scene& scene, Renderer& renderer);
	virtual ~ProjectileGameEntity();

	virtual void Update(const FLOAT deltaTime);

	virtual std::string GetBriefDescription() const;
	virtual std::vector<std::string> GetDetailedDescription() const;

private:
	XMFLOAT3 _velocity;
	INT _damage;
};