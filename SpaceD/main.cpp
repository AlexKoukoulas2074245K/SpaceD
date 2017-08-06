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
	int clientWidth   = 400;
	int clientHeight  = 600;
	LPCSTR clientName = "Space-D";
	
	Game game(hInstance, clientName, clientWidth, clientHeight);
	game.Run();

	return 0;
}

