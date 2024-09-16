
// EditorControlPanelDlg.h : header file
//

#pragma once
#include "ComponentInspectorPropertyGrid.h"
#include "EntityTreeCtrl.h"

namespace EditorServer
{
	struct Msg;
}


// CEditorControlPanelDlg dialog
class CEditorControlPanelDlg : public CDialogEx
{
// Construction
public:
	CEditorControlPanelDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EDITORCONTROLPANEL_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	afx_msg void OnSelChangedTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();

// Implementation
protected:
	HICON m_hIcon;
	UINT_PTR m_RecieveQueueTimer = 0;
	pugi::xml_document m_Doc;
	CEntityTreeCtrl m_ctlEntityTree;
	CComponentInspectorPropertyGrid m_ctlComponentInspector;
	CStatic m_ctlSelectedComponentTitle;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void Test();
	void HandleMsgRecieved(const EditorServer::Msg& msg);
	void SetSelectedComponentText(pugi::xml_node propertyNode, uint32_t entity);
	DECLARE_MESSAGE_MAP()
};