/**********************************************************************/
/** uimodel.cpp by Alex Koukoulas (C) 2017 All Rights Reserved  **/
/** File Description:                                                **/
/**********************************************************************/

#pragma once

// Local Headers
#include "uimodel.h"

// Remote Headers

UiModel::~UiModel()
{
}

UiModel::UiModel(const std::vector<Vertex>& rawVertexData, const std::vector<UINT>& rawIndexData)
	: Model("ui_square", rawVertexData, rawIndexData)
{

}

void UiModel::setCorrectGlyphVertices(const std::vector<Vertex>& glyphVertexData)
{
	_rawVertexData = glyphVertexData;
}