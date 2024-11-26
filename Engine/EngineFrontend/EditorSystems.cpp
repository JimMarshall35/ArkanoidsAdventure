#include "EditorSystems.h"
#include "Scene.h"
#include "TransformComponent.h"
#include "EditorCamControlsComponent.h"
#include "CameraComponent.h"
#include "InputFrontend.h"
#include "IBackendApp.h"
#include "FrontendError.h"
#include "pugixml.hpp"
#include "XMLArchive.h"
#include "ComponentReg.h"
#include "MeshComponent.h"

#define SENSITIVITY 0.4f
#define EDITOR_CAM_MOVEMENT_SPEED 20.0f

EString gEntityPalette[ENTITY_PALETTE_SIZE];                                                                                                 

static void ConstructEntityFromPaletteSlot(size_t paletteSlot, Transform& camTransform)
{
	if (paletteSlot >= ENTITY_PALETTE_SIZE)
	{
		Err::LogError("ConstructEntityFromPaletteSlot: palette index '%i' out of range. Max is %i", paletteSlot, ENTITY_PALETTE_SIZE - 1);
		return;
	}
	if (gEntityPalette[paletteSlot] == "")
	{
		return;
	}
	XMLArchive ar(gEntityPalette[paletteSlot].c_str());
	ar.SetStoring(false);
	ar.PushObj("Prefab");
	EntityReg& reg = Scn::GetScene().entities.GetReg();
	Entity e = reg.create();
	for (pugi::xml_node comp : ar.TopNode())
	{
		auto& map = Comp::GetComponentMetaMap();
		if (map.find(comp.name()) != map.end())
		{
			map.at(comp.name())->Serialize(&ar, e, reg);
		}
	}

	// if transform present, set it to be in front of the camera
	const float distanceAway = 5.0f;
	if (Transform* transform = reg.try_get<Transform>(e))
	{
		transform->setLocalPosition(camTransform.getLocalPosition() + camTransform.getForward() * distanceAway);
		transform->SetParent(Scn::GetScene().rootTransformEntity);
	}

	// if mesh compnent present, initialise it by creating drawable
	if (MeshComponent* mesh = reg.try_get<MeshComponent>(e))
	{
		const BackendAPI& api = Engine::GetAPI();
		if (PipeLine* pl = PipeLine::TryGetPipeline(mesh->m_hPipeline))
		{
			mesh->Create(mesh->m_hMesh, *pl, e);
		}
	}
}

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
			ConstructEntityFromPaletteSlot(0, t);
		}
		if (In::GetPressOccured(controls.Ent2) && gEntityPalette[1] != "")
		{
			ConstructEntityFromPaletteSlot(1, t);
		}
		if (In::GetPressOccured(controls.Ent3) && gEntityPalette[2] != "")
		{
			ConstructEntityFromPaletteSlot(2, t);
		}
		if (In::GetPressOccured(controls.Ent4) && gEntityPalette[3] != "")
		{
			ConstructEntityFromPaletteSlot(3, t);
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

bool SetEntityPaletteEntry(const char* e, int paletteIndex)
{
	if (paletteIndex >= ENTITY_PALETTE_SIZE || paletteIndex < 0)
	{
		Err::LogWarning("SetEntityPaletteEntry: index %i out of range", paletteIndex);
		return false;
	}
	gEntityPalette[paletteIndex] = e;
	return true;
}
