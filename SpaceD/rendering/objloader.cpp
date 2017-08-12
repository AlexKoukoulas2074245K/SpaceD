/*********************************************************************/
/** objloader.cpp by Alex Koukoulas (C) 2017 All Rights Reserved    **/
/** File Description:                                               **/
/*********************************************************************/

// Local Headers
#include "objloader.h"
#include "model.h"
#include "vertex.h"
#include "../util/stringutils.h"

// Remote Headers
#include <fstream>
#include <vector>
#include <Windows.h>

// Constants
const std::string OBJLoader::OBJ_EXT = ".obj";

// Internal Structs
struct OBJIndex 
{
	UINT _posIndex, _texIndex, _normalIndex; 

	OBJIndex(const UINT posIndex, const UINT texIndex, const UINT normalIndex)
		: _posIndex(posIndex)
		, _texIndex(texIndex)
		, _normalIndex(normalIndex)
	{
	}
};

OBJLoader::OBJLoader()
{
	_objModels.clear();
}

OBJLoader::~OBJLoader()
{
}

std::shared_ptr<Model> OBJLoader::LoadOBJModelByName(const std::string& modelName)
{
	// Model entry exists
	if (_objModels.count(modelName))
	{
		// Model's weak pointer has not expired
		if (auto sharedModel = _objModels[modelName].lock())
		{
			// Model found; return cached version
			return sharedModel;
		}

		// Model's weak pointer has expired, so entry needs to be erased
		_objModels.erase(modelName);
	}

	// Load Model normally and add entry
	auto fullModelPath = Model::MODEL_DIRECTORY_PATH + modelName + "/" + modelName + OBJ_EXT;
	auto loadedModel = LoadOBJModel(modelName, fullModelPath);
	_objModels[modelName] = loadedModel;

	return loadedModel;
}

std::shared_ptr<Model> OBJLoader::LoadOBJModel(const std::string& modelName, const std::string& fullModelPath)
{
	std::ifstream fileStream(fullModelPath);

	if (!fileStream.is_open())
	{
		MessageBox(0, (std::string("Model: ") + fullModelPath + " was not found").c_str(), 0, MB_ICONWARNING);
		return nullptr;
	}

	// Define OBJ parsing data structures 
	std::vector<XMFLOAT3> posRawData;
	std::vector<XMFLOAT2> texRawData;
	std::vector<XMFLOAT3> normalRawData;			
	std::vector<OBJIndex> indexData;

	// Begin parsing obj file
	std::string line;
	while (std::getline(fileStream, line))
	{
		if (line.size() < 2)
		{
			continue;
		}

		auto lineSplitBySpace = string_utils::split(line, ' ');

		// Vertex attribute entry
		if (line[0] == 'v')
		{
			// Texcoord entry
			if (line[1] == 't')
			{
				texRawData.emplace_back(std::stof(lineSplitBySpace[1]), 1 - std::stof(lineSplitBySpace[2]));
			}
			// Normal entry
			else if (line[1] == 'n')
			{
				normalRawData.emplace_back(std::stof(lineSplitBySpace[1]), std::stof(lineSplitBySpace[2]), std::stof(lineSplitBySpace[3]));
			}
			// Position entry
			else
			{
				posRawData.emplace_back(std::stof(lineSplitBySpace[1]), std::stof(lineSplitBySpace[2]), std::stof(lineSplitBySpace[3]));
			}
		}
		else if (line[0] == 'f')
		{
			// For each one of the three face points extract the position, texcoord, and normal indices
			for (auto i = 1; i < 4; ++i)
			{
				auto faceEntry = lineSplitBySpace[i];
				auto faceEntrySplitByDash = string_utils::split(faceEntry, '/');

				// Add the OBJ index with -1 for its data since they are automatically output with 1-starting index				
				indexData.emplace_back(std::stoi(faceEntrySplitByDash[0]) - 1, std::stoi(faceEntrySplitByDash[1]) - 1, std::stoi(faceEntrySplitByDash[2]) - 1);
			}
		}
	}
	fileStream.close();

	// Fill in final vertex and index data structures
	std::vector<Vertex> finalVertexData;
	std::vector<UINT>   finalIndexData;

	for (size_t i = 0; i < indexData.size(); ++i)
	{
		// Extract current OBJ index
		auto currentOBJIndex = indexData[i];

		// Extract attribute data indices from current OBJ Index
		auto currentPosData    = posRawData[currentOBJIndex._posIndex];
		auto currentTexData    = texRawData[currentOBJIndex._texIndex];
		auto currentNormalData = normalRawData[currentOBJIndex._normalIndex];

		// Add them to the final data buffers
		finalVertexData.emplace_back(currentPosData, currentTexData, currentNormalData);
		finalIndexData.push_back(i);
	}
	
	// Can't use make shared with private constructors (even if OBJLoader is Model's friend)
	return std::shared_ptr<Model>(new Model(modelName, finalVertexData, finalIndexData));
}