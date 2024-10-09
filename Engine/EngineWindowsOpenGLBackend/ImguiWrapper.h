#pragma once
#include <glm/glm.hpp> // can't forward declare for some reason
#include "GizmoOperation.h"

class Transform;

namespace ImGuiWrapper
{
	void Init();
	bool UpdateGizmo(glm::mat4* pV, glm::mat4* pP);
	void EndFrame();
	void BeginFrame();
	bool WantsMouseInput();
	bool WantsKeyboardInput();
	void SetGizmo(Transform* pM);
	void ClearGizmo();
	void SetGizmoOperation(GizmoOperation gizmoOp);
}