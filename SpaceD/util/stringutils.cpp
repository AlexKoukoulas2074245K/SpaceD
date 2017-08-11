/*********************************************************************/
/** stringutils.cpp by Alex Koukoulas (C) 2017 All Rights Reserved  **/
/** File Description:                                               **/
/*********************************************************************/

// Local Headers
#include "stringutils.h"

// Remote Headers
#include <string>

void string_utils::split(const std::string& s, char delim, std::vector<std::string>& elems)
{
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim))
	{
		elems.push_back(item);
	}
}

std::vector<std::string> string_utils::split(const std::string& s, char delim)
{
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}