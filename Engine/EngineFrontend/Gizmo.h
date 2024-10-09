#pragma once
#include "EngineLib.h"
#include "GizmoOperation.h"

namespace Gizmo
{
	void SetGizmo(Entity e);
	void DismissGizmo();
	void UpdateGizmo();
	void SetGizmoOperation(GizmoOperation op);
}