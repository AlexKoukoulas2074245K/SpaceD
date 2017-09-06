/*********************************************************************/
/** model.cpp by Alex Koukoulas (C) 2017 All Rights Reserved        **/
/** File Description:                                               **/
/*********************************************************************/

// Local Headers
#include "model.h"
#include "../textureloader.h"
#include "../objloader.h"

// Remote Headers
#include <d3dx11.h>
#include <Windows.h>

// Linking to external libs
#pragma comment (lib, "d3dx11.lib")

// Constant members
const std::string Model::MODEL_DIRECTORY_PATH = "../res/models/";
const std::string Model::MODEL_TEXTURE_EXT    = ".png";
const std::string Model::MODEL_OBJDATA_EXT    = ".obj";

static UINT modelCount = 0;

Model::Model(const std::string& modelName)
	: _name(modelName)
	, _texture(0)
	, _vertexBuffer(0)
	, _indexBuffer(0)
{	
}

Model::~Model()
{
	OutputDebugString((std::string("Deleting model: ") + _name + " model count: " + std::to_string(modelCount) + "\n").c_str()); 
	modelCount++;
}

const XMMATRIX Model::CalculateWorldMatrix() const
{
	const auto scaleMatrix = XMMatrixScaling(_transform.scale.x, _transform.scale.y, _transform.scale.z);
	const auto rotMatrix = XMMatrixRotationX(_transform.rotation.x) * XMMatrixRotationY(_transform.rotation.y) * XMMatrixRotationZ(_transform.rotation.z);
	const auto transMatrix = XMMatrixTranslation(_transform.translation.x, _transform.translation.y, _transform.translation.z);
	return scaleMatrix * rotMatrix * transMatrix;
}

const math::Transform& Model::GetTransform() const
{
	return _transform;
}

math::Transform& Model::GetTransform()
{
	return _transform;
}

const math::Dimensions& Model::GetDimensions() const
{
	return _dimensions;
}

const FLOAT Model::GetBiggestDimensionRad() const
{
	return math::max3f(_dimensions._width, _dimensions._height, _dimensions._depth) / 2.0f;
}

UINT Model::GetIndexCount() const 
{
	return static_cast<UINT>(_rawIndexData.size());
}

comptr<ID3D11Buffer> Model::GetVertexBuffer() const
{
	return _vertexBuffer;
}

comptr<ID3D11Buffer> Model::GetIndexBuffer() const
{
	return _indexBuffer;
}

comptr<ID3D11ShaderResourceView> Model::GetTexture() const
{
	return _texture;
}

void Model::SetTexture(comptr<ID3D11ShaderResourceView> texture)
{
	_texture = texture;
}

void Model::LoadModelComponents(comptr<ID3D11Device> device)
{
    LoadModelData();	
	LoadTexture(device);
	LoadBuffers(device);
}

void Model::LoadModelData()
{
	const auto modelData = OBJLoader::Get().LoadOBJData(MODEL_DIRECTORY_PATH + _name + "/" + _name + MODEL_OBJDATA_EXT);
	_rawVertexData = modelData->vertexData;
	_rawIndexData = modelData->indexData;
	_dimensions = modelData->dimensions;
}

void Model::LoadTexture(comptr<ID3D11Device> device)
{
	_texture = TextureLoader::Get().LoadTexture(MODEL_DIRECTORY_PATH + _name + "/" + _name + MODEL_TEXTURE_EXT, device);
}

void Model::LoadBuffers(comptr<ID3D11Device> device)
{
	_vertexBuffer.Reset();
	_indexBuffer.Reset();

	// Describe and create vertex buffer
	D3D11_BUFFER_DESC vbd;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.ByteWidth = sizeof(Vertex) * _rawVertexData.size();
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA vsrd;
	vsrd.pSysMem = &_rawVertexData[0];
	vsrd.SysMemPitch = 0;
	vsrd.SysMemSlicePitch = 0;

	device->CreateBuffer(&vbd, &vsrd, _vertexBuffer.GetAddressOf());

	// Describe and create index buffer;
	D3D11_BUFFER_DESC ibd;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.ByteWidth = sizeof(UINT) * _rawIndexData.size();
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA isrd;
	isrd.pSysMem = &_rawIndexData[0];
	isrd.SysMemPitch = 0;
	isrd.SysMemSlicePitch = 0;

	device->CreateBuffer(&ibd, &isrd, _indexBuffer.GetAddressOf());
}