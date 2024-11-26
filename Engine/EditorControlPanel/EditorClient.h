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

	/// <summary>
	/// callback params:
	///		- prefab name
	///		- slot
	///		- successful or not
	/// </summary>
	void SetPrefabPaletteAsync(const char* prefabName, const char* xml, char slot, std::function<void(const char*, char, bool)> callback);
};

