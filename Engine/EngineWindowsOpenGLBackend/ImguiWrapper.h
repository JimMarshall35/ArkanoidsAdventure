#pragma once
#include <glm/glm.hpp> // can't forward declare for some reason
#include "GizmoOperation.h"

class Transform;

namespace ImGuiWrapper
{
	void Init();
	void Update();
	void Draw();
	bool WantsMouseInput();
	bool WantsKeyboardInput();
	void SetGizmo(Transform* pM, glm::mat4* pV, glm::mat4* pP); // library isn't const correct so we can't be - view and projections should of course be const
	void ClearGizmo();
	void SetGizmoOperation(GizmoOperation gizmoOp);
}