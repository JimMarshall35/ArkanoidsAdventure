#include "pch.h"
#include "ExportDlg.h"
#include <vector>
#include "Exporters.h"

BEGIN_MESSAGE_MAP(ExportDlg, CDialog)
	ON_BN_CLICKED(IDC_EXPORT_PATH_BTN, OnExportPathBtnClick)
	ON_CBN_SELCHANGE(IDC_EXPORTER_COMBO, &OnComboSelchange)

END_MESSAGE_MAP()

ExportDlg::ExportDlg()
	:CDialog(IDD)
{

}

void ExportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EXPORT_PATH_SELECTED_LABEL, m_ctlPathLabel);
	DDX_Control(pDX, IDC_EXPORTER_COMBO, m_ctlExportersCombo);
}

BOOL ExportDlg::OnInitDialog()
{
	BOOL b = CDialog::OnInitDialog();
	std::vector<CString> exporterNames = GetExporters();
	ASSERT(exporterNames.size() > 0);
	for (int i = 0; i < exporterNames.size(); i++)
	{
		m_ctlExportersCombo.AddString(exporterNames[i]);
		m_ctlExportersCombo.SetItemData(i, i);
	}
	m_ctlExportersCombo.SetCurSel(0);
	return b;
}

int ExportDlg::GetSelectedExporter()
{
	return m_nSelectedExporter;
}

void ExportDlg::OnComboSelchange()
{
	int i = m_ctlExportersCombo.GetCurSel();
	m_nSelectedExporter = m_ctlExportersCombo.GetItemData(i);
}

void ExportDlg::OnExportPathBtnClick()
{
	CFolderPickerDialog dlg;
	if (dlg.DoModal() == IDOK)
	{
		m_ctlPathLabel = dlg.GetPathName();
		UpdateData(FALSE);
	}
}
