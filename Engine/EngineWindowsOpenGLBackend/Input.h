#pragma once

struct BackendInputState;
struct GLFWwindow;

namespace BAInput
{
	void PollInput(BackendInputState& s);
	void Init(GLFWwindow* window);
}