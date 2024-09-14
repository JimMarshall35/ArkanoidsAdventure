#pragma once
#include "EngineLib.h"
#include "InputFrontend.h"

struct BackendInputState;
struct GLFWwindow;

namespace BAInput
{
	void PollInput(BackendInputState& s);
	void Init(GLFWwindow* window);
	int GetInputCodeForAscii(char ascii);
	int GetInputCodeForMouseBtn(MouseButtons mb);
	void SetCursorMode(In::CursorMode md);

}