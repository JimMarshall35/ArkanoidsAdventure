
// EditorControlPanelDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "EditorControlPanel.h"
#include "EditorControlPanelDlg.h"
#include "afxdialogex.h"
#include "EditorClient.h"
#include "StringHelpers.h"
#include "EditorServerMsg.h"
#include "resource.h"
#include <variant>
#include <codecvt>
#include <locale> 
#include <array>
#include <algorithm>
#include <functional>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define POLL_RECIEVE_QUEUE_TIMER_EVENT_ID 420
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CEditorControlPanelDlg dialog



CEditorControlPanelDlg::CEditorControlPanelDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EDITORCONTROLPANEL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CEditorControlPanelDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_ENTITIES_TREE, m_ctlEntityTree);
	DDX_Control(pDX, IDC_COMPONENT_INSPECTOR_PROPERTY_GRID, m_ctlComponentInspector);
	DDX_Control(pDX, IDC_SELECTED_COMPONENT_TITLE, m_ctlSelectedComponentTitle);
	CDialogEx::DoDataExchange(pDX);
}

void CEditorControlPanelDlg::OnSelChangedTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult)
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
}


void CEditorControlPanelDlg::Test()
{
	EditorServer::Msg msg;
	msg.Type = EditorServer::MsgType::GetSceneXML;
	msg.Data = EditorServer::GetSceneXmlMsg{};
	EditorClient::EnqueueToSend(msg);
}

void CEditorControlPanelDlg::HandleMsgRecieved(const EditorServer::Msg& msg)
{
	switch (msg.Type)
	{
	case EditorServer::MsgType::GetSceneXML_Response:
		{
			const EditorServer::GetSceneXmlMsg_Response& resp = std::get<EditorServer::GetSceneXmlMsg_Response>(msg.Data);
			m_Doc.load_buffer(resp.xml.c_str(), resp.xml.length());
			m_ctlEntityTree.OnNewSceneRecived(m_Doc);
			break;
		}
	case EditorServer::MsgType::NewEntity_Response:
		break;
	default:
		break;
	}
}

void CEditorControlPanelDlg::SetSelectedComponentText(pugi::xml_node propertyNode, uint32_t entity)
{
	std::wstring ws = s2ws(propertyNode.name());
	CString text;
	text.Format(_T("%s - Entity %i"), ws.c_str(), entity);
	m_ctlSelectedComponentTitle.SetWindowText(text);
}

void CEditorControlPanelDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == POLL_RECIEVE_QUEUE_TIMER_EVENT_ID)
	{
		while (!EditorClient::RecieveQueueEmpty())
		{
			EditorServer::Msg msg;
			VERIFY(!EditorClient::PopRecieveQueue(msg));
			HandleMsgRecieved(msg);
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}

void CEditorControlPanelDlg::OnDestroy()
{
	EditorClient::DeInitClient();
	CDialogEx::OnDestroy();
}

BEGIN_MESSAGE_MAP(CEditorControlPanelDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_TESTBTN, &Test)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_NOTIFY(TVN_SELCHANGED, IDC_ENTITIES_TREE, &OnSelChangedTreeCtrl)

END_MESSAGE_MAP()


// CEditorControlPanelDlg message handlers

BOOL CEditorControlPanelDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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
	EditorClient::InitClient();
	m_RecieveQueueTimer = SetTimer(POLL_RECIEVE_QUEUE_TIMER_EVENT_ID, 20, NULL);

	CMenu menu;
	menu.LoadMenu(IDR_MAIN_DIALOG_MENU);
	SetMenu(&menu);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CEditorControlPanelDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CEditorControlPanelDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CEditorControlPanelDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




