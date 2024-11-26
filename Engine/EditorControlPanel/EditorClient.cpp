#include "pch.h"
#include "EditorClient.h"
#include "ThreadSafeQueue.h"
#include <thread>
#include <mutex>
#include <optional>
#include <atomic>
#include <cassert>
#include "EditorServerMsg.h"
#include "CommonEditorServerDefines.h"
#include <chrono>
#include <vector>
#include <algorithm>

#ifdef _DEBUG
#define verify(v) assert(v)
#else
#define verify(v) v
#endif

namespace EditorClient
{
    static SOCKET gConnectSocket = INVALID_SOCKET;
    static std::optional<std::thread> gClientRecThread;
    static std::optional<std::thread> gClientSendThread;
    
    static std::atomic<bool> gThreadsContinue = true;
    TSQueueFixedSizeStackAllocatedQueue<EDITOR_GUI_SEND_QUEUE_SIZE,EditorServer::Msg> gSendQueue("send queue");
    TSQueueFixedSizeStackAllocatedQueue<EDITOR_GUI_RECIEVE_QUEUE_SIZE, EditorServer::Msg> gRecieveQueue("recieve queue");

    std::vector<std::function<void(const char*)>> gGetAssetFolderPathCallbacks;


    struct SetPrefabPaletteCallbackData
    {
        std::string prefabName; char slot;  
        std::function<void(const char*, char, bool)> callback;
        bool wasSuccessful = false;
        void Call()
        {
            callback(prefabName.c_str(), slot, wasSuccessful);
        }
    };

    std::vector<SetPrefabPaletteCallbackData> gSetPrefabPaletteXMLCallbacks;
    /* was the message handled as an async callback? */
    bool DoAsyncCallbackTypeMessages(EditorServer::Msg& msg)
    {
        switch (msg.Type)
        {
        case EditorServer::MsgType::RequestAssetsFolderPath_Response:
            for (std::function<void(const char*)> callback : gGetAssetFolderPathCallbacks)
            {
                auto r = std::get<EditorServer::RequestAssetsFolderPath_Response>(msg.Data);
                callback(r.absolutePath.c_str());
            }
            gGetAssetFolderPathCallbacks.clear();
            return true;
        case EditorServer::MsgType::SetPrefabPaletteSlotMsg_Response:
            for (SetPrefabPaletteCallbackData& cb : gSetPrefabPaletteXMLCallbacks)
            {
                cb.wasSuccessful = std::get<EditorServer::SetPrefabPaletteSlotMsg_Response>(msg.Data).bSuccess;
                cb.Call();
            }
            gSetPrefabPaletteXMLCallbacks.clear();
            return true;

        }
        return false;
    }

    void ClientRecieveThread()
    {

        int recvbuflen = EDITOR_GUI_RECIEVE_MSG_BUFFER_SIZE;
        static char recvbuf[EDITOR_GUI_RECIEVE_MSG_BUFFER_SIZE];
        int iResult = 0;
        
        // Receive data until the server closes the connection
        do {
            
            iResult = recv(gConnectSocket, recvbuf, recvbuflen, 0);
            if (iResult > 0)
            {
                printf("Bytes received: %d\n", iResult);
                EditorServer::Msg msg = EditorServer::DeserializeMsg((unsigned char*)&recvbuf[0], iResult);
                gRecieveQueue.Push(msg);
            }
            else if (iResult == 0)
            {
                printf("Connection closed\n");
            }
            else
            {
                printf("recv failed: %d\n", WSAGetLastError());
            }
        } while (gThreadsContinue && iResult > 0);
    }

    void ClientSendThread()
    {
        while (gThreadsContinue)
        {
            while (!gSendQueue.Empty())
            {
                EditorServer::Msg msg;
                verify(!gSendQueue.Pop(msg));
                size_t size = 0;
                unsigned char* serializedMsg = EditorServer::SerializeMsg(msg, size);
                
                int iResult = send(gConnectSocket, (const char*)serializedMsg, size, 0);

                delete[] serializedMsg;
                if (iResult == SOCKET_ERROR) {
                    printf("send failed: %d\n", WSAGetLastError());
                    closesocket(gConnectSocket);
                    WSACleanup();
                    return;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    }

    int InitClient(ClientInitArgs args)
    {
        WSADATA wsaData;
        int iResult;

        // Initialize Winsock
        iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (iResult != 0) {
            printf("WSAStartup failed: %d\n", iResult);
            return 1;
        }

        struct addrinfo* result = NULL,
            * ptr = NULL,
            hints;

        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;

        // Resolve the server address and port
        iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);
        if (iResult != 0) {
            printf("getaddrinfo failed: %d\n", iResult);
            WSACleanup();
            return 1;
        }
        // Attempt to connect to the first address returned by
        // the call to getaddrinfo
        ptr = result;

        // Create a SOCKET for connecting to server
        gConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (gConnectSocket == INVALID_SOCKET) {
            printf("Error at socket(): %ld\n", WSAGetLastError());
            freeaddrinfo(result);
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = SOCKET_ERROR;
        while(iResult == SOCKET_ERROR)iResult = connect(gConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(gConnectSocket);
            gConnectSocket = INVALID_SOCKET;
        }
        // Should really try the next address returned by getaddrinfo
        // if the connect call failed
        // But for this simple example we just free the resources
        // returned by getaddrinfo and print an error message

        freeaddrinfo(result);

        if (gConnectSocket == INVALID_SOCKET) {
            printf("Unable to connect to server!\n");
            WSACleanup();
            return 1;
        }

        gClientRecThread = std::thread(&ClientRecieveThread);
        gClientSendThread = std::thread(&ClientSendThread);
        return 0;
    }

    void DeInitClient()
    {
        int iResult = shutdown(gConnectSocket, SD_BOTH);
        if (iResult == SOCKET_ERROR) {
            printf("shutdown failed: %d\n", WSAGetLastError());
            closesocket(gConnectSocket);
            WSACleanup();
            return;
        }
        gThreadsContinue = false;
        gClientRecThread.value().join();
    }

    bool EnqueueToSend(const EditorServer::Msg& msg)
    {
        return gSendQueue.Push(msg);
    }
    bool RecieveQueueEmpty()
    {
        return gRecieveQueue.Empty();
    }
    bool PopRecieveQueue(EditorServer::Msg& msg)
    {
        bool bVal = gRecieveQueue.Pop(msg);
        DoAsyncCallbackTypeMessages(msg);
        return bVal;
    }
    void GetAssetFolderPathAsync(std::function<void(const char*)> callback)
    {
        gGetAssetFolderPathCallbacks.push_back(callback);
        EditorServer::Msg msg;
        msg.Type = EditorServer::MsgType::RequestAssetsFolderPath;
        msg.Data = EditorServer::RequestAssetsFolderPath{};
        gSendQueue.Push(msg);
    }
    void SetPrefabPaletteAsync(const char* prefabName, const char* xml, char slot, std::function<void(const char*, char, bool)> callback)
    {
        gSetPrefabPaletteXMLCallbacks.push_back({ prefabName, slot, callback});
        EditorServer::Msg msg;
        msg.Type = EditorServer::MsgType::SetPrefabPaletteSlotMsg;
        msg.Data = EditorServer::SetPrefabPaletteSlotMsg{slot, xml};
        gSendQueue.Push(msg);
    }
}

