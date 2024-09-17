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
			//pWriteData[outSize - 1] = '\0';
			break;
		case MsgType::NewEntity:
			strcpy((char*)pWriteData, std::get<NewEntityMessage>(msg.Data).xml.c_str());
			//pWriteData[outSize - 1] = '\0';
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
		default:
			break;
		}

		return pMsg;
	}

}
