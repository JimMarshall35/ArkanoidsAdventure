#pragma once
#include <functional>

namespace EditorServer
{
	struct Msg;
}
namespace EditorClient
{
	struct ClientInitArgs
	{

	};
	int InitClient(ClientInitArgs args = {});
	void DeInitClient();
	bool EnqueueToSend(const EditorServer::Msg& msg);
	bool RecieveQueueEmpty();
	bool PopRecieveQueue(EditorServer::Msg& msg);
	void GetAssetFolderPathAsync(std::function<void(const char*)> callback);
};

