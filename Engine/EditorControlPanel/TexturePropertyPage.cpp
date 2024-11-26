#include "pch.h"
#include "TexturePropertyPage.h"
#include "resource.h"
#include "EditorServerMsg.h"
#include "StringHelpers.h"
#include "PickTextureFilesToUploadDlg.h"
#include "EditorClient.h"

BEGIN_MESSAGE_MAP(TexturesPropertyPage, CPropertyPage)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LOADED_TEXTURES_LIST, OnTextureListSelChange)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_CONTEXT_ADDTEXTURE, &AddTexture)
	ON_COMMAND(ID_CONTEXT_DELETETEXTURE, &DeleteTexture)
	ON_COMMAND(ID_CONTEXT_EDITTEXTURE, &EditTexture)
END_MESSAGE_MAP()


void TexturesPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOADED_TEXTURES_LIST, m_ctlLoadedTexturesList);
	DDX_Control(pDX, IDC_TEXTURE_PREVIEW_STATIC, m_ctlTexturePreviewStatic);
}

BOOL TexturesPropertyPage::OnInitDialog()
{
	BOOL b = CPropertyPage::OnInitDialog();
	UpdateData(FALSE);
	CRect rect;
	m_ctlLoadedTexturesList.GetClientRect(&rect);
	int nColInterval = rect.Width() / 11;
	nColInterval *= 2;
	m_ctlLoadedTexturesList.InsertColumn(0, _T("Path"), LVCFMT_LEFT, nColInterval);
	m_ctlLoadedTexturesList.InsertColumn(1, _T("Name"), LVCFMT_LEFT, nColInterval);
	m_ctlLoadedTexturesList.InsertColumn(2, _T("Size"), LVCFMT_LEFT, nColInterval);
	m_ctlLoadedTexturesList.InsertColumn(3, _T("Format"), LVCFMT_LEFT, nColInterval);
	m_ctlLoadedTexturesList.InsertColumn(4, _T("RowAlignment"), LVCFMT_LEFT, nColInterval);
	m_ctlLoadedTexturesList.InsertColumn(5, _T("MinifyFiltering"), LVCFMT_LEFT, nColInterval);
	m_ctlLoadedTexturesList.InsertColumn(6, _T("MagnifyFiltering"), LVCFMT_LEFT, nColInterval);
	m_ctlLoadedTexturesList.InsertColumn(7, _T("TextureClampS"), LVCFMT_LEFT, nColInterval);
	m_ctlLoadedTexturesList.InsertColumn(8, _T("TextureClampT"), LVCFMT_LEFT, nColInterval);
	m_ctlLoadedTexturesList.InsertColumn(9, _T("bGenerateMipMaps"), LVCFMT_LEFT, nColInterval);
	m_ctlLoadedTexturesList.InsertColumn(10, _T("DataSize"), LVCFMT_LEFT, nColInterval);
	m_ctlLoadedTexturesList.InsertColumn(11, _T("Handle"), LVCFMT_LEFT, nColInterval);

	if (m_bSceneRecieved)
	{
		PopulateUIFromScene();
	}
	return b;
}

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
		if (::IsWindow(m_hWnd))
		{
			// only populate if window is initialised - it won't be until the textures tab is selected
			PopulateUIFromScene();
			return;
		}
		m_bSceneRecieved = true;
		break;
	}
	case EditorServer::MsgType::NewEntity_Response:
		break;
	case EditorServer::MsgType::UploadTextureFile_Response:
		// notification that scene has been updated with new texture
		PopulateUIFromScene();
		break;
	default:
		break;
	}
}

void TexturesPropertyPage::PopulateUIFromScene()
{
	m_ctlLoadedTexturesList.DeleteAllItems();
	pugi::xml_node node = m_fpGetScn();
	if (pugi::xml_node texReg = node.child("TextureReg"))
	{
		int i = 0;
		LVITEM lvi;
		CString strItem;

		for (pugi::xml_node pair : texReg.child("Textures").children())
		{
			pugi::xml_node handle = pair.child("Handle");
			pugi::xml_node texData = pair.child("TextureData");
			const char* name = texData.child("Path").attribute("str").as_string();
			std::wstring s = s2ws(name);

			//m_ctlLoadedTexturesList.InsertItem(i++, s.c_str());
			lvi.mask = LVIF_IMAGE | LVIF_TEXT;
			lvi.iItem = i;
			lvi.iSubItem = 0;
			lvi.pszText = (LPTSTR)s.c_str();
			m_ctlLoadedTexturesList.InsertItem(&lvi);


			name = texData.child("Name").attribute("str").as_string();
			s = s2ws(name);
			strItem.Format(_T("%d"), 11 * i);
			lvi.iSubItem = 1;
			lvi.pszText = (LPTSTR)s.c_str();
			m_ctlLoadedTexturesList.SetItem(&lvi);

			unsigned long long int w = texData.child("WidthPx").attribute("u64Val").as_ullong();
			unsigned long long int h = texData.child("HeightPx").attribute("u64Val").as_ullong();
			strItem.Format(_T("%llux%llu"), w, h);
			lvi.iSubItem = 2;
			lvi.pszText = (LPTSTR)(LPCTSTR)(strItem);
			m_ctlLoadedTexturesList.SetItem(&lvi);

			unsigned int u32; unsigned long long int u64;
#define U32SubItem(Name, num) u32 = texData.child( Name ).attribute("u32Val").as_uint();\
				strItem.Format(_T("%lu"), u32);\
				lvi.iSubItem = num;\
				lvi.pszText = (LPTSTR)(LPCTSTR)(strItem);\
				m_ctlLoadedTexturesList.SetItem(&lvi);

#define U64SubItem(Name, num) u32 = texData.child( Name ).attribute("u64Val").as_uint();\
				strItem.Format(_T("%lu"), u32);\
				lvi.iSubItem = num;\
				lvi.pszText = (LPTSTR)(LPCTSTR)(strItem);\
				m_ctlLoadedTexturesList.SetItem(&lvi);

			U32SubItem("Format", 3)
			U32SubItem("RowAlignment", 4)
			U32SubItem("MinifyFiltering", 5)
			U32SubItem("MagnifyFiltering", 6)
			U32SubItem("TextureClampS", 7)
			U32SubItem("TextureClampT", 8)
			U32SubItem("bGenerateMipMaps", 9)
			U64SubItem("DataSize", 10)
			
			u32 = handle.attribute("u64Val").as_uint();
			strItem.Format(_T("%lu"), u32);
			lvi.iSubItem = 11;
			lvi.pszText = (LPTSTR)(LPCTSTR)(strItem); 
			m_ctlLoadedTexturesList.SetItem(&lvi);
		}
	}
	else
	{
		ASSERT(false); // TODO: log error
	}
}

void TexturesPropertyPage::OnTextureListSelChange(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if ((pNMListView->uChanged & LVIF_STATE)
		&& (pNMListView->uNewState & LVIS_SELECTED))
	{
		// do stuff...
		m_nSelectedTexture = pNMListView->iItem;
		CString path = m_ctlLoadedTexturesList.GetItemText(m_nSelectedTexture, 0);
		std::string sPath = ws2s(path);
		m_ctlTexturePreviewStatic.LoadTextureAsync(sPath.c_str());
	}
	else
	{
		m_nSelectedTexture = -1;
	}
}

void TexturesPropertyPage::OnContextMenu(CWnd* pWnd, CPoint pos)
{
	if (pWnd == &m_ctlLoadedTexturesList)
	{
		CPoint screenPos = pos;
		m_ctlLoadedTexturesList.ScreenToClient(&pos);
		int i = m_ctlLoadedTexturesList.HitTest(pos);
		int rmenu = (i >= 0) ? IDR_TEXTURE_HOVERRED_CTXT_MENU : IDR_TEXTURE_NOT_HOVERRED_CTXT_MENU;

		CMenu menu;
		menu.LoadMenu(rmenu);
		int c = menu.GetMenuItemCount();
		CMenu* popup = menu.GetSubMenu(0);
		popup->TrackPopupMenu(0, screenPos.x, screenPos.y, this, NULL);
	}
}

void TexturesPropertyPage::AddTexture()
{
	PickTextureFilesToUploadDlg dlg;

	if (dlg.DoModal() == IDOK)
	{
		const std::unordered_map<std::wstring, EditorServer::UploadTextureFileMsg>&  data = dlg.GetData();
		for (const std::pair<std::wstring, EditorServer::UploadTextureFileMsg>& d : data)
		{
			const EditorServer::UploadTextureFileMsg& msgData = d.second;
			//printf("dfjiidf");
			EditorServer::Msg msg;
			msg.Data = msgData;
			msg.Type = EditorServer::MsgType::UploadTextureFile;
			EditorClient::EnqueueToSend(msg);
		}
	}
}

void TexturesPropertyPage::EditTexture()
{
}

void TexturesPropertyPage::DeleteTexture()
{
}
