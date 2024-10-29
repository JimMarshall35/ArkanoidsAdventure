#include "pch.h"
#include "MainPropertySheet.h"
#include "resource.h"
#include "EditorClient.h"
#include "EditorServerMsg.h"
#include "TexturePropertyPage.h"
#include "EntitiesPropertyPage.h"
#include "CommonPropertyPageDefines.h"


BEGIN_MESSAGE_MAP(MainPropertySheet, CPropertySheet)
END_MESSAGE_MAP()

MainPropertySheet::MainPropertySheet()
{
	m_psh.dwFlags |= PSH_NOAPPLYNOW;
	m_psh.dwFlags &= ~(PSH_HASHELP);
	GetXMLSceneFn getScn = [this]() {return m_Scn; };
	m_pEntitiesPropertyPage = new CEntitiesPropertyPage(getScn);
	m_pTexturesPropertySheetPage = new TexturesPropertyPage(getScn);
	AddPage(m_pEntitiesPropertyPage);
	AddPage(m_pTexturesPropertySheetPage);
}

MainPropertySheet::~MainPropertySheet()
{
	if (m_pTexturesPropertySheetPage)
	{
		delete m_pTexturesPropertySheetPage;
	}
	if (m_pEntitiesPropertyPage)
	{
		delete m_pEntitiesPropertyPage;
	}
}

BOOL MainPropertySheet::OnInitDialog()
{
	
    BOOL res =  CPropertySheet::OnInitDialog();
	GetDlgItem(IDOK)->ShowWindow(SW_HIDE);
	GetDlgItem(IDCANCEL)->ShowWindow(SW_HIDE);

	EditorClient::InitClient();

	return res;
}

void MainPropertySheet::HandleMessageRecieved(const EditorServer::Msg& msg)
{
	switch (msg.Type)
	{
	case EditorServer::MsgType::GetSceneXML_Response:
		HandleGetSceneXmlResponseMsg(msg);
		m_pEntitiesPropertyPage->HandleMsgRecieved(msg);
		m_pTexturesPropertySheetPage->HandleMsgRecieved(msg);
		break;
	case EditorServer::MsgType::NewEntity_Response:
		break;
	case EditorServer::MsgType::EditComponent:
		HandleEditComponentMsg(msg);
		break;
	case EditorServer::MsgType::RequestAssetsFolderPath_Response:
		break;
	default:
		ASSERT(false);
		break;
	}
	
}

void MainPropertySheet::HandleGetSceneXmlResponseMsg(const EditorServer::Msg& msg)
{
	const EditorServer::GetSceneXmlMsg_Response& resp = std::get<EditorServer::GetSceneXmlMsg_Response>(msg.Data);
	m_Doc.load_buffer(resp.xml.c_str(), resp.xml.length());
	if (pugi::xml_node n = m_Doc.child("Scene"))
	{
		m_Scn = n;
	}
	else
	{
		// error
		ASSERT(false);
	}
}

void MainPropertySheet::HandleEditComponentMsg(const EditorServer::Msg& msg)
{
	// patch m_Scn with new component value
	const EditorServer::EditComponentMsg msgData = std::get<EditorServer::EditComponentMsg>(msg.Data);
	pugi::xml_document newComponent;
	newComponent.load_buffer(msgData.newComponentXml.c_str(), msgData.newComponentXml.length());
	pugi::xml_node sceneNode = newComponent.child("Scene");

	pugi::xml_node componentNode = sceneNode.first_child();
	if (pugi::xml_node ents = m_Scn.child("Entities"))
	{
		for (pugi::xml_node child : ents.children())
		{
			if (pugi::xml_attribute entID = child.attribute("u32Val"))
			{
				if (entID.as_uint() == msgData.entity)
				{
					const char* name = componentNode.name();
					child.remove_child(name);
					pugi::xml_node newNode = child.append_copy(componentNode);
					if (EditorClient::RecieveQueueEmpty())
					{
						// Only update the ui when the final message in queue is recieved.
						// - Assumes the messages are recieved in order - need to check if this is legit - probably not. Possibly this packet needs a time stamp 
						// - Messages are constantly sent to the editor, so ignore all but the latest one to prevent unecessary updating of the UI
						//   at a rate that it can't really cope with. It still flickers in an unslightly way, due to entire property 
						//   grid control being reconstructed with each update message. Need to either diff against previous state and update only the property that's changed
						//   or make a new type of editor message that notifies a component has changed witha  finer granularity - WHICH property of the component has changed?
						//  This will be better.
						m_pEntitiesPropertyPage->UpdateComponent(msgData.entity, newNode, name);
					}
				}
			}
		}
	}
}
