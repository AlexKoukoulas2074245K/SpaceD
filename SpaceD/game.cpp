/*********************************************************************/
/** game.cpp by Alex Koukoulas (C) 2017 All Rights Reserved         **/
/** File Description:                                               **/
/*********************************************************************/

// Local Headers
#include "game.h"
#include "inputhandler.h"
#include "camera.h"
#include "rendering/objloader.h"
#include "rendering/renderer.h"
#include "rendering/models/model.h"
#include "rendering/shaders/default3dwithlightingshader.h"
#include "util/clientwindow.h"
#include "util/gametimer.h"
#include "util/math.h"

// Remote Headers
#include <sstream>

namespace
{
	Game* game = 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return game->MsgProc(hwnd, msg, wParam, lParam);
}

Game::Game(HINSTANCE hInstance, const LPCSTR clientName, const int clientWidth, const int clientHeight)		
	: _gameTimer(std::make_unique<GameTimer>())
	, _debugPrompt(false)
	, _paused(false)
	, _minimized(false)
	, _maximized(false)
	, _resizing(false)
{	
	// Delayed window and rendering context creation to allow assignment to global Game pointer (Window procs can not 
	// be members of a class)
	game = this;

	_clientWindow = std::make_unique<ClientWindow>(hInstance, WndProc, clientName, clientWidth, clientHeight);
	_renderer     = std::make_unique<Renderer>(*_clientWindow);	
	_inputHandler = std::make_unique<InputHandler>();	
	_camera       = std::make_unique<Camera>();

	_shipModel = OBJLoader::Get().LoadOBJModelByName("ship_dps");
	_shipModel->PrepareD3DComponents(_renderer->GetDevice());
}

Game::~Game(){}

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

			if (!_debugPrompt)
			{
				Update(_gameTimer->DeltaTime());
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

		case WM_MBUTTONUP:
		{
			_debugPrompt = !_debugPrompt;
		} break;

		case WM_KEYDOWN:
		{
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

void Game::Update(const float deltaTime)
{
	if (_inputHandler->isKeyDown(InputHandler::LEFT)) _camera->RotateCamera(Camera::LEFT, deltaTime * 4);
	if (_inputHandler->isKeyDown(InputHandler::RIGHT)) _camera->RotateCamera(Camera::RIGHT, deltaTime * 4);
	if (_inputHandler->isKeyDown(InputHandler::UP)) _camera->RotateCamera(Camera::UP, deltaTime * 4);
	if (_inputHandler->isKeyDown(InputHandler::DOWN)) _camera->RotateCamera(Camera::DOWN, deltaTime * 4);
	if (_inputHandler->isKeyDown(InputHandler::W)) _camera->MoveCamera(Camera::FORWARD, deltaTime * 4);
	if (_inputHandler->isKeyDown(InputHandler::A)) _camera->MoveCamera(Camera::LEFT, deltaTime * 4);
	if (_inputHandler->isKeyDown(InputHandler::S)) _camera->MoveCamera(Camera::BACKWARD, deltaTime * 4);
	if (_inputHandler->isKeyDown(InputHandler::D)) _camera->MoveCamera(Camera::RIGHT,  deltaTime* 4);
}

void Game::Render()
{
	_renderer->ClearViews();

	const auto worldMatrix = _shipModel->CalculateWorldMatrix();

	XMMATRIX viewMatrix;
	_camera->CalculateViewMatrix(viewMatrix);

	XMMATRIX projMatrix;
	_camera->CalculateProjectionMatrix(*_clientWindow, projMatrix);

	auto wvp = worldMatrix * viewMatrix * projMatrix;

	Default3dWithLightingShader::ConstantBuffer cb;
	cb.gEyePosW = _camera->GetPos();
	cb.gLightCount = 3;
	
	cb.gMaterial.Ambient = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	cb.gMaterial.Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	cb.gMaterial.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);

	cb.gDirLight.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);	
    cb.gDirLight.Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	cb.gDirLight.Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
    cb.gDirLight.Direction = XMFLOAT3(0.0f, -0.707f, -0.707f);

	cb.gWorld = worldMatrix;
	cb.gWorldInvTranspose = math::InverseTranspose(worldMatrix);
	cb.gWorldViewProj = wvp;

	_renderer->RenderModel(*_shipModel, &cb);
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
		auto fps = (float)frameCnt; // fps = frameCnt / 1
		auto mspf = 1000.0f / fps;

		std::ostringstream outs;
		outs.precision(6);
		outs << "    "
			<< "FPS: " << fps << "    "
			<< "Frame Time: " << mspf << " (ms)";
		_clientWindow->UpdateCaption(outs.str());

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}