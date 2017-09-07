/**********************************************************************/
/** fontengine.cpp by Alex Koukoulas (C) 2017 All Rights Reserved    **/
/** File Description:                                                **/
/**********************************************************************/

#pragma once

// Local Headers
#include "fontengine.h"
#include "models/model.h"
#include "models/glyphmodel.h"
#include "textureloader.h"
#include "../util/stringutils.h"

// Remote Headers
#include <fstream>
#include <Windows.h>

// Constants
const std::string FontEngine::FONT_DIRECTORY   = "../res/fonts/";
const std::string FontEngine::FONT_CFG_EXT     = ".cfg";
const std::string FontEngine::FONT_TEXTURE_EXT = ".png";

FontEngine::FontEngine(const std::string& name, comptr<ID3D11Device> device)
	: _name(name)
	, _size(0.02f)
{
	LoadFont(name, device);
}

FontEngine::~FontEngine()
{

}

void FontEngine::LoadFont(const std::string& name, comptr<ID3D11Device> device)
{
	LoadFontTexture(device);
	LoadConfig();
	LoadGlyphs(device);
}

Model& FontEngine::GetGlyph(const std::string& character) const
{
	return *_glyphs.at(character);
}

FLOAT FontEngine::GetSize() const
{
	return _size;
}

void FontEngine::SetSize(const FLOAT size) 
{
	_size = size;
}

void FontEngine::LoadFontTexture(comptr<ID3D11Device> device)
{
	_texture = TextureLoader::Get().LoadTexture(FONT_DIRECTORY + _name + "/" + _name + FONT_TEXTURE_EXT, device);
}

void FontEngine::LoadConfig()
{
	_fontConfig.clear();

	const auto fontConfigPath = FONT_DIRECTORY + _name + "/" + _name + FONT_CFG_EXT;

	std::ifstream fileStream(fontConfigPath);

	if (!fileStream.is_open())
	{
		MessageBox(0, (std::string("Font: ") + fontConfigPath + " was not found").c_str(), 0, MB_ICONWARNING);
		return;
	}

	std::string line;
	while (std::getline(fileStream, line))
	{
		_fontConfig.push_back(string_utils::split(line, ' '));
	}
}

void FontEngine::LoadGlyphs(comptr<ID3D11Device> device)
{	
	_glyphs.clear();

	const auto TEX_ROWS       = _fontConfig.size();
	const auto TEX_COLS       = _fontConfig[0].size();
	const auto TEX_GLYPH_SIZE = 1.0f/TEX_ROWS;

	for (auto i = 0U; i < TEX_ROWS; ++i)
	{
		const auto innerVec = _fontConfig[i];
		for (auto j = 0U; j < TEX_COLS; ++j)
		{
			std::vector<XMFLOAT2> glyphTexCoords;
			glyphTexCoords.push_back(XMFLOAT2(j * TEX_GLYPH_SIZE + TEX_GLYPH_SIZE, i * TEX_GLYPH_SIZE + TEX_GLYPH_SIZE)); // bottom right
			glyphTexCoords.push_back(XMFLOAT2(j * TEX_GLYPH_SIZE,                  i * TEX_GLYPH_SIZE));                  // top left
			glyphTexCoords.push_back(XMFLOAT2(j * TEX_GLYPH_SIZE,                  i * TEX_GLYPH_SIZE + TEX_GLYPH_SIZE)); // bottom left
			glyphTexCoords.push_back(XMFLOAT2(j * TEX_GLYPH_SIZE + TEX_GLYPH_SIZE, i * TEX_GLYPH_SIZE));                  // top right

			const auto targetLetter = _fontConfig[i][j];
			_glyphs[targetLetter] = std::move(std::unique_ptr<Model>(new GlyphModel(targetLetter, glyphTexCoords)));
			_glyphs[targetLetter]->LoadModelComponents(device);
			_glyphs[targetLetter]->SetTexture(_texture);
			_glyphs[targetLetter]->GetTransform()._scale = XMFLOAT3(_size, _size, _size);
		}
	}
}

	
