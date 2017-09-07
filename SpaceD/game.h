/********************************************************************/
/** game.h by Alex Koukoulas (C) 2017 All Rights Reserved          **/
/** File Description:                                              **/
/********************************************************************/

#pragma once

// Local Headers
#include "camera.h"

// Remote Headers
#include <memory>
#include <Windows.h>
#include <Windowsx.h>

// Forward Declarations
class InputHandler;
class ClientWindow;
class GameTimer;
class GameEntity;
class Renderer;
class Camera;
class Scene;

class Game final
{
public:
	Game(HINSTANCE hInstance, const LPCSTR clientName, const int clientWidth, const int clientHeight);
	~Game();

	void Run();
	
	LRESULT MsgProc(HWND handle, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	void OnResize();
	void Update(const FLOAT deltaTime);
	void Render();
	void CalculateFrameStats();

private:
	std::unique_ptr<InputHandler> _inputHandler;
    std::unique_ptr<ClientWindow> _clientWindow;
	std::unique_ptr<GameTimer> _gameTimer;
	std::unique_ptr<Renderer> _renderer;		
	std::unique_ptr<Scene> _scene;

	std::shared_ptr<GameEntity> _ship;

	// Stack allocated to avoid matrix misalignments
	Camera _camera;

	bool _debugPrompt;
	bool _paused;
	bool _minimized;
	bool _maximized;
	bool _resizing;
};
