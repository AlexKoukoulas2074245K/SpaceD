/*********************************************************************/
/** textureloader.h by Alex Koukoulas (C) 2017 All Rights Reserved  **/
/** File Description:                                               **/
/*********************************************************************/

#pragma once

// Local Headers
#include "d3dcommon.h"

// Remote Headers
#include <string>
#include <memory>
#include <unordered_map>

class TextureLoader final
{
public:
	static TextureLoader& Get();

	~TextureLoader();

	comptr<ID3D11ShaderResourceView> LoadTexture(const std::string& texturePath, comptr<ID3D11Device> device);
		
private:
	TextureLoader();
	
	TextureLoader(const TextureLoader& rhs) = delete;
	TextureLoader& operator = (const TextureLoader& rhs) = delete;

private:
	std::unordered_map<std::string, comptr<ID3D11ShaderResourceView>> _textures;

};