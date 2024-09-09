#include "Input.h"
#include <GLFW/glfw3.h>
#include "InputFrontend.h"


static double gCursorX[2];
static double gCursorY[2];
static double gCursorDeltaX = 0.0;
static double gCursorDeltaY = 0.0;
static bool gMouseButtonStates[GLFW_MOUSE_BUTTON_LAST];

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
					switch (ep.data.axis.axis)
					{
					case In::Axis2D::X:
						In::SetVal(h, gCursorDeltaX);
						break;
					case In::Axis2D::Y:
						In::SetVal(h, gCursorDeltaY);
						break;
					}
					break;
				case In::AxisBindingEndpointType::MouseAbsolute:
					switch (ep.data.axis.axis)
					{
					case In::Axis2D::X:
						In::SetVal(h, gCursorX[0]);
						break;
					case In::Axis2D::Y:
						In::SetVal(h, gCursorY[0]);
						break;
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
					In::SetVal(b, gMouseButtonStates[ep.data.btn.code]);
					break;
				case In::BtnBindingEndpointType::ControllerBtn:
					break;
				case In::BtnBindingEndpointType::Keyboard:
					break;
				default:
					break;
				}
			}
			break;
		}
	}
}

void BAInput::Init(GLFWwindow* window)
{
	glfwSetCursorPosCallback(window, &CursorPosCallback);
	glfwSetMouseButtonCallback(window, &MouseButtonCallback);
	memset(&gMouseButtonStates[0], 0, sizeof(gMouseButtonStates));
}
