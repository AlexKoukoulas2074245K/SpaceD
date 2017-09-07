/********************************************************************/
/** inputhandler.h by Alex Koukoulas (C) 2017 All Rights Reserved  **/
/** File Description:                                              **/
/********************************************************************/

#pragma once

// Local Headers
#include "rendering/d3dcommon.h"
#include "util/math.h"

// Remote Headers
#include <unordered_map>
#include <Windows.h>
#include <Windowsx.h>

class Game;
class ClientWindow;

class InputHandler final
{
	friend class Game;
public:
	enum Button
	{		
		LMBUTTON = 1, RMBUTTON = 2
	};

	enum Key
	{
		LEFT = 1, RIGHT = 2, UP = 4, DOWN = 8, W = 16, A = 32, S = 64, D = 128, E = 256, Q = 512
	};

	enum MouseWheelState
	{
		AWAY_FROM_USER, NEUTRAL, TOWARDS_USER
	};

public:
	InputHandler(const ClientWindow& window);
	~InputHandler();

	// Accessor Methods
	bool IsButtonDown(const Button button) const;
	bool IsButtonUp(const Button button) const;
	bool IsButtonTapped(const Button button) const;

	bool IsKeyDown(const Key key) const;
	bool IsKeyUp(const Key key) const;
	bool IsKeyTapped(const Key key) const;

	MouseWheelState GetMouseWheelState() const;
	const POINT& GetMousePos() const;
	XMFLOAT2 GetMouseNDCCoords() const;

	// Update Methods
	void OnMouseDown(const Button button, LPARAM lParam);
	void OnMouseUp(const Button button, LPARAM lParam);

	void OnKeyDown(WPARAM wParam, LPARAM lParam);
	void OnKeyUp(WPARAM wParam, LPARAM lParam);

	void OnMouseMove(WPARAM wParam, LPARAM lParam);
	void OnMouseWheelMove(WPARAM wParam, LPARAM lParam);
	void OnFrameEnd();

private:
	const ClientWindow& _clientWindow;
	
	BYTE  _buttonCurrentState;
	BYTE  _buttonPreviousState;

	DWORD _keyCurrentState;
	DWORD _keyPreviousState;

	POINT _mousePos;
	MouseWheelState _mouseWheelState;
	
	std::unordered_map<UINT, UINT> _winVkDictionary;
};