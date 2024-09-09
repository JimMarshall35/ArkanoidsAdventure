#pragma once
#include "EngineLib.h"
#include "ExportMacro.h"
#include <bitset>

#define MAX_LOGICAL_BUTNS 128
#define MAX_LOGICAL_AXES 128


namespace In
{


	typedef Handle_t HLogicalAxis;
	typedef Handle_t HLogicalBtn;

	enum class BindingType
	{
		Btn,
		Axis
	};

	enum class BtnBindingEndpointType
	{
		Keyboard,
		MouseBtn,
		ControllerBtn,
	};

	enum class AxisBindingEndpointType
	{
		MouseAbsolute,
		MouseDelta,
		LAnalogAbsolute,
		LAnalogDelta,
		RAnalogAbsolute,
		RAnalogDelta
	};
	struct LogicalButton;
	struct LogicalAxis;

	enum class Axis2D
	{
		X,
		Y
	};

	struct BindingEndpoint
	{
		struct BtnEndpoint
		{
			BtnBindingEndpointType type;
			HLogicalBtn btn;
			int code;
		};
		struct AxisEndpoint
		{
			AxisBindingEndpointType type;
			Axis2D axis;
			HLogicalAxis la;
		};
		BindingType type;
		union
		{
			BtnEndpoint btn;
			AxisEndpoint axis;
		}data;
	};

	struct LogicalButton
	{
		EString name;
		bool state[2] = { false, false };
		bool GetCurrentState();
		bool HasPressOccured();
		bool HasReleaseOccured();
	};

	enum class LogicalAxisType
	{
		Delta,   // change between frames reported 
		Absolute // each frame exact position of mouse, exact position of analog stick
	};

	struct LogicalAxis
	{
		LogicalAxisType type;
		EString name;
		double state;
	};

	struct InputSet
	{
		std::bitset<MAX_LOGICAL_AXES> enabledAxes;
		std::bitset<MAX_LOGICAL_BUTNS> enabledBtns;
		void operator=(const InputSet& rhs);
	};

	InputSet operator&(const InputSet& lhs, const InputSet& rhs);
	InputSet operator|(const InputSet& lhs, const InputSet& rhs);
	InputSet operator~(const InputSet& rhs);


	void PollInput();

	HLogicalBtn RegisterLogicalButton(const LogicalButton& btn, bool bInitiallyEnabled = false);
	HLogicalAxis RegisterLogicalAxis(const LogicalAxis& la, bool bInitiallyEnabled = false);
	HLogicalBtn FindLogicalBtnHandle(const char* name);
	HLogicalAxis FindLogicalAxisHandle(const char* name, LogicalAxisType type);

	void MapLogicalButtonToKeyboardKey(HLogicalBtn btn, int key);
	void MapLogicalButtonToMouseKey(HLogicalBtn btn, int key);

	void MapLogicalAxisToMouseX(HLogicalAxis name, LogicalAxisType type);
	void MapLogicalAxisToMouseY(HLogicalAxis name, LogicalAxisType type);

	void EnableDisableAxis(HLogicalAxis axis, bool enabled);
	void EnableDisableBtn(HLogicalBtn axis, bool enabled);

	InputSet GetCurrentInputSet();
	void SetCurrentInputSet(InputSet s);

	bool ENGINE_FRONTEND_API IsBtnEnabled(HLogicalBtn btn);
	bool ENGINE_FRONTEND_API IsAxisEnabled(HLogicalAxis btn);

	void ENGINE_FRONTEND_API SetVal(HLogicalAxis axis, double val);
	void ENGINE_FRONTEND_API SetVal(HLogicalBtn btn, bool val);

	double GetAxisVal(HLogicalAxis hAxis);
	bool GetBtnVal(HLogicalBtn hBtn);
	bool GetPressOccured(HLogicalBtn hBtn);
	bool GetReleaseOccured(HLogicalBtn hBtn);

};


struct BackendInputState
{
	std::array<In::LogicalButton, MAX_LOGICAL_BUTNS> btns;
	std::array<In::LogicalAxis, MAX_LOGICAL_AXES> axis;
	size_t numBtns = 0;
	size_t numAxes = 0;

	EVec<In::BindingEndpoint> endpts;
	std::bitset<MAX_LOGICAL_AXES> enabledAxes;
	std::bitset<MAX_LOGICAL_BUTNS> enabledBtns;
};