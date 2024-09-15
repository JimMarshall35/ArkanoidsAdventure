#include <stdio.h>
#include "EditorServer.h"
#include "FrontendError.h"
#include <optional>
#include <thread>
#include <atomic>
#include <chrono>
#include "EngineLib.h"
#include "ThreadSafeQueue.h"
#include "CommonEditorServerDefines.h"
#include "EditorServerMsg.h"
#include "NewEntityMessage.h"
#include "GetSceneXMLMessage.h"
#include "MessageTypes.h"
#include "Scene.h"
#include "XMLArchive.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
#pragma comment(lib, "Ws2_32.lib")

namespace Editor {
#ifdef _WIN32

	static SOCKET gListenSocket = INVALID_SOCKET;
	static SOCKET gClientSocket = INVALID_SOCKET;

	static TSQueueFixedSizeStackAllocatedQueue<ENGINE_SEND_QUEUE_SIZE, EditorServer::Msg> gSendQueue("send queue");
	static TSQueueFixedSizeStackAllocatedQueue<ENGINE_RECIEVE_QUEUE_SIZE, EditorServer::Msg> gRecieveQueue("recieve queue");


	static std::optional<std::thread> gServerRecieveThread = std::nullopt;
	static std::optional<std::thread> gServerSendThread = std::nullopt;

	static std::atomic<bool> bServerThreadContinue = true;

	int InitSocket()
	{
		struct addrinfo* result = NULL, * ptr = NULL, hints;

		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		// Resolve the local address and port to be used by the server
		int iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
		if (iResult != 0)
		{
			Err::LogError("getaddrinfo failed: %d\n", iResult);
			WSACleanup();
			return 1;
		}
		gListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (gListenSocket == INVALID_SOCKET)
		{
			Err::LogError("Error at socket(): %ld\n", WSAGetLastError());
			freeaddrinfo(result);
			WSACleanup();
			return 1;
		}

		iResult = bind(gListenSocket, result->ai_addr, (int)result->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			printf("bind failed with error: %d\n", WSAGetLastError());
			freeaddrinfo(result);
			closesocket(gListenSocket);
			WSACleanup();
			return 1;
		}
		return 0;
	}

	void HandleRecievedEditorMsg(const EditorServer::Msg& msg)
	{
		switch (msg.Type)
		{
		case EditorServer::MsgType::GetSceneXML:
			{
				XMLArchive ar("", true);
				Scn::SerializeScene(ar);

				EditorServer::Msg msg;
				msg.Type = EditorServer::MsgType::GetSceneXML_Response;
				EString s = ar.AsString();
				msg.Data = EditorServer::GetSceneXmlMsg_Response{ std::move(s) };
				gSendQueue.Push(msg);
				break;
			}
		case EditorServer::MsgType::NewEntity:
			{
				EditorServer::Msg msg;
				msg.Type = EditorServer::MsgType::NewEntity_Response;
				msg.Data = EditorServer::NewEntityMessage_Response{true};
				gSendQueue.Push(msg);
				break;
			}
		}
	}

	void PollEditorMessageQueue()
	{
		while (!gRecieveQueue.Empty())
		{
			EditorServer::Msg msg;
			EVerify(!gRecieveQueue.Pop(msg));
			HandleRecievedEditorMsg(msg);
		}
	}

	void ServerSendThread()
	{
		while (bServerThreadContinue)
		{
			while (!gSendQueue.Empty())
			{
				EditorServer::Msg msg;
				EVerify(!gSendQueue.Pop(msg));
				size_t serializedSize = 0;
				unsigned char* serialized = EditorServer::SerializeMsg(msg, serializedSize);
				int iSendResult = send(gClientSocket, (const char*)serialized, serializedSize, 0);
				if (iSendResult == SOCKET_ERROR)
				{
					Err::LogError("[ServerSendThread] send failed: %d\n", WSAGetLastError());
					closesocket(gClientSocket);
					WSACleanup();
					break;
				}
				Err::LogInfo("Bytes sent: %d\n", iSendResult);
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
		}
	}

	void ServerRecieveThread()
	{
		char* recvbuf = new char[ENGINE_RECIEVE_MSG_BUFFER_SIZE];

		while (bServerThreadContinue)
		{
			// Accept a client socket
			gClientSocket = accept(gListenSocket, NULL, NULL);
			if (gClientSocket == INVALID_SOCKET)
			{
				Err::LogError("accept failed: %d\n", WSAGetLastError());
				closesocket(gListenSocket);
				WSACleanup();
				return;
			}
			Err::LogInfo("Editor Connected");
			std::thread sendThread(&ServerSendThread);
			int iResult, iSendResult;
			int recvbuflen = ENGINE_RECIEVE_MSG_BUFFER_SIZE;

			// Receive until the peer shuts down the connection
			do
			{
				iResult = recv(gClientSocket, recvbuf, recvbuflen, 0);
				if (iResult > 0)
				{
					printf("Bytes received: %d\n", iResult);
					EditorServer::Msg msg = EditorServer::DeserializeMsg((unsigned char*)recvbuf, iResult);
					gRecieveQueue.Push(msg);
				}
				else if (iResult == 0)
				{
					printf("Connection closing...\n");
				}
				else
				{
					printf("recv failed: %d\n", WSAGetLastError());
					closesocket(gClientSocket);
					WSACleanup();
					break;
				}
			} while (bServerThreadContinue && iResult > 0);
			
			bServerThreadContinue = false;// set this to false to kill the send thread
			sendThread.join();
			bServerThreadContinue = true; // ...but set it back to true again
		}
		delete[] recvbuf;
	}

	int Init(EditorServerInitArgs args)
	{
		WSADATA wsaData;
		int iResult;

		// Initialize Winsock
		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0)
		{
			Err::LogError("WSAStartup failed: %d\n", iResult);
			return 1;
		}
		if (int i = InitSocket())
		{
			return i;
		}
		if (listen(gListenSocket, SOMAXCONN) == SOCKET_ERROR)
		{
			Err::LogError("Listen failed with error: %ld\n", WSAGetLastError());
			closesocket(gListenSocket);
			WSACleanup();
			return 1;
		}

		gServerRecieveThread = std::thread(&ServerRecieveThread);

		return 0;
	}

	void DeInit()
	{
		if (gServerRecieveThread.has_value())
		{
			EAssert(bServerThreadContinue);
			bServerThreadContinue = false;
			gServerRecieveThread.value().join();
		}
	}
#else
	void Editor::Init(EditorServerInitArgs args)
	{
		Err::LogError("Editor server only implemented on windows");
	}
	void DeInit()
	{
	}
#endif

}