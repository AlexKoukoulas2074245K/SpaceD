/**********************************************************************/
/** inputhandler.cpp by Alex Koukoulas (C) 2017 All Rights Reserved  **/
/** File Description:                                                **/
/**********************************************************************/

#pragma once

// Local Headers
#include "inputhandler.h"
#include "util/clientwindow.h"

// Remote Headers

InputHandler::InputHandler(const ClientWindow& window)
	: _clientWindow(window)
	, _buttonCurrentState(0U)
	, _buttonPreviousState(0U)
	, _keyCurrentState(0U)
	, _keyPreviousState(0U)
	, _mousePos()
	, _mouseWheelState(MouseWheelState::NEUTRAL)
{
	_winVkDictionary[VK_LEFT]   = Key::LEFT;
	_winVkDictionary[VK_RIGHT]  = Key::RIGHT;
	_winVkDictionary[VK_UP]     = Key::UP;
	_winVkDictionary[VK_DOWN]   = Key::DOWN;
	_winVkDictionary[VK_BACK]   = Key::BACKSPACE;
	_winVkDictionary[VK_SPACE]  = Key::SPACE;
	_winVkDictionary[VK_RETURN] = Key::ENTER;
	_winVkDictionary[0x41]      = Key::A;
	_winVkDictionary[0x44]      = Key::D;
	_winVkDictionary[0x45]      = Key::E;
	_winVkDictionary[0x51]      = Key::Q;
	_winVkDictionary[0x53]      = Key::S;
	_winVkDictionary[0x57]      = Key::W;
	_winVkDictionary[0xBC]      = Key::COMMA;
	_winVkDictionary[0xBE]      = Key::DOT;
}

InputHandler::~InputHandler()
{

}

bool InputHandler::IsButtonDown(const InputHandler::Button button) const
{
	return (_buttonCurrentState & button) != 0;
}

bool InputHandler::IsButtonUp(const InputHandler::Button button) const
{
	return !IsButtonDown(button);
}

bool InputHandler::IsButtonTapped(const InputHandler::Button button) const
{
	return IsButtonDown(button) && (_buttonPreviousState & button) == 0;
}

bool InputHandler::IsKeyDown(const InputHandler::Key key) const
{
	return (_keyCurrentState & key) != 0;
}

bool InputHandler::IsKeyUp(const InputHandler::Key key) const
{
	return !IsKeyDown(key);
}

bool InputHandler::IsKeyTapped(const InputHandler::Key key) const
{
	return IsKeyDown(key) && (_keyPreviousState & key) == 0;
}

InputHandler::MouseWheelState InputHandler::GetMouseWheelState() const
{
	return _mouseWheelState;
}

const POINT& InputHandler::GetMousePos() const
{
	return _mousePos;
}

XMFLOAT2 InputHandler::GetMouseNDCCoords() const
{
	return math::MouseToNDC(_mousePos.x, _mousePos.y, _clientWindow.GetWidth(), _clientWindow.GetHeight());
}

UINT InputHandler::GetKey(WPARAM wParam) const
{
	if (_winVkDictionary.count(wParam))
	{
		return _winVkDictionary.at(wParam);
	}
	return Key::SPACE;
}

void InputHandler::OnMouseDown(const Button button, LPARAM lParam)
{
	_buttonCurrentState |= button;
	_mousePos.x = GET_X_LPARAM(lParam);
	_mousePos.y = GET_Y_LPARAM(lParam);
}

void InputHandler::OnMouseUp(const Button button, LPARAM lParam)
{
	_buttonCurrentState ^= button;	
	_mousePos.x = GET_X_LPARAM(lParam);
	_mousePos.y = GET_Y_LPARAM(lParam);
}

void InputHandler::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	_keyCurrentState |= _winVkDictionary[wParam];
}

void InputHandler::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
	_keyCurrentState ^= _winVkDictionary[wParam];
}

void InputHandler::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	_mousePos.x = GET_X_LPARAM(lParam);
	_mousePos.y = GET_Y_LPARAM(lParam);
}

void InputHandler::OnMouseWheelMove(WPARAM wParam, LPARAM lParam)
{
	_mouseWheelState = GET_WHEEL_DELTA_WPARAM(wParam) < 0 ? MouseWheelState::TOWARDS_USER : MouseWheelState::AWAY_FROM_USER;
}

void InputHandler::OnFrameEnd()
{
	_buttonPreviousState = _buttonCurrentState;	
	_keyPreviousState    = _keyCurrentState;
	_mouseWheelState     = MouseWheelState::NEUTRAL;
}