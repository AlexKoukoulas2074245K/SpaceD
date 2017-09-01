/*********************************************************************/
/** objloader.cpp by Alex Koukoulas (C) 2017 All Rights Reserved    **/
/** File Description:                                               **/
/*********************************************************************/

// Local Headers
#include "objloader.h"
#include "../util/stringutils.h"

// Remote Headers
#include <fstream>
#include <Windows.h>

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

OBJLoader& OBJLoader::Get()
{
	static OBJLoader instance;
	return instance;
}

OBJLoader::~OBJLoader()
{
}

OBJLoader::OBJLoader()
{
}

std::shared_ptr<OBJLoader::ModelData> OBJLoader::LoadOBJData(const std::string& modelDataPath)
{
	return LoadOBJData(modelDataPath, std::vector<XMFLOAT2>());
}

std::shared_ptr<OBJLoader::ModelData> OBJLoader::LoadOBJData(const std::string& modelDataPath, const std::vector<XMFLOAT2> customTexcoords)
{
	// Model entry exists
	if (_objModelData.count(modelDataPath))
	{
		// Model's weak pointer has not expired
		if (auto sharedModelData = _objModelData[modelDataPath].lock())
		{
			// Model found; return cached version
			return sharedModelData;
		}

		// Model's weak pointer has expired, so entry needs to be erased
		_objModelData.erase(modelDataPath);
	}

	// Load OBJ data normally
	std::ifstream fileStream(modelDataPath);

	if (!fileStream.is_open())
	{
		MessageBox(0, (std::string("Model: ") + modelDataPath + " was not found").c_str(), 0, MB_ICONWARNING);
		return nullptr;
	}

	// Define OBJ parsing data structures 
	std::vector<XMFLOAT3> posRawData;
	std::vector<XMFLOAT2> texRawData;
	std::vector<XMFLOAT3> normalRawData;
	std::vector<OBJIndex> indexData;

	// Check whether custom tex coords were passed in
	const auto customTexcoordsGiven = customTexcoords.size() > 0;
	if (customTexcoordsGiven)
	{
		texRawData = customTexcoords;
	}

	// Begin parsing obj file
	FLOAT minX = 0.0f;
	FLOAT maxX = 0.0f;
	FLOAT minY = 0.0f;
	FLOAT maxY = 0.0f;
	FLOAT minZ = 0.0f;
	FLOAT maxZ = 0.0f;
	
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
				if (!customTexcoordsGiven)
				{
				    texRawData.emplace_back(std::stof(lineSplitBySpace[1]), 1 - std::stof(lineSplitBySpace[2]));
				}
			}
			// Normal entry
			else if (line[1] == 'n')
			{
				normalRawData.emplace_back(std::stof(lineSplitBySpace[1]), std::stof(lineSplitBySpace[2]), std::stof(lineSplitBySpace[3]));
			}
			// Position entry
			else
			{
				const auto x = std::stof(lineSplitBySpace[1]);
				const auto y = std::stof(lineSplitBySpace[2]);
				const auto z = std::stof(lineSplitBySpace[3]);

				if (x < minX) minX = x;
				if (x > maxX) maxX = x;
				if (y < minY) minY = y;
				if (y > maxY) maxY = y;
				if (z < minZ) minZ = z;
				if (z > maxZ) maxZ = z;

				posRawData.emplace_back(x, y, z);
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
		auto currentPosData = posRawData[currentOBJIndex._posIndex];
		auto currentTexData = texRawData[currentOBJIndex._texIndex];
		auto currentNormalData = normalRawData[currentOBJIndex._normalIndex];

		// Add them to the final data buffers
		finalVertexData.emplace_back(currentPosData, currentTexData, currentNormalData);
		finalIndexData.push_back(i);
	}

	// Calculate model dimensions
	math::Dimensions dimensions(math::absf(maxX, minX), math::absf(maxY, minY), math::absf(maxZ, minZ));

	// Can't use make shared with private constructors (even if OBJLoader is Model's friend)
	auto loadedModelData = std::make_shared<OBJLoader::ModelData>(finalVertexData, finalIndexData, dimensions);
	_objModelData[modelDataPath] = loadedModelData;

	return loadedModelData;
}
