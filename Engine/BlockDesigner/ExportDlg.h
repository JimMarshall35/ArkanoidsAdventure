#pragma once
#include <afxdialogex.h>
#include "resource.h"
#include <memory>

class IExporter;

class ExportDlg :
    public CDialog
{
public:
    ExportDlg();
    virtual void DoDataExchange(CDataExchange* pDX) override;
    virtual BOOL OnInitDialog() override;
    CString GetFolderPath() { return m_ctlPathLabel; }
    int GetSelectedExporter();
    enum {IDD = IDD_EXPORT_FIRST_STAGE};
protected:
    afx_msg void OnComboSelchange();
    afx_msg void OnExportPathBtnClick();
protected:
    CString m_ctlPathLabel;
    CComboBox m_ctlExportersCombo;
    int m_nSelectedExporter = 0;
protected:
    DECLARE_MESSAGE_MAP()

};
