#pragma once
#include <afxdlgs.h>
#include "CommonPropertyPageDefines.h"

namespace EditorServer
{
    struct Msg;
}

class TexturesPropertyPage :
    public CPropertyPage
{
public:
    TexturesPropertyPage(GetXMLSceneFn getScn);
    void HandleMsgRecieved(const EditorServer::Msg& msg);
    GetXMLSceneFn m_fpGetScn;
};

