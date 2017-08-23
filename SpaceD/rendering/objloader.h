/********************************************************************/
/** objloader.h by Alex Koukoulas (C) 2017 All Rights Reserved     **/
/** File Description:                                              **/
/********************************************************************/

#pragma once

// Local Headers

// Remote Headers
#include <memory>
#include <string>
#include <unordered_map>

// Forward Declarations
class Model;

class OBJLoader final
{
public:
	static OBJLoader& Get();
	~OBJLoader();

	std::shared_ptr<Model> LoadOBJModelByName(const std::string& modelName);

private:
	OBJLoader();
	OBJLoader(const OBJLoader& rhs) = delete;
	OBJLoader& operator = (const OBJLoader& rhs) = delete;

	std::shared_ptr<Model> LoadOBJModel(const std::string& modelName, const std::string& fullModelPath);

private:
	static const std::string OBJ_EXT;

private:	
	std::unordered_map<std::string, std::weak_ptr<Model>> _objModels;
};