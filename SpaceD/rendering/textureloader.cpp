/***********************************************************************/
/** textureloader.cpp by Alex Koukoulas (C) 2017 All Rights Reserved  **/
/** File Description:                                                 **/
/***********************************************************************/

#pragma once

// Local Headers
#include "textureloader.h"

// Remote Headers
#include <D3DX11.h>

TextureLoader& TextureLoader::Get()
{
	static TextureLoader instance;
	return instance;
}

TextureLoader::~TextureLoader()
{
}

TextureLoader::TextureLoader()
{
}

comptr<ID3D11ShaderResourceView> TextureLoader::LoadTexture(const std::string& texturePath, comptr<ID3D11Device> device)
{
	// Textuer entry exists
	if (_textures.count(texturePath))
	{
		// Texture found; return cached version
		return _textures[texturePath];		
	}

	// Load Model normally and add entry
	comptr<ID3D11ShaderResourceView> loadedTexture;
	HR(D3DX11CreateShaderResourceViewFromFile(device.Get(), texturePath.c_str(), 0, 0, loadedTexture.GetAddressOf(), 0));
	_textures[texturePath] = loadedTexture;

	return loadedTexture;
}