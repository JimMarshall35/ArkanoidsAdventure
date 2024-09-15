#pragma once
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
};

