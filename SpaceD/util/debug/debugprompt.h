/********************************************************************/
/** debugprompt.h by Alex Koukoulas (C) 2017 All Rights Reserved   **/
/** File Description:                                              **/
/********************************************************************/

#pragma once

// Local Headers

// Remote Headers
#include <string>

class Renderer;
class InputHandler;

class DebugPrompt final
{
public:
	DebugPrompt(Renderer& renderer, const InputHandler& inputHandler);
	~DebugPrompt();

	void Clear();
	void UpdateSingleKeyChar(const char keyChar);
	void Update();
	void Render();

private:
	Renderer& _renderer;
	const InputHandler& _inputHandler;

	std::string _currentCommand;
};