#pragma once

namespace Editor
{
	struct EditorServerInitArgs
	{

	};
	int Init(EditorServerInitArgs args);
	void DeInit();
	void PollEditorMessageQueue();
}