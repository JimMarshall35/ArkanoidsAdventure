#pragma once
#include <afxwin.h>
#include <functional>

typedef std::function<void(const char*)> LineEnteredCallbackFn;

class Console :
    public CRichEditCtrl
{
public:
    void Init(LineEnteredCallbackFn lineEnteredFn);
    void OutputText(const char* text);
    void HandleSelchangedMsg(SELCHANGE* pSelChange);
    void OnEnter();
private:
    LineEnteredCallbackFn m_fpLineEntered;
};

