#pragma once
#include <afxdlgs.h>
#include "MainPropertySheet.h"
#include "Console.h"

class MainDialog : public CDialog
{
public:
	MainDialog();
	virtual BOOL OnInitDialog() override;
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	afx_msg void ConsoleSelectionChanged(NMHDR* pNotifyStruct, LRESULT* result);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSaveSceneAs();
	afx_msg void OnSaveScene();
	void SaveSceneInternal();

	void HandleMessageRecieved(const EditorServer::Msg& msg);

	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	void OnConsoleLineEntered(const char* line);
	
private:
	CString m_strSavePath = _T("");
	UINT_PTR m_RecieveQueueTimer = 0;
	MainPropertySheet m_dlgMainPropSheet;
	Console m_ctlConsole;
	DECLARE_MESSAGE_MAP()
};

