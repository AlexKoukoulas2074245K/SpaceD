/********************************************************************/
/** game.h by Alex Koukoulas (C) 2017 All Rights Reserved          **/
/** File Description:                                              **/
/********************************************************************/

#pragma once

// Remote Headers
#include <memory>
#include <Windows.h>
#include <Windowsx.h>

// Local Headers

// Forward Declarations
class ClientWindow;
class GameTimer;

class Game final
{
public:
	Game(HINSTANCE hInstance, const LPCSTR clientName, const int clientWidth, const int clientHeight);
	~Game();

	void Run();
	
	LRESULT MsgProc(HWND handle, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	void OnResize();
	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

	void Update();
	void Render();
	void CalculateFrameStats();

private:
    std::unique_ptr<ClientWindow> _clientWindow;
	std::unique_ptr<GameTimer> _gameTimer;

	bool _paused;
	bool _minimized;
	bool _maximized;
	bool _resizing;
};
