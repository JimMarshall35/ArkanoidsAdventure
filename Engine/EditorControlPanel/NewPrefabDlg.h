#pragma once
#include <afxdlgs.h>
#include <vector>
#include "pugixml.hpp"
#include "ComponentInspectorPropertyGrid.h"

class NewPrefabDlg : public CDialog
{
public:
	NewPrefabDlg();
	void SetData(pugi::xml_node n);
	std::string GetData(CString& outName);
	virtual BOOL OnInitDialog() override;
	virtual void DoDataExchange(CDataExchange* pDX) override;

private:
	pugi::xml_node m_ComponentTypes;
	CListBox m_ctlAllComponentTypesListBox;
	CListBox m_ctlNewPrefabComponentsListBox;
	CComponentInspectorPropertyGrid m_ctlComponentInspectorPropGrid;
	CButton m_ctlAddButton;
	CButton m_ctlRemoveButton;
	pugi::xml_document m_Doc;
	pugi::xml_node m_PrefabNode;
	BOOL m_bTransformPosFromCamera = TRUE;
	BOOL m_bTransformRotFromCamera = FALSE;
	CString m_strPrefabName;
private:
	afx_msg void OnAddComponent();
	afx_msg void OnRemoveComponent();
	afx_msg void OnNewPrefabItemChanged();
	afx_msg void OnAllComponentsItemChanged();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};