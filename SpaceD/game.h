/********************************************************************/
/** game.h by Alex Koukoulas (C) 2017 All Rights Reserved          **/
/** File Description:                                              **/
/********************************************************************/

#pragma once

// Local Headers

// Remote Headers
#include <memory>
#include <Windows.h>
#include <Windowsx.h>

// Forward Declarations
class InputHandler;
class ClientWindow;
class GameTimer;
class Renderer;
class Model;
class Camera;

class Game final
{
public:
	Game(HINSTANCE hInstance, const LPCSTR clientName, const int clientWidth, const int clientHeight);
	~Game();

	void Run();
	
	LRESULT MsgProc(HWND handle, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	void OnResize();
	void Update(const float deltaTime);
	void Render();
	void CalculateFrameStats();

private:
	std::unique_ptr<InputHandler> _inputHandler;
    std::unique_ptr<ClientWindow> _clientWindow;
	std::unique_ptr<GameTimer> _gameTimer;
	std::unique_ptr<Renderer> _renderer;	
	std::unique_ptr<Camera> _camera;
	std::shared_ptr<Model> _shipModel;
	
	bool _debugPrompt;
	bool _paused;
	bool _minimized;
	bool _maximized;
	bool _resizing;
};
