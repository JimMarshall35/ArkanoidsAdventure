#include "EditorCamControlsComponent.h"
#include "IArchive.h"
static void SerializeC(Comp::ComponentMeta* m, IArchive* ar, Entity e, EntityReg& reg)
{
	if (ar->IsStoring())
	{
		if (EditorCamControlsComponent* mc = reg.try_get<EditorCamControlsComponent>(e))
		{
			const int version = 1;
			ar->PushObj("EditorCamControlsComponent");
				*ar << version;
				AR_STORE(mc, YawAxis)
				AR_STORE(mc, PitchAxis)
				AR_STORE(mc, ActivateButton)
				AR_STORE(mc, MoveSpeedUpBtn)
				AR_STORE(mc, MoveSpeedDownBtn)
				ar->PushObj("UpBtn");
					*ar << mc->MovementButtons[(u32)Directions::UP];
					ar->HintUsage(HandleType::LogicalBtn);
				ar->PopObj();
				ar->PushObj("DownBtn");
					*ar << mc->MovementButtons[(u32)Directions::DOWN];
					ar->HintUsage(HandleType::LogicalBtn);
				ar->PopObj();
				ar->PushObj("LeftBtn");
					*ar << mc->MovementButtons[(u32)Directions::LEFT];
					ar->HintUsage(HandleType::LogicalBtn);
				ar->PopObj();
				ar->PushObj("RightBtn");
					*ar << mc->MovementButtons[(u32)Directions::RIGHT];
					ar->HintUsage(HandleType::LogicalBtn);
				ar->PopObj();
				ar->PushObj("ForwardBtn");
					*ar << mc->MovementButtons[(u32)Directions::FORWARD];
					ar->HintUsage(HandleType::LogicalBtn);
				ar->PopObj();
				ar->PushObj("BackwardBtn");
					*ar << mc->MovementButtons[(u32)Directions::BACKWARD];
					ar->HintUsage(HandleType::LogicalBtn);
				ar->PopObj();
			ar->PopObj();
		}
	}
	else
	{
		int fileVersion = 0;
		if (!ar->PushObj("TestPipelineMaterial")) return;
		EditorCamControlsComponent* mc = reg.try_get<EditorCamControlsComponent>(e);
		if (!mc)
		{
			reg.emplace<EditorCamControlsComponent>(e);
			mc = reg.try_get<EditorCamControlsComponent>(e);
		}
		*ar >> fileVersion;
		switch (fileVersion)
		{
		case 1:
		{
			AR_LOAD(mc, YawAxis)
			AR_LOAD(mc, PitchAxis)
			AR_LOAD(mc, ActivateButton)
			AR_LOAD(mc, MoveSpeedUpBtn)
			AR_LOAD(mc, MoveSpeedDownBtn)
			ar->PushObj("UpBtn");
				*ar >> mc->MovementButtons[(u32)Directions::UP];
			ar->PopObj();
			ar->PushObj("DownBtn");
				*ar >> mc->MovementButtons[(u32)Directions::DOWN];
			ar->PopObj();
			ar->PushObj("LeftBtn");
				*ar >> mc->MovementButtons[(u32)Directions::LEFT];
			ar->PopObj();
			ar->PushObj("RightBtn");
				*ar >> mc->MovementButtons[(u32)Directions::RIGHT];
			ar->PopObj();
			ar->PushObj("ForwardBtn");
				*ar >> mc->MovementButtons[(u32)Directions::FORWARD];
			ar->PopObj();
			ar->PushObj("BackwardBtn");
				*ar >> mc->MovementButtons[(u32)Directions::BACKWARD];
			ar->PopObj();
			break;
		}
		default:

			break;
		}

		ar->PopObj();
	}
}

static void MetaReg(Comp::ComponentMeta* m) 
{

}

META_IMPL(EditorCamControlsComponent, MetaReg, SerializeC)