#include "InputFrontend.h"
#include "EngineDLLInterface.h"
#include "FrontendError.h"
#include <string.h>
#include "IBackendApp.h"
#include "IArchive.h"

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
				Err::LogWarning("Logical Button Name '%s' already taken - cannot register", btn.name.c_str());
				return ENGINE_NULL_HANDLE;
			}
		}
		if (gBackendInputState.numBtns + 1 == MAX_LOGICAL_BUTNS)
		{
			Err::LogWarning("RegisterLogicalButton, MAX_LOGICAL_BUTNS exceeded");
			return ENGINE_NULL_HANDLE;
		}
		gBackendInputState.btns[gBackendInputState.numBtns++] = btn;
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
					Err::LogWarning("Logical Axis Name '%s' and type '%i' already taken - cannot register", la.name.c_str(), (int)la.type);
					return ENGINE_NULL_HANDLE;
				}
			}
		}
		if (gBackendInputState.numAxes + 1 == MAX_LOGICAL_AXES)
		{
			Err::LogWarning("RegisterLogicalButton, MAX_LOGICAL_AXES exceeded");
			return ENGINE_NULL_HANDLE;
		}
		gBackendInputState.axis[gBackendInputState.numAxes++] = la;
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
			Err::LogError("TryGetBtn, Logical btn handle '%i' out of range", (int)bn);
			return nullptr;
		}
		return GetBtn(bn);
	}

	static LogicalAxis* TryGetAxis(HLogicalAxis h)
	{
		if (CheckAxisHandle(h))
		{
			Err::LogError("TryGetBtn, Logical btn handle '%i' out of range", (int)h);
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
			Err::LogError("FindLogicalBtnHandle, Can't find logical btn '%s'", name);
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
			Err::LogError("FindLogicalAxisHandle, Can't find logical axis '%s'", name);
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
			Err::LogError("MapLogicalButtonToKeyboardKey, Logical btn handle '%i' out of range", (int)h);
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
			Err::LogError("MapLogicalButtonToMouseKey, Logical btn handle '%i' out of range", (int)h);
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
			Err::LogError("MapLogicalAxisToMouseX, Logical Axis handle '%i' out of range", (int)h);
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
			Err::LogError("MapLogicalAxisToMouseY, Logical Axis handle '%i' out of range", (int)h);
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
			Err::LogError("EnableDisableAxis, Logical Axis handle '%i' out of range", (int)h);
			return;
		}
		gBackendInputState.enabledAxes[h - 1] = enabled;
	}

	void EnableDisableBtn(HLogicalBtn h, bool enabled)
	{
		if (CheckBtnHandle(h))
		{
			Err::LogError("EnableDisableBtn, Logical Btn handle '%i' out of range", (int)h);
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
			Err::LogError("IsBtnEnabled, Logical Btn handle '%i' out of range", (int)btn);
			EAssert(false);
			return false;
		}
		return gBackendInputState.enabledBtns[btn - 1];
	}

	bool IsAxisEnabled(HLogicalAxis btn)
	{
		if (CheckAxisHandle(btn))
		{
			Err::LogError("IsBtnEnabled, Logical Btn handle '%i' out of range", (int)btn);
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

	void Serialize(IArchive& archive)
	{
		gBackendInputState.Serialize(archive);
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
	void BindingEndpoint::Serialize(IArchive& ar)
	{
		if (ar.IsStoring())
		{
			//ar.PushObj("BindingEndpoint");
				ar.PushObj("type");
					ar << (int)type;
					switch (type)
					{
					case In::BindingType::Btn:
						ar.PushObj("Btn");
							ar.PushObj("type");
								ar << (int)data.btn.type;
							ar.PopObj();
							ar.PushObj("hBtn");
								ar << data.btn.btn;
							ar.PopObj();
							ar.PushObj("code");
								ar << data.btn.code;
							ar.PopObj();
						ar.PopObj();
						break;
					case In::BindingType::Axis:
						ar.PushObj("Axis");
							ar.PushObj("type");
								ar << (int)data.axis.type;
							ar.PopObj();
							ar.PushObj("axis");
								ar << (int)data.axis.axis;
							ar.PopObj();
							ar.PushObj("code");
								ar << (int)data.axis.la;
							ar.PopObj();
						ar.PopObj();
						break;
					}
				ar.PopObj();
			//ar.PopObj();
		}
		else
		{
			//ar.PushObj("BindingEndpoint");
				ar.PushObj("type");
					int t = 0;
					ar >> t;//(int&)type;
					type = (In::BindingType)t;
					switch (type)
					{
					case In::BindingType::Btn:
						ar.PushObj("Btn");
							ar.PushObj("type");
								ar >> (int&)data.btn.type;
							ar.PopObj();
							ar.PushObj("hBtn");
								ar >> (size_t&)data.btn.btn;
							ar.PopObj();
							ar.PushObj("code");
								ar >> data.btn.code;
							ar.PopObj();
						ar.PopObj();
						break;
					case In::BindingType::Axis:
						ar.PushObj("Axis");
							ar.PushObj("type");
								ar >> (int&)data.axis.type;
							ar.PopObj();
							ar.PushObj("axis");
								ar >> (int&)data.axis.axis;
							ar.PopObj();
							ar.PushObj("code");
								ar >> (int&)data.axis.la;
							ar.PopObj();
						ar.PopObj();
						break;
					}
				ar.PopObj();
			//ar.PopObj();
		}
	}
}

template<size_t size>
void SerializeBitset64(IArchive& ar, std::bitset<size>& bitset, const char* serializationName)
{
	static_assert(size % 64 == 0);
	if (ar.IsStoring())
	{
		ar.PushObj(serializationName);
			u64* pRead = (u64*)&bitset;
			for (int i = 0; i < size / 64; i++)
			{
				ar.PushObj("BitsetElement");
					ar << *pRead++;
				ar.PopObj();
			}
		ar.PopObj();
	}
	else
	{
		ar.PushObj(serializationName);
			u64* pWrite = (u64*)&bitset;
			for (int i = 0; i < size / 64; i++)
			{
				ar.PushObj("BitsetElement");
					ar >> *pWrite++;
				ar.PopObj();
			}
		ar.PopObj();
	}
}

void BackendInputState::Serialize(IArchive& ar)
{
	int version = 1;

	if (ar.IsStoring())
	{
		ar.PushObj("Input");
			ar << version;

			ar.PushObj("numBtns");
				ar << numBtns;
			ar.PopObj();
			ar.PushObj("Buttons");
				for(int i=0; i<numBtns; i++)
				{
					ar.PushObj("Button");
						ar.PushObj("Name");
							ar << btns[i].name;
						ar.PopObj();
					ar.PopObj();
				}
			ar.PopObj();

			ar.PushObj("numAxes");
				ar << numAxes;
			ar.PopObj();
			ar.PushObj("Axes");
				for(int i=0; i<numAxes; i++)
				{
					ar.PushObj("Axis");
						ar.PushObj("Name");
							ar << axis[i].name;
						ar.PopObj();
						ar.PushObj("Type");
							ar << (int)axis[i].type;
						ar.PopObj();
					ar.PopObj();
				}
			ar.PopObj();

			ar.PushObj("BindingEndpoints");
				ar << endpts.size();
				for (In::BindingEndpoint& ep : endpts)
				{
					ar.PushObj("BindingEndpoint");
						ep.Serialize(ar);
					ar.PopObj();
				}
			ar.PopObj();

			SerializeBitset64<MAX_LOGICAL_AXES>(ar, enabledAxes, "enabledAxes");
			SerializeBitset64<MAX_LOGICAL_BUTNS>(ar, enabledBtns, "enabledBtns");

		ar.PopObj();
	}
	else
	{
		ar.PushObj("Input");
			ar >> version;
			switch (version)
			{
			case 1:
			{
				ar.PushObj("numBtns");
					ar >> numBtns;
				ar.PopObj();
				ar.PushObj("Buttons");
					for(int i=0; i<numBtns; i++)
					{
						ar.PushChild(i);
							ar.PushObj("Name");
								ar >> btns[i].name;
							ar.PopObj();
						ar.PopObj();
					}
				ar.PopObj();

				ar.PushObj("numAxes");
					ar >> numAxes;
				ar.PopObj();
				ar.PushObj("Axes");
					for(int i=0; i<numAxes; i++)
					{
						ar.PushChild(i);
							ar.PushObj("Name");
								ar >> axis[i].name;
							ar.PopObj();
							ar.PushObj("Type");
								ar >> (int&)axis[i].type;
							ar.PopObj();
						ar.PopObj();
					}
				ar.PopObj();

				ar.PushObj("BindingEndpoints");
					size_t enptsSize = 0;
					ar >> enptsSize;
					endpts.resize(enptsSize);

					for(int i=0; i< enptsSize; i++)
					{
						In::BindingEndpoint& ep = endpts[i];
						ar.PushChild(i);
						ep.Serialize(ar);
						ar.PopObj();
					}
				ar.PopObj();

				SerializeBitset64<MAX_LOGICAL_AXES>(ar, enabledAxes, "enabledAxes");
				SerializeBitset64<MAX_LOGICAL_BUTNS>(ar, enabledBtns, "enabledBtns");
				break;
			}
				
			}
			

		ar.PopObj();
	}
}
