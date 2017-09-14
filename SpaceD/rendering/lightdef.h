/********************************************************************/
/** lightdef.h by Alex Koukoulas (C) 2017 All Rights Reserved      **/
/** File Description:                                              **/
/********************************************************************/

#pragma once

// Local Headers
#include "../util/math.h"

// Remote Headers
#include <string>
#include <sstream>
#include <vector>

struct DirectionalLight
{
	DirectionalLight()
		: _ambient(0.4f, 0.4f, 0.4f, 1.0f)
		, _diffuse(0.6f, 0.6f, 0.6f, 1.0f)
		, _specular(0.8f, 0.8f, 0.8f, 1.0f)
		, _direction(0.0f, 0.0f, 0.0f)
	{
	}

	std::string GetBriefDescription() const
	{
		std::stringstream lightDesc;
		lightDesc << "DirectionalLight facing: " << _direction.x << ", " << _direction.y << ", " << _direction.z;
		return lightDesc.str();
	}

	std::vector<std::string> GetDetailedDescription() const
	{
		std::vector<std::string> descVector;

		std::stringstream dirDescStream;
		dirDescStream << "Direction: " << _direction.x << ", " << _direction.y << ", " << _direction.z;

		std::stringstream ambDescStream;
		ambDescStream << "Ambient:   " << _ambient.x << ", " << _ambient.y << ", " << _ambient.z << ", " << _ambient.w;

		std::stringstream diffDescStream;
		diffDescStream << "Diffuse:  " << _diffuse.x << ", " << _diffuse.y << ", " << _diffuse.z << ", " << _diffuse.w;

		std::stringstream specDescStream;
		specDescStream << "Specular: " << _specular.x << ", " << _specular.y << ", " << _specular.z << ", " << _specular.w;

		descVector.push_back(dirDescStream.str());
		descVector.push_back(ambDescStream.str());
		descVector.push_back(diffDescStream.str());
		descVector.push_back(specDescStream.str());		

		return descVector;
	}

	XMFLOAT4 _ambient;
	XMFLOAT4 _diffuse;
	XMFLOAT4 _specular;
	XMFLOAT3 _direction;
	float Pad; // Pad the last float so we can set an array of lights if we wanted.
};

struct PointLight
{
	PointLight()
		: _ambient(0.4f, 0.4f, 0.4f, 1.0f)
		, _diffuse(0.6f, 0.6f, 0.6f, 1.0f)
		, _specular(0.8f, 0.8f, 0.8f, 1.0f)
		, _att(0.0f, 0.0f, 0.1f)
		, _position(0.0f, 0.0f, 0.0f)
		, _range(10.0f)
	{
	}

	std::string GetBriefDescription() const 
	{
		std::stringstream lightDesc;
		lightDesc << "PointLight at: " << _position.x << ", " << _position.y << ", " << _position.z << " Range: " << _range;
		return lightDesc.str();
	}
	
	std::vector<std::string> GetDetailedDescription() const
	{
		std::vector<std::string> descVector;

		std::stringstream posDescStream;
		posDescStream   << "Translation: " << _position.x << ", " << _position.y << ", " << _position.z;

		std::stringstream _rangeDescStream;
		_rangeDescStream << "Range:       " << _range;

		std::stringstream _attDescStream;
		_attDescStream   << "Attenuation: " << _att.x << ", " << _att.y << ", " << _att.z;

		std::stringstream ambDescStream;
		ambDescStream   << "Ambient:     " << _ambient.x << ", " << _ambient.y << ", " << _ambient.z << ", " << _ambient.w;

		std::stringstream diffDescStream;
		diffDescStream  << "Diffuse:     " << _diffuse.x << ", " << _diffuse.y << ", " << _diffuse.z << ", " << _diffuse.w;

		std::stringstream specDescStream;
		specDescStream  << "Specular:    " << _specular.x << ", " << _specular.y << ", " << _specular.z << ", " << _specular.w;

		descVector.push_back(posDescStream.str());
		descVector.push_back(_rangeDescStream.str());
		descVector.push_back(_attDescStream.str());
		descVector.push_back(ambDescStream.str());
		descVector.push_back(diffDescStream.str());
		descVector.push_back(specDescStream.str());

		return descVector;
	}

	XMFLOAT4 _ambient;
	XMFLOAT4 _diffuse;
	XMFLOAT4 _specular;

	// Packed into 4D vector: (_position, _range)
	XMFLOAT3 _position;
	float _range;

	// Packed into 4D vector: (A0, A1, A2, Pad)
	XMFLOAT3 _att;
	float Pad; // Pad the last float so we can set an array of lights if we wanted.
};

struct SpotLight
{
	SpotLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 _ambient;
	XMFLOAT4 _diffuse;
	XMFLOAT4 _specular;

	// Packed into 4D vector: (_position, _range)
	XMFLOAT3 _position;
	float _range;

	// Packed into 4D vector: (_direction, Spot)
	XMFLOAT3 _direction;
	float _spot;

	// Packed into 4D vector: (_att, Pad)
	XMFLOAT3 _att;
	float Pad; // Pad the last float so we can set an array of lights if we wanted.
};

struct Material
{
	Material() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 _ambient;
	XMFLOAT4 _diffuse;
	XMFLOAT4 _specular; // w = SpecPower
	XMFLOAT4 _reflect;
};