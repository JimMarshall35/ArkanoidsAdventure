#pragma once
#include "InputFrontend.h"
#include <array>
#include "ComponentReg.h"

struct EditorCamControlsComponent
{
	In::HLogicalAxis YawAxis = ENGINE_NULL_HANDLE;
	In::HLogicalAxis PitchAxis = ENGINE_NULL_HANDLE;
	In::HLogicalBtn ActivateButton = ENGINE_NULL_HANDLE;
	In::HLogicalBtn MoveSpeedUpBtn = ENGINE_NULL_HANDLE;
	In::HLogicalBtn MoveSpeedDownBtn = ENGINE_NULL_HANDLE;
	std::array<In::HLogicalBtn, (u32)Directions::NUMDIRECTIONS> MovementButtons;
	float MovementSpeed = 20.0f;
};

META_DECL(EditorCamControlsComponent)