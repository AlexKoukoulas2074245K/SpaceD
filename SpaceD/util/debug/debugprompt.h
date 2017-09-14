/********************************************************************/
/** debugprompt.h by Alex Koukoulas (C) 2017 All Rights Reserved   **/
/** File Description:                                              **/
/********************************************************************/

#pragma once

// Local Headers

// Remote Headers
#include <unordered_map>
#include <string>
#include <functional>
#include <Windows.h>

class Renderer;
class Scene;
class InputHandler;

class DebugPrompt final
{
public:
	DebugPrompt(Renderer& renderer, Scene& scene, const InputHandler& inputHandler);
	~DebugPrompt();

	void Clear();
	void UpdateSingleKeyChar(const char keyChar, WPARAM wParam);
	void Update();
	void Render();

private:
	Renderer& _renderer;
	Scene& _scene;
	const InputHandler& _inputHandler;
	
	INT _historyIndex;
		
	std::string _currentCommand;
	std::vector<std::string> _commandOutput;
	std::vector<std::string> _commandHistory;
	std::unordered_map<std::string, std::function<bool(const std::vector<std::string>&)>> _commandDictionary;
};