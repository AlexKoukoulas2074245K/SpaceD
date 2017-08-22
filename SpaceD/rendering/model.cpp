/*********************************************************************/
/** model.cpp by Alex Koukoulas (C) 2017 All Rights Reserved        **/
/** File Description:                                               **/
/*********************************************************************/

// Local Headers
#include "model.h"

// Remote Headers
#include <d3dx11.h>
#include <Windows.h>

// Linking to external libs
#pragma comment (lib, "d3dx11.lib")

// Constant members
const std::string Model::MODEL_DIRECTORY_PATH = "../res/models/";
const std::string Model::MODEL_TEXTURE_EXT    = ".png";

Model::Model(const std::string& modelName, std::vector<Vertex>& rawVertexData, const std::vector<UINT>& rawIndexData)
	: _name(modelName)
	, _texture(0)
	, _vertexBuffer(0)
	, _indexBuffer(0)
	, _rawVertexData(rawVertexData)
	, _rawIndexData(rawIndexData)
{
	
}

Model::~Model()
{
	OutputDebugString((std::string("Deleting model: ") + _name + "\n").c_str()); 
}

void Model::prepareD3DComponents(comptr<ID3D11Device> device)
{
	LoadBuffers(device);
	LoadTexture(device);
}

const math::Transform& Model::GetTransform() const
{
	return _transform;
}

math::Transform& Model::GetTransform()
{
	return _transform;
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

void Model::LoadBuffers(comptr<ID3D11Device> device)
{
	_vertexBuffer.Reset();
	_indexBuffer.Reset();

	// Describe and create vertex buffer
	D3D11_BUFFER_DESC vbd;
	vbd.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	vbd.ByteWidth           = sizeof(Vertex) * _rawVertexData.size();
	vbd.CPUAccessFlags      = 0;
	vbd.MiscFlags           = 0;
	vbd.StructureByteStride = 0;
	vbd.Usage               = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA vsrd;
	vsrd.pSysMem          = &_rawVertexData[0];
	vsrd.SysMemPitch      = 0;
	vsrd.SysMemSlicePitch = 0;

	device->CreateBuffer(&vbd, &vsrd, _vertexBuffer.GetAddressOf());

	// Describe and create index buffer;
	D3D11_BUFFER_DESC ibd;
	ibd.BindFlags           = D3D11_BIND_INDEX_BUFFER;
	ibd.ByteWidth           = sizeof(UINT) * _rawIndexData.size();
	ibd.CPUAccessFlags      = 0;
	ibd.MiscFlags           = 0;
	ibd.StructureByteStride = 0;
	ibd.Usage               = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA isrd;
	isrd.pSysMem          = &_rawIndexData[0];
	isrd.SysMemPitch      = 0;
	isrd.SysMemSlicePitch = 0;

	device->CreateBuffer(&ibd, &isrd, _indexBuffer.GetAddressOf());
}

void Model::LoadTexture(comptr<ID3D11Device> device)
{
	HR(D3DX11CreateShaderResourceViewFromFile(device.Get(), (MODEL_DIRECTORY_PATH + _name + "/" + _name + MODEL_TEXTURE_EXT).c_str(), 0, 0, &_texture, 0));
}