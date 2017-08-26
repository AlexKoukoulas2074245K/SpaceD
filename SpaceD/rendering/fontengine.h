/********************************************************************/
/** fontengine.h by Alex Koukoulas (C) 2017 All Rights Reserved    **/
/** File Description:                                              **/
/********************************************************************/

#pragma once

// Local Headers
#include "d3dcommon.h"

// Remote Headers
#include <memory>
#include <string>	
#include <unordered_map>
#include <vector>

class Model;

class FontEngine final
{
public:
	FontEngine(const std::string& name, comptr<ID3D11Device> device);
	~FontEngine();

	void LoadFont(const std::string& name, comptr<ID3D11Device> device);
	
	Model& GetGlyph(const std::string& character) const;
	
	FLOAT GetSize() const;
	void SetSize(const FLOAT size);

private:
	void LoadFontTexture(comptr<ID3D11Device> device);
	void LoadConfig();
	void LoadGlyphs(comptr<ID3D11Device> device);

private:
	static const std::string FONT_DIRECTORY;
	static const std::string FONT_CFG_EXT;
	static const std::string FONT_TEXTURE_EXT;

private:
	FLOAT _size;
	std::string _name;
	std::unordered_map<std::string, std::unique_ptr<Model>> _glyphs;
	std::vector<std::vector<std::string>> _fontConfig;

	comptr<ID3D11ShaderResourceView> _texture;
};