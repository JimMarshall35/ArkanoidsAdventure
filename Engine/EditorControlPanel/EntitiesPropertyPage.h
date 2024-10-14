
// EditorControlPanelDlg.h : header file
//

#pragma once
#include "ComponentInspectorPropertyGrid.h"
#include "EntityTreeCtrl.h"
#include "CommonPropertyPageDefines.h"

namespace EditorServer
{
	struct Msg;
}


// CEditorControlPanelDlg dialog
class CEntitiesPropertyPage : public CPropertyPage
{
// Construction
public:
	CEntitiesPropertyPage(GetXMLSceneFn getScn);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ENTITIES_PROPERTY_SHEET_PAGE};
#endif
	void HandleMsgRecieved(const EditorServer::Msg& msg);
	void UpdateComponent(size_t entity, pugi::xml_node newNode, const char* componentName);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	afx_msg void OnSelChangedTreeCtrl(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();

// Implementation
protected:
	HICON m_hIcon;
	pugi::xml_document m_Doc;
	CEntityTreeCtrl m_ctlEntityTree;
	CComponentInspectorPropertyGrid m_ctlComponentInspector;
	CStatic m_ctlSelectedComponentTitle;
	GetXMLSceneFn m_fpGetScn;
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void Test();
	void SetSelectedComponentText(pugi::xml_node propertyNode, uint32_t entity);
	DECLARE_MESSAGE_MAP()
};
