
// EditorControlPanelDlg.h : header file
//

#pragma once
#include "ComponentInspectorPropertyGrid.h"
#include "EntityTreeCtrl.h"
#include "CommonPropertyPageDefines.h"
#include <list>

namespace EditorServer
{
	struct Msg;
}

class CPrefabListbox : public CListBox
{
public:
	void OnContextMenu(CPoint pos);
	void OnNewSceneRecieved(const pugi::xml_node node);
protected:
	afx_msg void AddPrefab();
	afx_msg void AddToPalette1();
	afx_msg void AddToPalette2();
	afx_msg void AddToPalette3();
	afx_msg void AddToPalette4();

	bool PrefabNameTaken(const CString& name);
	void AddToPalette(char slot);
private:
	pugi::xml_node m_Types;
	int m_nSelectedItemContextMenu = -1;
	std::list<std::string> m_ListboxEntryXMLs;
	DECLARE_MESSAGE_MAP()

};

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
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);

// Implementation
protected:
	HICON m_hIcon;
	pugi::xml_document m_Doc;
	CEntityTreeCtrl m_ctlEntityTree;
	CComponentInspectorPropertyGrid m_ctlComponentInspector;
	CStatic m_ctlSelectedComponentTitle;
	GetXMLSceneFn m_fpGetScn;
	CPrefabListbox m_ctlPrefabListBox;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void Test();
	void SetSelectedComponentText(pugi::xml_node propertyNode, uint32_t entity);
	DECLARE_MESSAGE_MAP()
};
