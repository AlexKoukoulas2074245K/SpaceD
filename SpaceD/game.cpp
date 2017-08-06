/*********************************************************************/
/** game.cpp by Alex Koukoulas (C) 2017 All Rights Reserved         **/
/** File Description:                                               **/
/*********************************************************************/

// Remote Headers
#include <sstream>

// Local Headers
#include "game.h"
#include "util\clientwindow.h"
#include "util\gametimer.h"

namespace
{
	Game* game = 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return game->MsgProc(hwnd, msg, wParam, lParam);
}

Game::Game(HINSTANCE hInstance, const LPCSTR clientName, const int clientWidth, const int clientHeight)
	: _clientWindow(nullptr)
	, _gameTimer(std::make_unique<GameTimer>())
	, _paused(false)
	, _minimized(false)
	, _maximized(false)
	, _resizing(false)
{	
	// Delayed window creation to allow assignment to global Game pointer (Window procs can not 
	// be members of a class)
	game = this;
	_clientWindow = std::make_unique<ClientWindow>(hInstance, WndProc, clientName, clientWidth, clientHeight);
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
		else
		{
			_gameTimer->Tick();
			CalculateFrameStats();
			//updateStateManager();
			//renderStateManager();
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
			int newWidth = LOWORD(lParam);
			int newHeight = HIWORD(lParam);
            
			if (_clientWindow)
			{
				_clientWindow->UpdateOnResize(newWidth, newHeight);
			}

			if (true)//md3dDevice)
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
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		{
			OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));			
		} break;

		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		{
			OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));			
		} break;

		case WM_MOUSEMOVE:
		{
			OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		} break;		
	}

	return DefWindowProc(handle, msg, wParam, lParam);
}

void Game::OnResize()
{
	OutputDebugString("Resize\n");
	/*
	assert(md3dImmediateContext);
	assert(md3dDevice);
	assert(mSwapChain);

	// Release the old views, as they hold references to the buffers we
	// will be destroying.  Also release the old depth/stencil buffer.

	ReleaseCOM(mRenderTargetView);
	ReleaseCOM(mDepthStencilView);
	ReleaseCOM(mDepthStencilBuffer);


	// Resize the swap chain and recreate the render target view.

	HR(mSwapChain->ResizeBuffers(1, mClientWidth, mClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	ID3D11Texture2D* backBuffer;
	HR(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));
	HR(md3dDevice->CreateRenderTargetView(backBuffer, 0, &mRenderTargetView));
	ReleaseCOM(backBuffer);

	// Create the depth/stencil buffer and view.

	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = mClientWidth;
	depthStencilDesc.Height = mClientHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// Use 4X MSAA? --must match swap chain MSAA values.
	if (mEnable4xMsaa)
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	// No MSAA
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	HR(md3dDevice->CreateTexture2D(&depthStencilDesc, 0, &mDepthStencilBuffer));
	HR(md3dDevice->CreateDepthStencilView(mDepthStencilBuffer, 0, &mDepthStencilView));


	// Bind the render target view and depth/stencil view to the pipeline.

	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);


	// Set the viewport transform.

	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width = static_cast<float>(mClientWidth);
	mScreenViewport.Height = static_cast<float>(mClientHeight);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);
	*/
}

void Game::OnMouseDown(WPARAM btnState, int x, int y)
{
}

void Game::OnMouseUp(WPARAM btnState, int x, int y)
{
}

void Game::OnMouseMove(WPARAM btnState, int x, int y)
{
}

void Game::Update()
{
}

void Game::Render()
{
}

void Game::CalculateFrameStats()
{
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if ((_gameTimer->TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

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