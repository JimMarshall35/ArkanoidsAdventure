#include "pch.h"
#include "EditorServerMsg.h"
#include "MessageTypes.h"
#include <cassert>

namespace EditorServer
{

	Msg DeserializeMsg(unsigned char* data, size_t dataLen)
	{
		Msg msg;
		msg.Type = (MsgType)data[0];
		unsigned char* msgData = &data[1];
		switch (msg.Type)
		{
		case MsgType::GetSceneXML:
			msg.Data = GetSceneXmlMsg();
			break;
		case MsgType::GetSceneXML_Response:
			msg.Data = GetSceneXmlMsg_Response();
			assert(data[dataLen-1] == '\0');
			std::get<GetSceneXmlMsg_Response>(msg.Data).xml = (const char*)msgData;
			break;
		case MsgType::NewEntity:
			msg.Data = NewEntityMessage();
			assert(data[dataLen - 1] == '\0');
			std::get<NewEntityMessage>(msg.Data).xml = (const char*)msgData;
			break;
		case MsgType::NewEntity_Response:
			msg.Data = NewEntityMessage_Response();
			std::get<NewEntityMessage_Response>(msg.Data).bOK = *msgData;
			break;
		case MsgType::EditComponent:
		{
			size_t entity = *((size_t*)msgData);
			msgData += sizeof(size_t);
			msg.Data = EditComponentMsg(entity, (const char*)msgData);
			break;
		}
		case MsgType::EditComponent_Response:
		{
			bool bVal = *((bool*)msgData);
			msgData += sizeof(bool);
			msg.Data = EditComponentMsg_Response{ bVal };
			break;
		}
		case MsgType::EngineCmd:
		{
			msg.Data = EngineCmdMsg();
			assert(data[dataLen - 1] == '\0');
			std::get<EngineCmdMsg>(msg.Data).cmd = (const char*)msgData;
			break;
		}
		case MsgType::EngineLog:
		{
			msg.Data = EngineLogMsg();
			std::get<EngineLogMsg>(msg.Data).src = *((LogSource*)msgData);
			msgData += sizeof(LogSource);
			std::get<EngineLogMsg>(msg.Data).severity = *((LogSeverity*)msgData);
			msgData += sizeof(LogSeverity);
			std::get<EngineLogMsg>(msg.Data).msg = (const char*)msgData;
			break;
		}
		case MsgType::SetEntityGizmo:
		{
			msg.Data = SetEntityGizmoMsg{};
			std::get<SetEntityGizmoMsg>(msg.Data).entity = *((size_t*)msgData);
			break;
		}
		case MsgType::ClearGizmoEntity:
		{
			msg.Data = ClearEntityGizmoMsg{};
			break;
		}
		case MsgType::SetGizmoOperation:
		{
			msg.Data = SetGizmoOperationMsg{};
			std::get<SetGizmoOperationMsg>(msg.Data).operation = *((uint8_t*)msgData);
		}
		}
		return msg;
	}

	unsigned char* AllocateMsg(const Msg& msg, size_t& outSize)
	{
		outSize = 1; // 1 byte for msg type
		switch (msg.Type)
		{
		case MsgType::GetSceneXML:	 
			break;
		case MsgType::GetSceneXML_Response:
			outSize += std::get<GetSceneXmlMsg_Response>(msg.Data).xml.length() + 1;
			break;
		case MsgType::NewEntity:
			outSize += std::get<NewEntityMessage>(msg.Data).xml.length() + 1;
			break;
		case MsgType::NewEntity_Response:
			outSize += 1;
			break;
		case MsgType::EditComponent:
			outSize += (sizeof(size_t) + std::get<EditComponentMsg>(msg.Data).newComponentXml.length() + 1);
			break;
		case MsgType::EditComponent_Response:
			outSize += sizeof(bool);
			break;
		case MsgType::EngineCmd:
			outSize += std::get<EngineCmdMsg>(msg.Data).cmd.length() + 1;
			break;
		case MsgType::EngineLog:
			outSize += std::get<EngineLogMsg>(msg.Data).msg.length() + 1 + sizeof(LogSeverity) + sizeof(LogSource);
			break;
		case MsgType::SetEntityGizmo:
			outSize += sizeof(size_t);
			break;
		case MsgType::ClearGizmoEntity:
			break;
		case MsgType::SetGizmoOperation:
			outSize++;
			break;
		default:
			break;
		}
		return new unsigned char[outSize];
	}


	unsigned char* SerializeMsg(const Msg& msg, size_t& outSize)
	{
		unsigned char* pMsg = AllocateMsg(msg, outSize);
		pMsg[0] = (uint8_t)msg.Type;
		unsigned char* pWriteData = &pMsg[1];
		switch (msg.Type)
		{
		case MsgType::GetSceneXML:
			break;
		case MsgType::GetSceneXML_Response:
			strcpy((char*)pWriteData, std::get<GetSceneXmlMsg_Response>(msg.Data).xml.c_str());
			break;
		case MsgType::NewEntity:
			strcpy((char*)pWriteData, std::get<NewEntityMessage>(msg.Data).xml.c_str());
			break;
		case MsgType::NewEntity_Response:
			*pWriteData = (unsigned char)std::get< NewEntityMessage_Response>(msg.Data).bOK;
			break;
		case MsgType::EditComponent:
		{
			size_t* c = (size_t*)pWriteData;
			*c = std::get<EditComponentMsg>(msg.Data).entity;
			pWriteData += sizeof(size_t);
			strcpy((char*)pWriteData, std::get<EditComponentMsg>(msg.Data).newComponentXml.c_str());
			break;
		}
		case MsgType::EditComponent_Response:
		{
			bool* b = (bool*)pWriteData;
			*b = std::get<EditComponentMsg_Response>(msg.Data).bSucceeded;
			break;
		}
		case MsgType::EngineCmd:
		{
			strcpy((char*)pWriteData, std::get<EngineCmdMsg>(msg.Data).cmd.c_str());
			break;
		}
		case MsgType::EngineLog:
		{
			*((LogSource*)pWriteData) = std::get<EngineLogMsg>(msg.Data).src;
			pWriteData += sizeof(LogSource);
			*((LogSeverity*)pWriteData) = std::get<EngineLogMsg>(msg.Data).severity;
			pWriteData += sizeof(LogSeverity);
			strcpy((char*)pWriteData, std::get<EngineLogMsg>(msg.Data).msg.c_str());
			break;
		}
		case MsgType::SetEntityGizmo:
		{
			*((size_t*)pWriteData) = std::get<SetEntityGizmoMsg>(msg.Data).entity;
			break;
		}
		case MsgType::ClearGizmoEntity:
		{
			break;
		}
		case MsgType::SetGizmoOperation:
		{
			*((uint8_t*)pWriteData) = std::get<SetGizmoOperationMsg>(msg.Data).operation;

			break;
		}
		default:
			break;
		}

		return pMsg;
	}

}
