#include "EditorSystems.h"
#include "Scene.h"
#include "TransformComponent.h"
#include "EditorCamControlsComponent.h"
#include "CameraComponent.h"
#include "InputFrontend.h"
#include "IBackendApp.h"
#include "FrontendError.h"


#define SENSITIVITY 0.4f
#define EDITOR_CAM_MOVEMENT_SPEED 20.0f

EString gEntityPalette[ENTITY_PALETTE_SIZE];                                                                                                 

static void EditorControls(Scn::Scene& s, double deltaT)
{
	EntityReg& r = s.entities.GetReg();
	auto v = r.view<Transform, EditorCamControlsComponent, CameraComponent>();
	v.each([deltaT](Transform& t, EditorCamControlsComponent& controls, CameraComponent& cam)
	{
		
		if (In::GetBtnVal(controls.ActivateButton))
		{
			const glm::vec3& rot = t.getLocalRotation();
			double dX = In::GetAxisVal(controls.PitchAxis);
			double dY = In::GetAxisVal(controls.YawAxis);
			t.Rotate((float)dX * SENSITIVITY, (float)dY * SENSITIVITY, 0);
			cam.bViewMatrixDirty = true;
			if (In::GetPressOccured(controls.ActivateButton))
			{
				Engine::GetAPI().SetCursorMode(In::CursorMode::Disabled);
				Err::LogInfo("cursor disabled");
			}
		}
		else if (In::GetReleaseOccured(controls.ActivateButton))
		{
			Engine::GetAPI().SetCursorMode(In::CursorMode::Normal);
			Err::LogInfo("cursor enabled");
		}

		glm::vec3 velocity = { 0.0f, 0.0f, 0.0f };
		bool bMoved = false;
		if (In::GetBtnVal(controls.MovementButtons[(u32)Directions::FORWARD]))
		{
			velocity += t.getForward();
			bMoved = true;
		}
		if (In::GetBtnVal(controls.MovementButtons[(u32)Directions::BACKWARD]))
		{
			velocity += t.getBackward();
			bMoved = true;
		}
		if (In::GetBtnVal(controls.MovementButtons[(u32)Directions::LEFT]))
		{
			velocity += -t.getRight();
			bMoved = true;
		}
		if (In::GetBtnVal(controls.MovementButtons[(u32)Directions::RIGHT]))
		{
			velocity += t.getRight();
			bMoved = true;
		}
		if (In::GetBtnVal(controls.MovementButtons[(u32)Directions::UP]))
		{
			velocity += glm::vec3{0.0f, 1.0f, 0.0f};
			bMoved = true;
		}
		if (In::GetBtnVal(controls.MovementButtons[(u32)Directions::DOWN]))
		{
			velocity += glm::vec3{ 0.0f, -1.0f, 0.0f };
			bMoved = true;
		}

		bool bSpeedChanged = false;
		if (In::GetPressOccured(controls.MoveSpeedUpBtn))
		{
			controls.MovementSpeed *= 2.0f;
			bSpeedChanged = true;
		}
		if (In::GetPressOccured(controls.MoveSpeedDownBtn))
		{
			controls.MovementSpeed /= 2.0f;
			bSpeedChanged = true;
		}

		if (In::GetPressOccured(controls.Ent1) && gEntityPalette[0] != "")
		{
			Err::LogInfo("Ent1");
		}
		if (In::GetPressOccured(controls.Ent2) && gEntityPalette[1] != "")
		{
			Err::LogInfo("Ent2");
		}
		if (In::GetPressOccured(controls.Ent3) && gEntityPalette[2] != "")
		{
			Err::LogInfo("Ent3");
		}
		if (In::GetPressOccured(controls.Ent4) && gEntityPalette[3] != "")
		{
			Err::LogInfo("Ent4");
		}


		if (bSpeedChanged)
		{
			Err::LogInfo("Editor Cam speed %.2f", controls.MovementSpeed);
		}

		if (bMoved)
		{
			velocity = glm::normalize(velocity) * (float)(controls.MovementSpeed * deltaT/1000.0);
			t.Translate(velocity.x, velocity.y, velocity.z);
			cam.bViewMatrixDirty = true;

		}
	});
}

UpdateSystem gEditorControlsSystem = { &EditorControls, UpdateFunctionGroup::PrePhysics, EDITOR_CONTROLS_SYSTEM_NAME };

void SetEntityPaletteEntry(const char* e, int paletteIndex)
{
	if (paletteIndex >= ENTITY_PALETTE_SIZE)
	{
		Err::LogWarning("SetEntityPaletteEntry: index %i out of range", paletteIndex);
		return;
	}
	gEntityPalette[paletteIndex] = e;
}
