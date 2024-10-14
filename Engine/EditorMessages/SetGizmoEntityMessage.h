#pragma once

namespace EditorServer
{
	/*
		sent by editor to game to bring up the gizmo to edit a particular entity
	*/
	struct SetEntityGizmoMsg
	{
		size_t entity;
	};

	/*
		sent by editor to game to clear the gizmo
	*/
	struct ClearEntityGizmoMsg
	{

	};

	/*
		sent by editor to game to set the gizmo operation. See GizmoOperation.h in editor frontend dll
			- Translate = 0
			- Scale = 1
			- Rotate = 2
	*/
	struct SetGizmoOperationMsg
	{
		uint8_t operation;
	};
}