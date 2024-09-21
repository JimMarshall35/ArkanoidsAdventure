#include "pch.h"
#include "MainDialog.h"
#include "resource.h"

MainDialog::MainDialog()
    :CDialog(IDD_MAIN)
{
}

BOOL MainDialog::OnInitDialog()
{
    BOOL b = CDialog::OnInitDialog();

    CMenu menu;
    menu.LoadMenu(IDR_MAIN_DIALOG_MENU);
    SetMenu(&menu);

    m_dlgMainPropSheet.Create(this, WS_CHILD | WS_VISIBLE, 0);
    m_dlgMainPropSheet.ModifyStyleEx(0, WS_EX_CONTROLPARENT);
    m_dlgMainPropSheet.ModifyStyle(0, WS_TABSTOP);
    CRect rcSheet;
    GetDlgItem(IDC_MAINPROPSHEET)->GetWindowRect(&rcSheet);
    ScreenToClient(&rcSheet);
    m_dlgMainPropSheet.SetWindowPos(NULL, rcSheet.left - 7, rcSheet.top - 7, 0, 0,
        SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);

    return b;
}
