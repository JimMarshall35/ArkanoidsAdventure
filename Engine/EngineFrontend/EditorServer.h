#pragma once

namespace EditorServer
{
	struct Msg;
}

namespace Editor
{
	struct EditorServerInitArgs
	{

	};
	int Init(EditorServerInitArgs args);
	void DeInit();
	void PollEditorMessageQueue();
	bool IsEditorConnected();
	bool EnqueueMsg(const EditorServer::Msg& msg);
}