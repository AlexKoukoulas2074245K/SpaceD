/********************************************************************/
/** vertex.h by Alex Koukoulas (C) 2017 All Rights Reserved        **/
/** File Description:                                              **/
/********************************************************************/

#pragma once

// Local Headers
#include "../util/math.h"

// Remote Headers

struct Vertex
{
	XMFLOAT3 _pos;
	XMFLOAT2 _tex;
	XMFLOAT3 _normal;

	Vertex(const XMFLOAT3 pos, const XMFLOAT2 tex, const XMFLOAT3 normal)
		: _pos(pos)
		, _tex(tex)
		, _normal(normal)
	{
	}
};