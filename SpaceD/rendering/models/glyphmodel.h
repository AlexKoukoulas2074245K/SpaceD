/********************************************************************/
/** glyphmodel.h by Alex Koukoulas (C) 2017 All Rights Reserved    **/
/** File Description:                                              **/
/********************************************************************/

#pragma once

// Local Headers
#include "model.h"

// Remote Headers

class GlyphModel final: public Model
{
public:
	GlyphModel(const std::string& glyphName, const std::vector<XMFLOAT2>& glyphTexcoords);
	~GlyphModel();

protected:
	void LoadModelComponents(comptr<ID3D11Device> device) override;
	void LoadModelData() override;

private:
	const std::string _glyphName;	
	const std::vector<XMFLOAT2> _glyphTexcoords;
};