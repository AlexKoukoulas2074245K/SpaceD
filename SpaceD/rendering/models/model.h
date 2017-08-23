/********************************************************************/
/** model.h by Alex Koukoulas (C) 2017 All Rights Reserved         **/
/** File Description:                                              **/
/********************************************************************/

#pragma once

// Local Headers
#include "../d3dcommon.h"
#include "../vertex.h"
#include "../../util/math.h"

// Remote Headers
#include <string>
#include <vector>

class Model
{
public:
	Model(const std::string& modelName, comptr<ID3D11Device> device);
	virtual ~Model();

	const XMMATRIX CalculateWorldMatrix() const;

	const math::Transform& GetTransform() const;
	math::Transform& GetTransform();

	UINT GetIndexCount() const;
	
	comptr<ID3D11Buffer> GetVertexBuffer() const;
	comptr<ID3D11Buffer> GetIndexBuffer() const;
	comptr<ID3D11ShaderResourceView> GetTexture() const;

protected:
	virtual void LoadModelComponents(comptr<ID3D11Device> device);
	virtual void LoadModelData(comptr<ID3D11Device> device);
	virtual void LoadTexture(comptr<ID3D11Device> device);
	virtual void LoadBuffers(comptr<ID3D11Device> device);

public:
	static const std::string MODEL_DIRECTORY_PATH;
	static const std::string MODEL_TEXTURE_EXT;
	static const std::string MODEL_OBJDATA_EXT;
	
protected:
	const std::string _name;

	comptr<ID3D11ShaderResourceView> _texture;
	comptr<ID3D11Buffer> _vertexBuffer;
	comptr<ID3D11Buffer> _indexBuffer;

	std::vector<Vertex> _rawVertexData;
	std::vector<UINT>   _rawIndexData;

	math::Transform _transform;
};