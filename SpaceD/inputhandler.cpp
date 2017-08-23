/**********************************************************************/
/** inputhandler.cpp by Alex Koukoulas (C) 2017 All Rights Reserved  **/
/** File Description:                                                **/
/**********************************************************************/

#pragma once

// Local Headers
#include "inputhandler.h"

// Remote Headers

InputHandler::InputHandler()
	: _buttonCurrentState(0U)
	, _buttonPreviousState(0U)
	, _keyCurrentState(0U)
	, _keyPreviousState(0U)
	, _mousePos()
	, _mouseWheelState(MouseWheelState::NEUTRAL)
{
	_winVkDictionary[VK_LEFT]  = Key::LEFT;
	_winVkDictionary[VK_RIGHT] = Key::RIGHT;
	_winVkDictionary[VK_UP]    = Key::UP;
	_winVkDictionary[VK_DOWN]  = Key::DOWN;
	_winVkDictionary[0x41]     = Key::A;
	_winVkDictionary[0x44]     = Key::D;
	_winVkDictionary[0x53]     = Key::S;
	_winVkDictionary[0x57]     = Key::W;
}

InputHandler::~InputHandler()
{

}

bool InputHandler::isButtonDown(const InputHandler::Button button) const
{
	return (_buttonCurrentState & button) != 0;
}

bool InputHandler::isButtonUp(const InputHandler::Button button) const
{
	return !isButtonDown(button);
}

bool InputHandler::isButtonTapped(const InputHandler::Button button) const
{
	return isButtonDown(button) && (_buttonPreviousState & button) == 0;
}

bool InputHandler::isKeyDown(const InputHandler::Key key) const
{
	return (_keyCurrentState & key) != 0;
}

bool InputHandler::isKeyUp(const InputHandler::Key key) const
{
	return !isKeyDown(key);
}

bool InputHandler::isKeyTapped(const InputHandler::Key key) const
{
	return isKeyDown(key) && (_keyPreviousState & key) == 0;
}

InputHandler::MouseWheelState InputHandler::getMouseWheelState() const
{
	return _mouseWheelState;
}

const POINT& InputHandler::getMousePos() const
{
	return _mousePos;
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