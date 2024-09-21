#pragma once
#include <afxwin.h>
class Console :
    public CRichEditCtrl
{
public:
    void Init();
    void OutputText(const char* text);
    

};

