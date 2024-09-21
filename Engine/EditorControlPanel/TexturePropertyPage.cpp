#include "pch.h"
#include "TexturePropertyPage.h"
#include "resource.h"
#include "EditorServerMsg.h"

TexturesPropertyPage::TexturesPropertyPage(GetXMLSceneFn getScn)
	:CPropertyPage(IDD_TEXTURE_PROPERTY_SHEET_PAGE, IDS_TEXTURESTAB),
	m_fpGetScn(getScn)
{
	m_psp.dwFlags &= ~PSP_HASHELP;

}

void TexturesPropertyPage::HandleMsgRecieved(const EditorServer::Msg& msg)
{
	switch (msg.Type)
	{
	case EditorServer::MsgType::GetSceneXML_Response:
	{
		pugi::xml_node node = m_fpGetScn();

		break;
	}
	case EditorServer::MsgType::NewEntity_Response:
		break;
	default:
		break;
	}
}
