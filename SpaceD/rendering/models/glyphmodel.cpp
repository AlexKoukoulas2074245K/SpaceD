/**********************************************************************/
/** glyphmodel.cpp by Alex Koukoulas (C) 2017 All Rights Reserved    **/
/** File Description:                                                **/
/**********************************************************************/

#pragma once

// Local Headers
#include "glyphmodel.h"
#include "../objloader.h"

// Remote Headers

GlyphModel::GlyphModel(const std::string& glyphName, const std::vector<XMFLOAT2>& glyphTexcoords)
	: _glyphName(glyphName)	
	, _glyphTexcoords(glyphTexcoords)
	, Model("ui_square")
{
	
}

GlyphModel::~GlyphModel()
{
}

void GlyphModel::LoadModelComponents(comptr<ID3D11Device> device)
{
	LoadModelData();
	Model::LoadBuffers(device);
}

void GlyphModel::LoadModelData()
{
	const auto modelData = OBJLoader::Get().LoadOBJData(MODEL_DIRECTORY_PATH + _name + "/" + _name + MODEL_OBJDATA_EXT, _glyphTexcoords);
	_rawVertexData = modelData->vertexData;
	_rawIndexData = modelData->indexData;
	_dimensions = modelData->dimensions;
}
