/********************************************************************/
/** main.cpp by Alex Koukoulas (C) 2017 All Rights Reserved        **/
/** File Description:                                              **/
/********************************************************************/

// Local Headers
#include "game.h"

// Remote Headers
#include <vld.h>
#include <Windows.h>
#include <Windowsx.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	//TODO change to config values
	auto clientWidth   = 800;
	auto clientHeight  = 900;
	auto clientName = "Space-D";
	
	Game game(hInstance, clientName, clientWidth, clientHeight);
	game.Run();

	return 0;
}

