/********************************************************************/
/** model.h by Alex Koukoulas (C) 2017 All Rights Reserved         **/
/** File Description:                                              **/
/********************************************************************/

#pragma once

// Local Headers
#include "d3dcommon.h"
#include "vertex.h"
#include "../util/math.h"

// Remote Headers
#include <string>
#include <vector>

class Model
{
public:
	Model(const std::string& modelName, std::vector<Vertex>& rawVertexData, const std::vector<UINT>& rawIndexData);
	virtual ~Model();

	void prepareD3DComponents(comptr<ID3D11Device> device);

	UINT GetIndexCount() const;
	
	comptr<ID3D11Buffer> GetVertexBuffer() const;
	comptr<ID3D11Buffer> GetIndexBuffer() const;
	comptr<ID3D11ShaderResourceView> GetTexture() const;

private:
	void LoadBuffers(comptr<ID3D11Device> device);
	void LoadTexture(comptr<ID3D11Device> device);

public:
	static const std::string MODEL_DIRECTORY_PATH;
	static const std::string MODEL_TEXTURE_EXT;

protected:
	const std::string _name;

	comptr<ID3D11ShaderResourceView> _texture;
	comptr<ID3D11Buffer> _vertexBuffer;
	comptr<ID3D11Buffer> _indexBuffer;

	std::vector<Vertex> _rawVertexData;
	std::vector<UINT>   _rawIndexData;

	math::Transform _transform;
};