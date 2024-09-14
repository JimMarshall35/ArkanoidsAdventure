#include "EditorSystems.h"
#include "Scene.h"
#include "TransformComponent.h"
#include "EditorCamControlsComponent.h"
#include "CameraComponent.h"
#include "InputFrontend.h"

#define SENSITIVITY 0.4f
static void EditorControls(Scn::Scene& s, double deltaT)
{
	EntityReg& r = s.entities.GetReg();
	auto v = r.view<Transform, EditorCamControlsComponent, CameraComponent>();
	v.each([](Transform& t, EditorCamControlsComponent& controls, CameraComponent& cam)
	{
		if (In::GetBtnVal(controls.ActivateButton))
		{
			const glm::vec3& rot = t.getLocalRotation();
			double dX = In::GetAxisVal(controls.PitchAxis );
			double dY = In::GetAxisVal(controls.YawAxis);
			t.Rotate((float)dX * SENSITIVITY, (float)dY * SENSITIVITY, 0);
			cam.bViewMatrixDirty = true;
		}
	});
}

UpdateSystem gEditorControlsSystem = { &EditorControls, UpdateFunctionGroup::PrePhysics, EDITOR_CONTROLS_SYSTEM_NAME };