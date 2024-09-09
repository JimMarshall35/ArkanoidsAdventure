#include "EditorSystems.h"
#include "Scene.h"
#include "TransformComponent.h"
#include "EditorCamControlsComponent.h"
#include "CameraComponent.h"
#include "InputFrontend.h"

static void EditorControls(Scn::Scene& s, double deltaT)
{
	EntityReg& r = s.entities.GetReg();
	auto v = r.view<Transform, EditorCamControlsComponent, CameraComponent>();
	v.each([](Transform& t, EditorCamControlsComponent& controls, CameraComponent& cam)
	{
		if (In::GetBtnVal(controls.ActivateButton))
		{
			const glm::vec3& rot = t.getLocalRotation();
			double dX = In::GetAxisVal(controls.YawAxis);
			double dY = In::GetAxisVal(controls.PitchAxis);
			t.setLocalRotation({ rot.x + (float)dX, rot.y + (float)dY, rot.z });
			cam.bViewMatrixDirty = true;
		}
	});
}

UpdateSystem gEditorControlsSystem = { &EditorControls, UpdateFunctionGroup::PrePhysics, EDITOR_CONTROLS_SYSTEM_NAME };