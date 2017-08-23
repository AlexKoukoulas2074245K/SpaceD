/********************************************************************/
/** inputhandler.h by Alex Koukoulas (C) 2017 All Rights Reserved  **/
/** File Description:                                              **/
/********************************************************************/

#pragma once

// Local Headers

// Remote Headers
#include <unordered_map>
#include <Windows.h>
#include <Windowsx.h>

class Game;
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
		LEFT = 1, RIGHT = 2, UP = 4, DOWN = 8, W = 16, A = 32, S = 64, D = 128
	};

	enum MouseWheelState
	{
		AWAY_FROM_USER, NEUTRAL, TOWARDS_USER
	};

public:
	InputHandler();
	~InputHandler();

	// Accessor Methods
	bool isButtonDown(const Button button) const;
	bool isButtonUp(const Button button) const;
	bool isButtonTapped(const Button button) const;

	bool isKeyDown(const Key key) const;
	bool isKeyUp(const Key key) const;
	bool isKeyTapped(const Key key) const;

	MouseWheelState getMouseWheelState() const;
	const POINT& getMousePos() const;

	// Update Methods
	void OnMouseDown(const Button button, LPARAM lParam);
	void OnMouseUp(const Button button, LPARAM lParam);

	void OnKeyDown(WPARAM wParam, LPARAM lParam);
	void OnKeyUp(WPARAM wParam, LPARAM lParam);

	void OnMouseMove(WPARAM wParam, LPARAM lParam);
	void OnMouseWheelMove(WPARAM wParam, LPARAM lParam);
	void OnFrameEnd();

private:
	BYTE  _buttonCurrentState;
	BYTE  _buttonPreviousState;

	DWORD _keyCurrentState;
	DWORD _keyPreviousState;

	POINT _mousePos;
	MouseWheelState _mouseWheelState;

	std::unordered_map<UINT, UINT> _winVkDictionary;
};