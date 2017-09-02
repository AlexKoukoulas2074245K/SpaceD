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
class Renderer;
class Model;
class UIModel;
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
	std::shared_ptr<Model> _shipModel;
	std::shared_ptr<UIModel> _uiModel;
	
	// Stack allocated to avoid matrix misalignments
	Camera _camera;

	bool _debugPrompt;
	bool _paused;
	bool _minimized;
	bool _maximized;
	bool _resizing;
};
