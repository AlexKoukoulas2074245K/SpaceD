/********************************************************************/
/** objloader.h by Alex Koukoulas (C) 2017 All Rights Reserved     **/
/** File Description:                                              **/
/********************************************************************/

#pragma once

// Local Headers
#include "d3dcommon.h"
#include "vertex.h"
#include "lightdef.h"

// Remote Headers
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class OBJLoader final
{
public:
	struct ModelData
	{		
		std::vector<Vertex> vertexData;
		std::vector<UINT> indexData;
		math::Dimensions dimensions;
		Material material;

		ModelData(const std::vector<Vertex>& rawVertexData, const std::vector<UINT>& rawIndexData, const math::Dimensions& dims, const Material& mat)
			: vertexData(rawVertexData)
			, indexData(rawIndexData)
			, dimensions(dims)
			, material(mat)
		{
		}
	};

public:
	static OBJLoader& Get();
	~OBJLoader();

	std::shared_ptr<ModelData> LoadOBJData(const std::string& modelDataPath);
	std::shared_ptr<ModelData> LoadOBJData(const std::string& modelDataPath, const std::vector<XMFLOAT2> customTexcoords);

private:
	OBJLoader();
	OBJLoader(const OBJLoader& rhs) = delete;
	OBJLoader& operator = (const OBJLoader& rhs) = delete;

private:	
	std::unordered_map<std::string, std::shared_ptr<ModelData>> _objModelData;
};