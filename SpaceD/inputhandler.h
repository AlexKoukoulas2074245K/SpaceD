/********************************************************************/
/** inputhandler.h by Alex Koukoulas (C) 2017 All Rights Reserved  **/
/** File Description:                                              **/
/********************************************************************/

#pragma once

// Local Headers
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
		LEFT      = 0x00000001,
		RIGHT     = 0x00000002,
		UP        = 0x00000004,
		DOWN      = 0x00000008,
		W         = 0x00000010,
		A         = 0x00000020,
		S         = 0x00000040,
		D         = 0x00000080,
		E         = 0x00000100,
		Q         = 0x00000200,
		BACKSPACE = 0x00000400,
		SPACE     = 0x00000800,
		DOT       = 0x00001000,
		COMMA     = 0x00002000
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

	UINT GetKey(WPARAM wParam) const;

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