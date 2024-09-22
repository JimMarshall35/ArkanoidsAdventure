#include "pch.h"
#include "MainDialog.h"
#include "resource.h"
#include "EditorClient.h"
#include "EditorServerMsg.h"

#define POLL_RECIEVE_QUEUE_TIMER_EVENT_ID 420

BEGIN_MESSAGE_MAP(MainDialog, CDialog)
    ON_NOTIFY(EN_SELCHANGE, IDC_CONSOLE, &ConsoleSelectionChanged)
    ON_WM_TIMER()
END_MESSAGE_MAP()

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

    m_ctlConsole.Init([this](const char* line) { OnConsoleLineEntered(line); });
    m_RecieveQueueTimer = SetTimer(POLL_RECIEVE_QUEUE_TIMER_EVENT_ID, 20, NULL);

    return b;
}

void MainDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CONSOLE, m_ctlConsole);
}

void MainDialog::ConsoleSelectionChanged(NMHDR* pNotifyStruct, LRESULT* result)
{
    SELCHANGE*  pSelChange = (SELCHANGE*)pNotifyStruct;
    m_ctlConsole.HandleSelchangedMsg(pSelChange);  // could do this with message reflection: https://learn.microsoft.com/en-us/cpp/mfc/tn062-message-reflection-for-windows-controls?view=msvc-170

}

void MainDialog::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == POLL_RECIEVE_QUEUE_TIMER_EVENT_ID)
    {
        while (!EditorClient::RecieveQueueEmpty())
        {
            EditorServer::Msg msg;
            VERIFY(!EditorClient::PopRecieveQueue(msg));
            HandleMessageRecieved(msg);
        }
    }
    CDialog::OnTimer(nIDEvent);
}

void OutputLogMsgToConsole(const EditorServer::EngineLogMsg& msg, Console& console)
{
    // TODO: use EM_SETCHARFORMAT message to set text colour depending on severity, ect
    std::string s = "";
    switch (msg.src)
    {
    case EditorServer::LogSource::Frontend:
        s += "[Frontend]";
        break;
    case EditorServer::LogSource::Backend:
        s += "[Backend]";
        break;
    }
    switch (msg.severity)
    {
    case EditorServer::LogSeverity::Error:
        s += "[Error] ";
        break;
    case EditorServer::LogSeverity::Info:
        s += "[Info] ";
        break;
    case EditorServer::LogSeverity::Warning:
        s += "[Warning] ";
        break;
    case EditorServer::LogSeverity::Unknown:
        s += "[Unknown] ";
        break;
    }
    s += msg.msg;
    console.OutputText(s.c_str());
}

void MainDialog::HandleMessageRecieved(const EditorServer::Msg& msg)
{
    switch (msg.Type)
    {
    case EditorServer::MsgType::EngineLog:
        OutputLogMsgToConsole(std::get<EditorServer::EngineLogMsg>(msg.Data), m_ctlConsole);
        break;
    }
    m_dlgMainPropSheet.HandleMessageRecieved(msg);
}

BOOL MainDialog::PreTranslateMessage(MSG* pMsg)
{
    if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN))
    {
        if (pMsg->hwnd == m_ctlConsole.m_hWnd)
        {
            m_ctlConsole.OnEnter();
        }
        else return TRUE;
    }
    if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_F5))
    {
        m_ctlConsole.OutputText("hello world");
    }

    return CDialog::PreTranslateMessage(pMsg);
}

void MainDialog::OnConsoleLineEntered(const char* line)
{
    EditorServer::Msg msg;
    msg.Type = EditorServer::MsgType::EngineCmd;
    msg.Data = EditorServer::EngineCmdMsg();
    std::get<EditorServer::EngineCmdMsg>(msg.Data).cmd = line;
    EditorClient::EnqueueToSend(msg);
}


