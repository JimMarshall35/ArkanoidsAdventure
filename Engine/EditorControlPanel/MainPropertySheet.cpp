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
	m_pAssetsPropertyPage = new TexturesPropertyPage(getScn);
	AddPage(m_pEntitiesPropertyPage);
	AddPage(m_pAssetsPropertyPage);
}

MainPropertySheet::~MainPropertySheet()
{
	if (m_pAssetsPropertyPage)
	{
		delete m_pAssetsPropertyPage;
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
		}
		break;
	}
	case EditorServer::MsgType::NewEntity_Response:
		break;
	default:
		break;
	}
	m_pEntitiesPropertyPage->HandleMsgRecieved(msg);
}
