/********************************************************************/
/** objloader.h by Alex Koukoulas (C) 2017 All Rights Reserved     **/
/** File Description:                                              **/
/********************************************************************/

#pragma once

// Local Headers
#include "d3dcommon.h"
#include "vertex.h"

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

		ModelData(const std::vector<Vertex>& rawVertexData, const std::vector<UINT>& rawIndexData)			
			: vertexData(rawVertexData)
			, indexData(rawIndexData)
		{
		}
	};

public:
	static OBJLoader& Get();
	~OBJLoader();

	std::shared_ptr<ModelData> LoadOBJData(const std::string& modelDataPath);

private:
	OBJLoader();
	OBJLoader(const OBJLoader& rhs) = delete;
	OBJLoader& operator = (const OBJLoader& rhs) = delete;

private:	
	std::unordered_map<std::string, std::weak_ptr<ModelData>> _objModelData;
};