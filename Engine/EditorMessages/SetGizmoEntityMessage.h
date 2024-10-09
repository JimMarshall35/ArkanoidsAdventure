#pragma once

namespace EditorServer
{
	struct SetEntityGizmoMsg
	{
		size_t entity;
	};

	struct ClearEntityGizmoMsg
	{

	};

	struct SetGizmoOperationMsg
	{
		uint8_t operation;
	};
}