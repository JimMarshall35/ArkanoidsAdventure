#include "Input.h"
#include <GLFW/glfw3.h>
#include "InputFrontend.h"
#include "Error.h"
#include "Sys.h"
#include "ImguiWrapper.h"
#include <bitset>

static double gCursorX[2];
static double gCursorY[2];
static double gCursorDeltaX = 0.0;
static double gCursorDeltaY = 0.0;
static std::bitset<GLFW_MOUSE_BUTTON_LAST> gMouseButtonStates;
#define GLFW_KEY_FIRST GLFW_KEY_SPACE
#define KEY_STATES_SIZE (GLFW_KEY_LAST - GLFW_KEY_FIRST)
static std::bitset<KEY_STATES_SIZE> gKeyBoardStates;

static int gAsciiLUT[256];
static int gMouseBtnLUT[(u32)MouseButtons::NUM];

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (action)
	{
	case GLFW_PRESS:
		gKeyBoardStates[key] = true;
		break;
	case GLFW_RELEASE:
		gKeyBoardStates[key] = false;
		break;
	}
}


static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
	gCursorX[1] = gCursorX[0];
	gCursorY[1] = gCursorY[0];

	gCursorX[0] = xpos;
	gCursorY[0] = ypos;

	gCursorDeltaX = gCursorX[1] - gCursorX[0];
	gCursorDeltaY = gCursorY[1] - gCursorY[0];
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	switch (action)
	{
	case GLFW_RELEASE:
		gMouseButtonStates[button] = false;
		break;
	case GLFW_PRESS:
		gMouseButtonStates[button] = true;
		break;
	default:
		EAssert(false);
		break;
	}

}

void BAInput::PollInput(BackendInputState& s)
{
	glfwPollEvents();

	for (const In::BindingEndpoint& ep : s.endpts)
	{
		switch (ep.type)
		{
		case In::BindingType::Axis:
			if (In::IsAxisEnabled(ep.data.axis.la))
			{
				In::HLogicalAxis h = ep.data.axis.la;
				switch (ep.data.axis.type)
				{
				case In::AxisBindingEndpointType::LAnalogAbsolute:
					break;
				case In::AxisBindingEndpointType::RAnalogAbsolute:
					break;
				case In::AxisBindingEndpointType::LAnalogDelta:
					break;
				case In::AxisBindingEndpointType::RAnalogDelta:
					break;
				case In::AxisBindingEndpointType::MouseDelta:
					if (!ImGuiWrapper::WantsMouseInput())
					{
						switch (ep.data.axis.axis)
						{
						case In::Axis2D::X:
							In::SetVal(h, gCursorDeltaX);
							break;
						case In::Axis2D::Y:
							In::SetVal(h, gCursorDeltaY);
							break;
						}
					}
					break;
				case In::AxisBindingEndpointType::MouseAbsolute:
					if (!ImGuiWrapper::WantsMouseInput())               // we might not want this behavior for absolute mouse - will see
					{
						switch (ep.data.axis.axis)
						{
						case In::Axis2D::X:
							In::SetVal(h, gCursorX[0]);
							break;
						case In::Axis2D::Y:
							In::SetVal(h, gCursorY[0]);
							break;
						}
					}
					break;
				default:
					break;
				}
			}
			break;
		case In::BindingType::Btn:
			if (In::IsBtnEnabled(ep.data.btn.btn))
			{
				In::HLogicalBtn b = ep.data.btn.btn;
				switch (ep.data.btn.type)
				{
				case In::BtnBindingEndpointType::MouseBtn:
					if (!ImGuiWrapper::WantsMouseInput())
					{
						In::SetVal(b, gMouseButtonStates[ep.data.btn.code]);
					}
					break;
				case In::BtnBindingEndpointType::ControllerBtn:
					break;
				case In::BtnBindingEndpointType::Keyboard:
					if (!ImGuiWrapper::WantsKeyboardInput())
					{
						In::SetVal(b, gKeyBoardStates[ep.data.btn.code]);
					}
					break;
				default:
					break;
				}
			}
			break;
		}
	}
	gCursorDeltaY = 0.0;
	gCursorDeltaX = 0.0;
}

void BAInput::Init(GLFWwindow* window)
{
	glfwSetCursorPosCallback(window, &CursorPosCallback);
	glfwSetMouseButtonCallback(window, &MouseButtonCallback);
	glfwSetKeyCallback(window, &KeyCallback);
	gMouseButtonStates.reset();
	gKeyBoardStates.reset();
	for (int i = 0; i < 256; i++)
	{
		if (i >= 'a' && i <= 'z')
		{
			gAsciiLUT[i] = toupper(i);
		}
		else
		{
			gAsciiLUT[i] = i;
		}
	}
	gMouseBtnLUT[(u32)MouseButtons::LEFT] = GLFW_MOUSE_BUTTON_LEFT;
	gMouseBtnLUT[(u32)MouseButtons::RIGHT] = GLFW_MOUSE_BUTTON_RIGHT;
	gMouseBtnLUT[(u32)MouseButtons::MIDDLE] = GLFW_MOUSE_BUTTON_MIDDLE;
	gMouseBtnLUT[(u32)MouseButtons::MB_4] = GLFW_MOUSE_BUTTON_4;
	gMouseBtnLUT[(u32)MouseButtons::MB_5] = GLFW_MOUSE_BUTTON_5;

}

int BAInput::GetInputCodeForAscii(char ascii)
{
	return gAsciiLUT[(unsigned char)ascii];
}

int BAInput::GetInputCodeForMouseBtn(MouseButtons mb)
{
	return gMouseBtnLUT[(u32)mb];
}

void BAInput::SetCursorMode(In::CursorMode md)
{
	switch (md)
	{
	case In::CursorMode::Normal:
		glfwSetInputMode(Sys::GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		break;
	case In::CursorMode::Disabled:
		glfwSetInputMode(Sys::GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		break;
	default:
		Log::LogMsg({ "[BAInput::SetCursorMode] invalid cursor mode.", BackendErrorSeverity::Error });
	}
}
