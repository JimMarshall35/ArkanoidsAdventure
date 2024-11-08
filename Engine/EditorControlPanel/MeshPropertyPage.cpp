#include "pch.h"
#include "MeshPropertyPage.h"
#include "resource.h"
#include "EditorServerMsg.h"
#include "StringHelpers.h"
#include "EditorClient.h"
#include "FilePathHelpers.h"


BEGIN_MESSAGE_MAP(MeshesPropertyPage, CPropertyPage)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LOADED_MESHES_LIST, OnTextureListSelChange)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_CONTEXT_ADD_MESH, &AddMesh)
	ON_COMMAND(ID_CONTEXT_DELETE_MESH, &DeleteTexture)
	ON_COMMAND(ID_CONTEXT_EDIT_MESH, &EditTexture)
END_MESSAGE_MAP()


void MeshesPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOADED_MESHES_LIST, m_ctlLoadedMeshesList);
}

BOOL MeshesPropertyPage::OnInitDialog()
{
	BOOL b = CPropertyPage::OnInitDialog();
	UpdateData(FALSE);
	CRect rect;
	m_ctlLoadedMeshesList.GetClientRect(&rect);
	const int numCols = 3;
	int nColInterval = rect.Width() / numCols;
	//nColInterval *= 2;
	m_ctlLoadedMeshesList.InsertColumn(0, _T("Path"), LVCFMT_LEFT, nColInterval);
	m_ctlLoadedMeshesList.InsertColumn(1, _T("Handle"), LVCFMT_LEFT, nColInterval);
	m_ctlLoadedMeshesList.InsertColumn(2, _T("DataSize"), LVCFMT_LEFT, nColInterval);

	if (m_bSceneRecieved)
	{
		PopulateUIFromScene();
	}
	return b;
}

MeshesPropertyPage::MeshesPropertyPage(GetXMLSceneFn getScn)
	:CPropertyPage(IDD_MESH_PROPERTY_SHEET_PAGE, IDS_MESHESTAB),
	m_fpGetScn(getScn)

{
}

void MeshesPropertyPage::HandleMsgRecieved(const EditorServer::Msg& msg)
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
	case EditorServer::MsgType::UploadMeshFileMsg_Response:
		// notification that scene has been updated with new texture
		PopulateUIFromScene();
		break;
	default:
		break;
	}
}

void MeshesPropertyPage::PopulateUIFromScene()
{
	bool bMalformedData = false;
	m_ctlLoadedMeshesList.DeleteAllItems();
	pugi::xml_node node = m_fpGetScn();
	if (pugi::xml_node texReg = node.child("MeshReg"))
	{
		LVITEM lvi;
		int i = 0;
		for (pugi::xml_node mesh : texReg.child("Meshes").children())
		{
			if (pugi::xml_node handle = mesh.child("Handle"))
			{
				if (pugi::xml_node meshData = mesh.child("PipelineMeshData"))
				{
					const char* name = meshData.child("Name").attribute("str").as_string();
					std::wstring s = s2ws(name);

					lvi.mask = LVIF_IMAGE | LVIF_TEXT;
					lvi.iItem = i;
					lvi.iSubItem = 0;
					lvi.pszText = (LPTSTR)s.c_str();
					m_ctlLoadedMeshesList.InsertItem(&lvi);

					CString cs;

					name = handle.attribute("u64Val").as_string();
					s = s2ws(name);
					lvi.iSubItem = 1;
					lvi.pszText = (LPTSTR)s.c_str();
					m_ctlLoadedMeshesList.SetItem(&lvi);

					s = L"to implement";
					lvi.iSubItem = 2;
					lvi.pszText = (LPTSTR)s.c_str();
					m_ctlLoadedMeshesList.SetItem(&lvi);
				}
			}
			i++;
		}
	}
}

void MeshesPropertyPage::OnTextureListSelChange(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if ((pNMListView->uChanged & LVIF_STATE)
		&& (pNMListView->uNewState & LVIS_SELECTED))
	{
		m_nSelectedMesh = pNMListView->iItem;
		//CString path = m_ctlLoadedMeshesList.GetItemText(m_nSelectedMesh, 0);
		//std::string sPath = ws2s(path);
		//m_ctlTexturePreviewStatic.LoadTextureAsync(sPath.c_str());
	}
	else
	{
		m_nSelectedMesh = -1;
	}
}

void MeshesPropertyPage::OnContextMenu(CWnd* pWnd, CPoint pos)
{
	if (pWnd == &m_ctlLoadedMeshesList)
	{
		CPoint screenPos = pos;
		m_ctlLoadedMeshesList.ScreenToClient(&pos);
		int i = m_ctlLoadedMeshesList.HitTest(pos);
		int rmenu = (i >= 0) ? IDR_MESH_HOVERRED_CTXT_MENU : IDR_MESH_NOT_HOVERRED_CTXT_MENU;

		CMenu menu;
		menu.LoadMenu(rmenu);
		int c = menu.GetMenuItemCount();
		CMenu* popup = menu.GetSubMenu(0);
		popup->TrackPopupMenu(0, screenPos.x, screenPos.y, this, NULL);
	}
}

void MeshesPropertyPage::AddMesh()
{
	EditorClient::GetAssetFolderPathAsync([this](const char* folder) {
		CString sFilter = _T("OBJ Files (*.obj)|*.obj|All Files (*.*)|*.*||");

		std::wstring w = s2ws(folder);

		w += '\\';

		CFileDialog my_file_dialog(TRUE, _T("xxx"), w.c_str(),
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT,
			sFilter, this);

		if (my_file_dialog.DoModal() != IDOK)
			return;

		POSITION pos(my_file_dialog.GetStartPosition());
		while (pos)
		{
			CString filename = my_file_dialog.GetNextPathName(pos);
			std::wstring ext = FilePath::GetFileExtension(filename);
			if (ext != L".obj")
			{
				CString msg;
				msg.Format(L"texture file: '%s'. invalid extension", filename);
				AfxMessageBox(msg, MB_OK);
				continue;
			}
			if (!FilePath::IsFilepathWithinFolder(w.c_str(), filename))
			{
				CString msg;
				msg.Format(L"texture file: '%s'. not within assets folder: '%s'", filename, w.c_str());
				AfxMessageBox(msg, MB_OK);
				continue;
			}
			std::wstring relativePath = FilePath::GetFilepathRelativeToFolder(w.c_str(), filename);
			std::wstring stem = FilePath::GetFileNameWithoutExtension(filename);
			EditorServer::Msg msg;
			msg.Data = EditorServer::UploadMeshFileMsg{
				ws2s(relativePath.c_str())
			};
			msg.Type = EditorServer::MsgType::UploadMeshFileMsg;
			EditorClient::EnqueueToSend(msg);
		}
	});
}

void MeshesPropertyPage::EditTexture()
{
}

void MeshesPropertyPage::DeleteTexture()
{
}
