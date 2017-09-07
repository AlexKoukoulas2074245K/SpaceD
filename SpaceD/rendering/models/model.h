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

class GameEntity;
class Scene;

class Model
{
public:
	friend class GameEntity;
	friend class Scene;

	Model(const std::string& modelName);
	virtual ~Model();

	virtual void LoadModelComponents(comptr<ID3D11Device> device);

	const XMMATRIX CalculateWorldMatrix() const;

	const math::Transform& GetTransform() const;
	math::Transform& GetTransform();

	const math::Dimensions& GetDimensions() const;
	const FLOAT GetBiggestDimensionRad() const;
	
	UINT GetIndexCount() const; 
	
	comptr<ID3D11Buffer> GetVertexBuffer() const;
	comptr<ID3D11Buffer> GetIndexBuffer() const;
	comptr<ID3D11ShaderResourceView> GetTexture() const;

	void SetTexture(comptr<ID3D11ShaderResourceView> texture);


protected:	
	virtual void LoadModelData();
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
	math::Dimensions _dimensions;
};