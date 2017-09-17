/*********************************************************************/
/** game.cpp by Alex Koukoulas (C) 2017 All Rights Reserved         **/
/** File Description:                                               **/
/*********************************************************************/

// Local Headers
#include "game.h"
#include "inputhandler.h"
#include "camera.h"
#include "scene.h"
#include "rendering/objloader.h"
#include "rendering/renderer.h"
#include "rendering/models/model.h"
#include "gameentities/playershipgameentity.h"
#include "gameentities/trainingbotgameentity.h"
#include "util/clientwindow.h"
#include "util/gametimer.h"
#include "util/math.h"
#include "util/debug/debugprompt.h"

// Remote Headers
#include <sstream>
#include <cmath>
#include <Psapi.h>

namespace
{
	Game* game = 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return game->MsgProc(hwnd, msg, wParam, lParam);
}

Game::Game(HINSTANCE hInstance, const LPCSTR clientName, const int clientWidth, const int clientHeight)
	: _debugMode(false)
	, _paused(false)
	, _minimized(false)
	, _maximized(false)
	, _resizing(false)
{	
	// Delayed window and rendering context creation to allow assignment to global Game pointer (Window procs can not 
	// be members of a class)
	game = this;

	_gameTimer    = std::make_unique<GameTimer>();
	_clientWindow = std::make_unique<ClientWindow>(hInstance, WndProc, clientName, clientWidth, clientHeight);
	_renderer     = std::make_unique<Renderer>(*_clientWindow);
	_inputHandler = std::make_unique<InputHandler>(*_clientWindow);
	_scene        = std::make_unique<Scene>(*_renderer, _camera, *_inputHandler, *_clientWindow);
	_debugPrompt  = std::make_unique<DebugPrompt>(*_renderer, *_scene, *_inputHandler);

	_ship = std::make_shared<PlayerShipGameEntity>(*_scene, *_renderer, _camera, *_inputHandler);
	_scene->InsertEntity(_ship); 	
	
	_scene->InsertEntity(std::make_shared<TrainingBotGameEntity>(*_scene, *_renderer, XMFLOAT3(0.0f, 0.0f, -50.0f)));

	auto dirLight2 = std::make_shared<DirectionalLight>();
	dirLight2->_ambient =  XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	dirLight2->_diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	dirLight2->_specular = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	dirLight2->_direction = XMFLOAT3(0.0f, -0.8f, 0.0f);

	_scene->InsertDirectionalLight(dirLight2);

}

Game::~Game()
{
}

void Game::Run()
{		
	_gameTimer->Reset();

	// Message Loop
	MSG msg = {};

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		
		_gameTimer->Tick();

		if (!_paused)
		{
			CalculateFrameStats();

			if (!_debugMode)
			{
				Update(_gameTimer->DeltaTime());
			}
			else
			{
				_debugPrompt->Update();
			}
			Render();
			_inputHandler->OnFrameEnd();
		}
		else
		{
			Sleep(100);
		}
	}
}

LRESULT Game::MsgProc(HWND handle, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		// WM_ACTIVATE is sent when the window is activated or deactivated.  		
		case WM_ACTIVATE:
		{			
			if (LOWORD(wParam) == WA_INACTIVE)
			{
				_paused = true;
			    _gameTimer->Stop();
			}
			else
			{
				_paused = false;
				_gameTimer->Start();
			}
			return 0;
		} break;
		
		// WM_SIZE is sent when the user resizes the window.  
		case WM_SIZE:
		{			
			// Save the new client area dimensions.
			auto newWidth = LOWORD(lParam);
			auto newHeight = HIWORD(lParam);
            
			if (_clientWindow)
			{			
				_clientWindow->UpdateOnResize(newWidth, newHeight);
			}

			if (_renderer)
			{				
				if (wParam == SIZE_MINIMIZED)
				{
					_paused    = true;
					_minimized = true;
					_maximized = false;
				}
				else if (wParam == SIZE_MAXIMIZED)
				{
					_paused    = false;
					_minimized = false;
					_maximized = true;
					OnResize();
				}
				else if (wParam == SIZE_RESTORED)
				{

					// Restoring from minimized state?
					if (_minimized)
					{
						_paused    = false;
						_minimized = false;
						OnResize();
					}

					// Restoring from maximized state?
					else if (_maximized)
					{
						_paused    = false;
						_maximized = false;
						OnResize();
					}
					else if (_resizing)
					{
						// If user is dragging the resize bars, we do not resize 
						// the buffers here because as the user continuously 
						// drags the resize bars, a stream of WM_SIZE messages are
						// sent to the window, and it would be pointless (and slow)
						// to resize for each WM_SIZE message received from dragging
						// the resize bars.  So instead, we reset after the user is 
						// done resizing the window and releases the resize bars, which 
						// sends a WM_EXITSIZEMOVE message.
					}
					else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
					{
						OnResize();
					}
				}
			}			
		} break;
		

		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
		case WM_ENTERSIZEMOVE:
		{		
			_paused = true;
			_resizing = true;
			_gameTimer->Stop();			
		} break;

		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.		
		case WM_EXITSIZEMOVE:
		{			
			_paused = false;
			_resizing = false;
			_gameTimer->Start();
			OnResize();			
		} break;
		
		// WM_DESTROY is sent when the window is being destroyed.
		case WM_DESTROY:
		{
			PostQuitMessage(0);			
		} break;

		// The WM_MENUCHAR message is sent when a menu is active and the user presses 
		// a key that does not correspond to any mnemonic or accelerator key. 
	    case WM_MENUCHAR:
		{
			// Don't beep when we alt-enter.
			return MAKELRESULT(0, MNC_CLOSE); 
		} break;

		// Catch this message so to prevent the window from becoming too small.
		case WM_GETMINMAXINFO:
		{
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;			
		} break;

		case WM_LBUTTONDOWN:
		{			
			_inputHandler->OnMouseDown(InputHandler::Button::LMBUTTON, lParam);
		} break;

		case WM_RBUTTONDOWN:
		{
			_inputHandler->OnMouseDown(InputHandler::Button::RMBUTTON, lParam);
		} break;

		case WM_LBUTTONUP:
		{
			_inputHandler->OnMouseUp(InputHandler::Button::LMBUTTON, lParam);
		} break;

		case WM_RBUTTONUP:
		{
			_inputHandler->OnMouseUp(InputHandler::Button::RMBUTTON, lParam);
		} break;

		case WM_MOUSEMOVE:
		{
			_inputHandler->OnMouseMove(wParam, lParam);
		} break;

		case WM_MOUSEWHEEL:
		{
			_inputHandler->OnMouseWheelMove(wParam, lParam);
		} break;

#if defined(DEBUG) || defined(_DEBUG)
		case WM_MBUTTONUP:
		{
			_debugMode = !_debugMode;
		} break;
#endif
		case WM_KEYDOWN:
		{
			if (_debugPrompt && _debugMode)
			{
				_debugPrompt->UpdateSingleKeyChar(MapVirtualKey(wParam, MAPVK_VK_TO_CHAR), wParam);		
			}			
			_inputHandler->OnKeyDown(wParam, lParam);
		} break;

		case WM_KEYUP:
		{
			_inputHandler->OnKeyUp(wParam, lParam);
		} break;
	}

	return DefWindowProc(handle, msg, wParam, lParam);
}

void Game::OnResize()
{	
	_renderer->OnResize();
}

void Game::Update(const FLOAT deltaTime)
{
	_scene->Update(deltaTime);
}

void Game::Render()
{
	_renderer->ClearViews();
	_scene->Render();

	//_renderer->RenderText(dirLight2->Direction.y, XMFLOAT2(-1.0f, 0.95f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	//_renderer->RenderText(std::to_string(_shipModel->GetTransform().translation.x) + ", " + std::to_string(_shipModel->GetTransform().translation.z), XMFLOAT2(-1.0f, 0.9f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	
	//_renderer->RenderText("Camera pos: " + std::to_string(_camera._pos.x) + ", " + std::to_string(_camera._pos.y) + ", " + std::to_string(_camera._pos.z), XMFLOAT2(-1.0f, 0.95f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	//_renderer->RenderText("Camera pyr: " + std::to_string(_camera._pitch) + ", " + std::to_string(_camera._yaw) + ", " + std::to_string(_camera._roll), XMFLOAT2(-1.0f, 0.9f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	
	if (_debugMode)
	{
		_debugPrompt->Render();
	}

	_renderer->Present();
}

void Game::CalculateFrameStats()
{
	static auto frameCnt = 0;
	static auto timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if ((_gameTimer->TotalTime() - timeElapsed) >= 1.0f)
	{

		PROCESS_MEMORY_COUNTERS pmc = {};
		GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));

		auto mem = pmc.PeakWorkingSetSize / 1000000;
		auto fps = (float)frameCnt; // fps = frameCnt / 1
		auto mspf = 1000.0f / fps;

		std::ostringstream outs;
		outs.precision(6);
		outs << "    "
			<< "FPS: " << fps << "    "
			<< "Frame Time: " << mspf << " (ms)   "
		    << "Mem Usage: " << mem << " (MB)";
		_clientWindow->UpdateCaption(outs.str());		

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}