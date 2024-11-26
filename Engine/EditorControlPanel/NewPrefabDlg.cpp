#include "pch.h"
#include "NewPrefabDlg.h"
#include "Resource.h"
#include "StringHelpers.h"
#include <sstream>

BEGIN_MESSAGE_MAP(NewPrefabDlg, CDialog)
	ON_BN_CLICKED(IDC_ADD_COMPONENT, &OnAddComponent)
	ON_BN_CLICKED(IDC_REMOVE_COMPONENT, &OnRemoveComponent)
	//ON_NOTIFY(LBN_SELCHANGE, IDC_NEW_PREFAB_COMPONENTS, &OnNewPrefabItemChanged)  // DOESN'T WORK
	//ON_NOTIFY(LBN_SELCHANGE, IDC_ALL_COMPONENT_TYPES, &OnAllComponentsItemChanged) // DOESN'T WORK

	ON_LBN_SELCHANGE(IDC_NEW_PREFAB_COMPONENTS, &NewPrefabDlg::OnNewPrefabItemChanged)
	ON_LBN_SELCHANGE(IDC_ALL_COMPONENT_TYPES, &NewPrefabDlg::OnAllComponentsItemChanged)
	ON_BN_CLICKED(IDOK, &NewPrefabDlg::OnBnClickedOk)
END_MESSAGE_MAP()

NewPrefabDlg::NewPrefabDlg()
	:CDialog(IDD_NEW_PREFAB),
	m_ctlComponentInspectorPropGrid(false)
{
	m_PrefabNode = m_Doc.append_child("Prefab");
}

void NewPrefabDlg::SetData(pugi::xml_node n)
{
	m_ComponentTypes = n;
	
}

std::string NewPrefabDlg::GetData(CString& outName)
{
	//UpdateData(FALSE);
	outName = m_strPrefabName;
	std::stringstream ss;
	m_Doc.save(ss);

	return ss.str();
}

BOOL NewPrefabDlg::OnInitDialog()
{
	UpdateData(FALSE);

	m_ctlAllComponentTypesListBox.ResetContent();
	for (pugi::xml_node cType : m_ComponentTypes)
	{
		auto ws = s2ws(cType.name());
		m_ctlAllComponentTypesListBox.AddString(ws.c_str());
	}
	return 0;
}

void NewPrefabDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_TRANSFORM_POS_FROM_CAMERA, m_bTransformPosFromCamera);
	DDX_Check(pDX, IDC_TRANSFORM_ROT_FROM_CAMERA, m_bTransformRotFromCamera);
	DDX_Control(pDX, IDC_ALL_COMPONENT_TYPES, m_ctlAllComponentTypesListBox);
	DDX_Control(pDX, IDC_NEW_PREFAB_COMPONENTS, m_ctlNewPrefabComponentsListBox);
	DDX_Control(pDX, IDC_COMPONENT_PROPERTY_GRID, m_ctlComponentInspectorPropGrid);
	DDX_Control(pDX, IDC_ADD_COMPONENT, m_ctlAddButton);
	DDX_Control(pDX, IDC_REMOVE_COMPONENT, m_ctlRemoveButton);
	DDX_Text(pDX, IDC_NEW_PREFAB_NAME, m_strPrefabName);
}

static bool ComponentAlreadyAdded(CListBox& lb, CString component)
{
	CString s;
	for (int i = 0; i < lb.GetCount(); i++)
	{
		lb.GetText(i, s);
		if (component == s)
		{
			return true;
		}
	}
	return false;
}

void NewPrefabDlg::OnAddComponent()
{
	if (m_ctlAllComponentTypesListBox.GetCurSel() < 0)
	{
		return;
	}
	CString name;
	m_ctlAllComponentTypesListBox.GetText(m_ctlAllComponentTypesListBox.GetCurSel(), name);

	if (!ComponentAlreadyAdded(m_ctlNewPrefabComponentsListBox, name))
	{
		std::string s = ws2s(name);
		pugi::xml_node n = m_ComponentTypes.child(s.c_str());
		ASSERT(n);
		m_PrefabNode.append_copy(n);
		m_ctlNewPrefabComponentsListBox.AddString(name);
	}
}

void NewPrefabDlg::OnNewPrefabItemChanged()
{
	m_ctlAddButton.EnableWindow(FALSE);
	m_ctlRemoveButton.EnableWindow(TRUE);
	m_ctlAllComponentTypesListBox.SetCurSel(-1);
	CString ws;
	m_ctlNewPrefabComponentsListBox.GetText(m_ctlNewPrefabComponentsListBox.GetCurSel(), ws);
	std::string s = ws2s(ws);
	pugi::xml_node child =  m_PrefabNode.child(s.c_str());
	m_ctlComponentInspectorPropGrid.OnNewComponentSelected(-1, child);
}

void NewPrefabDlg::OnAllComponentsItemChanged()
{
	m_ctlAddButton.EnableWindow(TRUE);
	m_ctlRemoveButton.EnableWindow(FALSE);
	m_ctlNewPrefabComponentsListBox.SetCurSel(-1);
}

void NewPrefabDlg::OnRemoveComponent()
{
	if (m_ctlNewPrefabComponentsListBox.GetCurSel() < 0)
	{
		return;
	}
	
	CString name;
	m_ctlNewPrefabComponentsListBox.GetText(m_ctlNewPrefabComponentsListBox.GetCurSel(), name);
	if (ComponentAlreadyAdded(m_ctlNewPrefabComponentsListBox, name))
	{
		std::string s = ws2s(name);
		pugi::xml_node n = m_PrefabNode.child(s.c_str());
		ASSERT(n);
		m_PrefabNode.remove_child(s.c_str());
		m_ctlNewPrefabComponentsListBox.DeleteString(m_ctlNewPrefabComponentsListBox.GetCurSel());
	}
	
}


void NewPrefabDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CDialog::OnOK();
}
