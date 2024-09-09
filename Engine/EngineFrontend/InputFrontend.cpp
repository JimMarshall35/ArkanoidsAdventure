#include "InputFrontend.h"
#include "EngineDLLInterface.h"
#include "FrontendError.h"
#include <string.h>
#include "IBackendApp.h"

namespace In
{
	static BackendInputState gBackendInputState;


	void PollInput()
	{
		const BackendAPI& api = Engine::GetAPI();
		api.PollInput(gBackendInputState);
	}

	HLogicalBtn RegisterLogicalButton(const LogicalButton& btn, bool bInitiallyEnabled /*= false*/)
	{
		for (int i=0; i < gBackendInputState.numBtns; i++)
		{
			LogicalButton& bn = gBackendInputState.btns[i];
			if (bn.name == btn.name)
			{
				Err::ReportError(Err::FrontendErrorSeverity::Warning, "Logical Button Name '%s' already taken - cannot register", btn.name.c_str());
				return ENGINE_NULL_HANDLE;
			}
		}
		if (gBackendInputState.numBtns + 1 == MAX_LOGICAL_BUTNS)
		{
			Err::ReportError(Err::FrontendErrorSeverity::Warning, "RegisterLogicalButton, MAX_LOGICAL_BUTNS exceeded");
			return ENGINE_NULL_HANDLE;
		}
		gBackendInputState.btns[gBackendInputState.numBtns++];
		gBackendInputState.enabledBtns[gBackendInputState.numBtns - 1] = bInitiallyEnabled;
		return gBackendInputState.numBtns;
	}

	HLogicalAxis RegisterLogicalAxis(const LogicalAxis& la, bool bInitiallyEnabled /*= false*/)
	{
		for (int i = 0; i < gBackendInputState.numAxes; i++)
		{
			LogicalAxis& bn = gBackendInputState.axis[i];
			if (bn.name == la.name)
			{
				if (bn.type == la.type)
				{
					Err::ReportError(Err::FrontendErrorSeverity::Warning, "Logical Axis Name '%s' and type '%i' already taken - cannot register", la.name.c_str(), (int)la.type);
					return ENGINE_NULL_HANDLE;
				}
			}
		}
		if (gBackendInputState.numAxes + 1 == MAX_LOGICAL_AXES)
		{
			Err::ReportError(Err::FrontendErrorSeverity::Warning, "RegisterLogicalButton, MAX_LOGICAL_AXES exceeded");
			return ENGINE_NULL_HANDLE;
		}
		gBackendInputState.axis[gBackendInputState.numAxes++];
		gBackendInputState.enabledAxes[gBackendInputState.numAxes - 1] = bInitiallyEnabled;
		return gBackendInputState.numAxes;
	}

	static LogicalButton* FindLB(const char* name)
	{
		auto itr = std::find_if(gBackendInputState.btns.begin(), gBackendInputState.btns.begin() + gBackendInputState.numBtns,
			[name](const LogicalButton& lb) -> bool { return 0 == strcmp(lb.name.c_str(), name); });
		if (itr == gBackendInputState.btns.end())
		{
			return nullptr;
		}
		return &(*itr);
	}

	static LogicalAxis* FindLA(const char* name, LogicalAxisType type)
	{
		auto itr = std::find_if(gBackendInputState.axis.begin(), gBackendInputState.axis.begin() + gBackendInputState.numAxes, [type, name](const LogicalAxis& lb) -> bool {
			return 0 == strcmp(lb.name.c_str(), name) && lb.type == type;
		});
		if (itr == gBackendInputState.axis.end())
		{
			return nullptr;
		}
		return &(*itr);
	}

	static bool CheckAxisHandle(HLogicalAxis h)
	{
		return (h <= 0) || (h > gBackendInputState.numAxes);
	}

	static bool CheckBtnHandle(HLogicalBtn h)
	{
		return (h <= 0) || (h > gBackendInputState.numBtns);
	}

	static LogicalButton* GetBtn(HLogicalBtn h)
	{
		return &gBackendInputState.btns[h - 1];
	}

	static LogicalAxis* GetAxis(HLogicalAxis h)
	{
		return &gBackendInputState.axis[h - 1];
	}

	static LogicalButton* TryGetBtn(HLogicalBtn bn)
	{
		if (CheckBtnHandle(bn))
		{
			Err::ReportError(Err::FrontendErrorSeverity::Error, "TryGetBtn, Logical btn handle '%i' out of range", (int)bn);
			return nullptr;
		}
		return GetBtn(bn);
	}

	static LogicalAxis* TryGetAxis(HLogicalAxis h)
	{
		if (CheckAxisHandle(h))
		{
			Err::ReportError(Err::FrontendErrorSeverity::Error, "TryGetBtn, Logical btn handle '%i' out of range", (int)h);
			return nullptr;
		}
		return GetAxis(h);
	}

	HLogicalBtn FindLogicalBtnHandle(const char* name)
	{
		auto itr = std::find_if(gBackendInputState.btns.begin(), gBackendInputState.btns.begin() + gBackendInputState.numBtns,
			[name](const LogicalButton& lb) -> bool { return 0 == strcmp(lb.name.c_str(), name); });
		if (itr == gBackendInputState.btns.end())
		{
			Err::ReportError(Err::FrontendErrorSeverity::Error, "FindLogicalBtnHandle, Can't find logical btn '%s'", name);
			return ENGINE_NULL_HANDLE;
		}
		return (itr - gBackendInputState.btns.begin()) + 1;
	}

	HLogicalAxis FindLogicalAxisHandle(const char* name, LogicalAxisType type)
	{
		auto itr = std::find_if(gBackendInputState.axis.begin(), gBackendInputState.axis.begin() + gBackendInputState.numAxes, [type, name](const LogicalAxis& lb) -> bool {
			return 0 == strcmp(lb.name.c_str(), name) && lb.type == type;
		});
		if (itr == gBackendInputState.axis.end())
		{
			Err::ReportError(Err::FrontendErrorSeverity::Error, "FindLogicalAxisHandle, Can't find logical axis '%s'", name);
			return ENGINE_NULL_HANDLE;
		}
		return (itr - gBackendInputState.axis.begin()) + 1;
	}

	void MapLogicalButtonToKeyboardKey(HLogicalBtn h, int key)
	{
		BindingEndpoint ep;
		ep.type = BindingType::Btn;
		if (CheckBtnHandle(h))
		{
			Err::ReportError(Err::FrontendErrorSeverity::Error, "MapLogicalButtonToKeyboardKey, Logical btn handle '%i' out of range", (int)h);
		}
		ep.data.btn.btn = h;
		ep.data.btn.code = key;
		ep.data.btn.type = BtnBindingEndpointType::Keyboard;
		gBackendInputState.endpts.push_back(ep);
	}

	void MapLogicalButtonToMouseKey(HLogicalBtn h, int key)
	{
		BindingEndpoint ep;
		ep.type = BindingType::Btn;
		if (CheckBtnHandle(h))
		{
			Err::ReportError(Err::FrontendErrorSeverity::Error, "MapLogicalButtonToMouseKey, Logical btn handle '%i' out of range", (int)h);
		}
		ep.data.btn.btn = h;
		ep.data.btn.code = key;
		ep.data.btn.type = BtnBindingEndpointType::MouseBtn;
		gBackendInputState.endpts.push_back(ep);
	}

	void MapLogicalAxisToMouseX(HLogicalAxis h, LogicalAxisType type)
	{
		BindingEndpoint ep;
		ep.type = BindingType::Axis;
		if (CheckAxisHandle(h))
		{
			Err::ReportError(Err::FrontendErrorSeverity::Error, "MapLogicalAxisToMouseX, Logical Axis handle '%i' out of range", (int)h);
			return;
		}

		ep.data.axis.la = h;
		ep.type = BindingType::Axis;
		ep.data.axis.axis = Axis2D::X;
		ep.data.axis.type = type == LogicalAxisType::Absolute ? AxisBindingEndpointType::MouseAbsolute : AxisBindingEndpointType::MouseDelta;
		gBackendInputState.endpts.push_back(ep);
	}

	void MapLogicalAxisToMouseY(HLogicalAxis h, LogicalAxisType type)
	{
		BindingEndpoint ep;
		ep.type = BindingType::Axis;
		if (CheckAxisHandle(h))
		{
			Err::ReportError(Err::FrontendErrorSeverity::Error, "MapLogicalAxisToMouseY, Logical Axis handle '%i' out of range", (int)h);
			return;
		}

		ep.data.axis.la = h;
		ep.type = BindingType::Axis;
		ep.data.axis.axis = Axis2D::Y;
		ep.data.axis.type = type == LogicalAxisType::Absolute ? AxisBindingEndpointType::MouseAbsolute : AxisBindingEndpointType::MouseDelta;
		gBackendInputState.endpts.push_back(ep);
	}

	void EnableDisableAxis(HLogicalAxis h, bool enabled)
	{
		if (CheckAxisHandle(h))
		{
			Err::ReportError(Err::FrontendErrorSeverity::Error, "EnableDisableAxis, Logical Axis handle '%i' out of range", (int)h);
			return;
		}
		gBackendInputState.enabledAxes[h - 1] = enabled;
	}

	void EnableDisableBtn(HLogicalBtn h, bool enabled)
	{
		if (CheckBtnHandle(h))
		{
			Err::ReportError(Err::FrontendErrorSeverity::Error, "EnableDisableBtn, Logical Btn handle '%i' out of range", (int)h);
			return;
		}
		gBackendInputState.enabledBtns[h - 1] = enabled;
	}

	InputSet operator&(const InputSet& lhs, const InputSet& rhs)
	{
		return InputSet{ lhs.enabledAxes & rhs.enabledAxes, lhs.enabledBtns & rhs.enabledBtns };
	}

	InputSet operator|(const InputSet& lhs, const InputSet& rhs)
	{
		return InputSet{ lhs.enabledAxes | rhs.enabledAxes, lhs.enabledBtns | rhs.enabledBtns };
	}

	InputSet operator~(const InputSet& rhs)
	{
		return InputSet{~rhs.enabledAxes, ~rhs.enabledBtns};
	}

	InputSet GetCurrentInputSet()
	{
		return { gBackendInputState.enabledAxes, gBackendInputState.enabledBtns };
	}

	void SetCurrentInputSet(InputSet s)
	{
		gBackendInputState.enabledAxes = s.enabledAxes;
		gBackendInputState.enabledBtns = s.enabledBtns;
	}

	bool IsBtnEnabled(HLogicalBtn btn)
	{
		if (CheckBtnHandle(btn))
		{
			Err::ReportError(Err::FrontendErrorSeverity::Error, "IsBtnEnabled, Logical Btn handle '%i' out of range", (int)btn);
			EAssert(false);
			return false;
		}
		return gBackendInputState.enabledBtns[btn - 1];
	}

	bool IsAxisEnabled(HLogicalAxis btn)
	{
		if (CheckAxisHandle(btn))
		{
			Err::ReportError(Err::FrontendErrorSeverity::Error, "IsBtnEnabled, Logical Btn handle '%i' out of range", (int)btn);
			EAssert(false);
			return false;
		}
		return gBackendInputState.enabledAxes[btn - 1];
	}

	void SetVal(HLogicalAxis axis, double val)
	{
		GetAxis(axis)->state = val;
	}

	void SetVal(HLogicalBtn btn, bool val)
	{
		LogicalButton* pBtn = GetBtn(btn);
		bool temp = pBtn->state[0];
		pBtn->state[0] = val;
		pBtn->state[1] = temp;
	}

	double GetAxisVal(HLogicalAxis hAxis)
	{
		if (LogicalAxis* axis = TryGetAxis(hAxis))
		{
			return axis->state;
		}
		return 0.0;
	}

	bool GetBtnVal(HLogicalBtn hBtn)
	{
		if (LogicalButton* pBtn = TryGetBtn(hBtn))
		{
			return pBtn->GetCurrentState();
		}
		return false;
	}

	bool GetPressOccured(HLogicalBtn hBtn)
	{
		if (LogicalButton* pBtn = TryGetBtn(hBtn))
		{
			return pBtn->HasPressOccured();
		}
		return false;
	}

	bool GetReleaseOccured(HLogicalBtn hBtn)
	{
		if (LogicalButton* pBtn = TryGetBtn(hBtn))
		{
			return pBtn->HasReleaseOccured();
		}
		return false;
	}

	void InputSet::operator=(const InputSet& rhs)
	{
		enabledAxes = rhs.enabledAxes;
		enabledBtns = rhs.enabledBtns;
	}

	bool LogicalButton::GetCurrentState()
	{
		return state[0];
	}
	
	bool LogicalButton::HasPressOccured()
	{
		return state[0] && !state[1];
	}
	
	bool LogicalButton::HasReleaseOccured()
	{
		return !state[0] && state[1];
	}
}