/********************************************************************/
/** main.cpp by Alex Koukoulas (C) 2017 All Rights Reserved        **/
/** File Description:                                              **/
/********************************************************************/

// Remote Headers
#include <vld.h>
#include <Windows.h>
#include <Windowsx.h>

// Local Headers
#include "game.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	//TODO change to config values
	auto clientWidth   = 400;
	auto clientHeight  = 600;
	auto clientName = "Space-D";
	
	Game game(hInstance, clientName, clientWidth, clientHeight);
	game.Run();

	return 0;
}

