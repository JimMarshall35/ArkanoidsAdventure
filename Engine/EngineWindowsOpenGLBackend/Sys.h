#pragma once
#include "IBackendApp.h"

struct BackendInputState;
struct GLFWwindow;

namespace Sys
{
	GLFWwindow* GetWindow();
	void InitWindow(int& outW, int& outH);
	bool ShouldGameContinue();
	void Cleanup();
	double GetTime();
	void SwapBuffers();
	void RegisterWindowResizeHandler(BackendWindowResizeFn fn);


	int GetW();
	int GetH();
}