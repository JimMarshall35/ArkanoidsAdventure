
// EditorControlPanelDlg.cpp : implementation file
//

#include "pch.h"
#include <variant>
#include <codecvt>
#include <locale> 
#include <array>
#include <algorithm>
#include <functional>
#include "framework.h"
#include "EditorControlPanel.h"
#include "EntitiesPropertyPage.h"
#include "afxdialogex.h"
#include "EditorClient.h"
#include "StringHelpers.h"
#include "EditorServerMsg.h"
#include "resource.h"
#include "NewPrefabDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CEditorControlPanelDlg dialog



CEntitiesPropertyPage::CEntitiesPropertyPage(GetXMLSceneFn getScn)
	: CPropertyPage(IDD_ENTITIES_PROPERTY_SHEET_PAGE, IDS_ENTITIESTAB),
	m_fpGetScn(getScn)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_psp.dwFlags &= ~PSP_HASHELP;
}

void CEntitiesPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_ENTITIES_TREE, m_ctlEntityTree);
	DDX_Control(pDX, IDC_COMPONENT_INSPECTOR_PROPERTY_GRID, m_ctlComponentInspector);
	DDX_Control(pDX, IDC_SELECTED_COMPONENT_TITLE, m_ctlSelectedComponentTitle);
	DDX_Control(pDX, IDC_PREFAB_LIST, m_ctlPrefabListBox);
	CPropertyPage::DoDataExchange(pDX);
}

void CEntitiesPropertyPage::OnSelChangedTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult)
{
	using namespace pugi;
	static_assert(sizeof(xml_node) == sizeof(DWORD_PTR));
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	TVITEMW item = pNMTreeView->itemNew;
	xml_node propertyNode;
	memcpy(&propertyNode, &item.lParam, sizeof(DWORD_PTR));
	
	if (HTREEITEM parent = m_ctlEntityTree.GetNextItem(item.hItem, TVGN_PARENT))
	{
		uint32_t entity = m_ctlEntityTree.GetItemData(parent);
		m_ctlComponentInspector.OnNewComponentSelected(entity, propertyNode);
		SetSelectedComponentText(propertyNode, entity);
	}
	else
	{
		uint32_t entity = m_ctlEntityTree.GetItemData(item.hItem);
		EditorServer::Msg msg = {
			EditorServer::MsgType::SetEntityGizmo,
			EditorServer::SetEntityGizmoMsg{entity}
		};
		EditorClient::EnqueueToSend(msg);
	}
}


void CEntitiesPropertyPage::Test()
{
	EditorServer::Msg msg;
	msg.Type = EditorServer::MsgType::GetSceneXML;
	msg.Data = EditorServer::GetSceneXmlMsg{};
	EditorClient::EnqueueToSend(msg);
	msg.Type = EditorServer::MsgType::RequestAssetsFolderPath;
	EditorClient::EnqueueToSend(msg);
}

void CEntitiesPropertyPage::HandleMsgRecieved(const EditorServer::Msg& msg)
{
	switch (msg.Type)
	{
	case EditorServer::MsgType::GetSceneXML_Response:
		{
			m_ctlEntityTree.OnNewSceneRecived(m_fpGetScn());
			m_ctlPrefabListBox.OnNewSceneRecieved(m_fpGetScn());
			break;
		}
	case EditorServer::MsgType::NewEntity_Response:
		break;
	case EditorServer::MsgType::EditComponent:
		break;
	default:
		ASSERT(false);
		break;
	}
}

void CEntitiesPropertyPage::UpdateComponent(size_t entity, pugi::xml_node newNode, const char* componentName)
{
	m_ctlEntityTree.UpdateComponent(entity, newNode, componentName);
	m_ctlComponentInspector.UpdateComponent(entity, newNode, componentName);
}

void CEntitiesPropertyPage::SetSelectedComponentText(pugi::xml_node propertyNode, uint32_t entity)
{
	std::wstring ws = s2ws(propertyNode.name());
	CString text;
	text.Format(_T("%s - Entity %i"), ws.c_str(), entity);
	m_ctlSelectedComponentTitle.SetWindowText(text);
}

void CEntitiesPropertyPage::OnDestroy()
{
	EditorClient::DeInitClient();
	CPropertyPage::OnDestroy();
}

void CEntitiesPropertyPage::OnContextMenu(CWnd* pWnd, CPoint pos)
{

	if (pWnd == &m_ctlPrefabListBox)
	{
		m_ctlPrefabListBox.OnContextMenu(pos);
	}
}

BEGIN_MESSAGE_MAP(CEntitiesPropertyPage, CPropertyPage)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_TESTBTN, &Test)
	ON_WM_DESTROY()
	ON_NOTIFY(TVN_SELCHANGED, IDC_ENTITIES_TREE, &OnSelChangedTreeCtrl)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


// CEditorControlPanelDlg message handlers

BOOL CEntitiesPropertyPage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	

	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CEntitiesPropertyPage::OnSysCommand(UINT nID, LPARAM lParam)
{
	CPropertyPage::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CEntitiesPropertyPage::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPropertyPage::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CEntitiesPropertyPage::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BEGIN_MESSAGE_MAP(CPrefabListbox, CListBox)
	ON_COMMAND(ID_ADDNEWPREFAB, &AddPrefab)
	ON_COMMAND(ID_ADDTOPALETTESLOT1, &AddToPalette1)
	ON_COMMAND(ID_ADDTOPALETTESLOT2, &AddToPalette2)
	ON_COMMAND(ID_ADDTOPALETTESLOT3, &AddToPalette3)
	ON_COMMAND(ID_ADDTOPALETTESLOT4, &AddToPalette4)
END_MESSAGE_MAP()

void CPrefabListbox::OnContextMenu(CPoint pos)
{
	

	auto HitItem = [this](const CPoint& pt) -> int
	{
		CRect r;
		for (int i = 0; i < GetCount(); i++)
		{
			GetItemRect(i, &r);
			if (r.PtInRect(pt))
			{
				return i;
			}
		}
		return -1;
	};

	CPoint screen = pos;
	ScreenToClient(&pos);
	CRect r;
	m_nSelectedItemContextMenu = -1;

	m_nSelectedItemContextMenu = HitItem(pos);
	int menuID = m_nSelectedItemContextMenu < 0 ? IDR_PREFAB_NOT_HOVERRED_CTXT_MENU : IDR_PREFAB_HOVERRED_CTXT_MENU;

	CMenu menu;
	menu.LoadMenu(menuID);
	int c = menu.GetMenuItemCount();
	CMenu* popup = menu.GetSubMenu(0);
	popup->TrackPopupMenu(0, screen.x, screen.y, this, NULL);
}

void CPrefabListbox::OnNewSceneRecieved(const pugi::xml_node node)
{
	using namespace pugi;
	static_assert(sizeof(DWORD_PTR) == sizeof(xml_node));
	m_Types = node.child("ComponentTypes");

	if (xml_node prefabs = node.child("Prefabs"))
	{
		ResetContent();
		for (xml_node prefab : prefabs)
		{
			std::wstring ws = s2ws(prefab.name());
			InsertString(-1, ws.c_str());
			DWORD_PTR p;
			memcpy(&p, &prefab, sizeof(DWORD_PTR));;
			SetItemData(GetCount() - 1, p);
		}
	}
}

void CPrefabListbox::AddPrefab()
{
	NewPrefabDlg dlg;
	dlg.SetData(m_Types);
	if (dlg.DoModal() == IDOK)
	{
		CString name;
		std::string data = dlg.GetData(name);
		if (!PrefabNameTaken(name))
		{
			m_ListboxEntryXMLs.push_back(data);

			int i = InsertString(-1, name);
			SetItemData(i, (DWORD_PTR)&m_ListboxEntryXMLs.back());
		}
		else
		{
			CString s;
			s.Format(L"Prefab name '%s' taken.", name);
			AfxMessageBox(s);
		}
	}
}

void CPrefabListbox::AddToPalette1()
{
	AddToPalette(0);
}

void CPrefabListbox::AddToPalette2()
{
	AddToPalette(1);
}

void CPrefabListbox::AddToPalette3()
{
	AddToPalette(2);
}

void CPrefabListbox::AddToPalette4()
{
	AddToPalette(3);
}

bool CPrefabListbox::PrefabNameTaken(const CString& name)
{
	for (int i = 0; i < GetCount(); i++)
	{
		CString lbString;
		GetText(i, lbString);
		if (lbString == name)
		{
			return true;
		}
	}
	return false;
}

void CPrefabListbox::AddToPalette(char slot)
{
	CString s;
	GetText(m_nSelectedItemContextMenu, s);
	std::string name = ws2s(s);
	std::string* xml = (std::string*)GetItemData(m_nSelectedItemContextMenu);

	EditorClient::SetPrefabPaletteAsync(name.c_str(), xml->c_str(), slot, [](const char* name, char slot, bool bSuccess)
	{
		printf("fjidj");
	});
}
