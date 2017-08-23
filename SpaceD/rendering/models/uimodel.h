/********************************************************************/
/** uimodel.h by Alex Koukoulas (C) 2017 All Rights Reserved       **/
/** File Description:                                              **/
/********************************************************************/

#pragma once

// Local Headers
#include "model.h"

// Remote Headers

class UiModel: public Model
{
public:
	~UiModel();

	// To be called after the model has been loaded and before Model::PrepareD3DComponents
	void setCorrectGlyphVertices(const std::vector<Vertex>& glyphVertexData);
	
private:
	UiModel(const std::vector<Vertex>& rawVertexData, const std::vector<UINT>& rawIndexData);

};