#pragma once
#include <afxdlgs.h>
#include "pugixml.hpp"

namespace EditorServer
{
    struct Msg;
}
class CEntitiesPropertyPage;
class TexturesPropertyPage;


class MainPropertySheet :
    public CPropertySheet
{
public:
    MainPropertySheet();
    ~MainPropertySheet();
    virtual BOOL OnInitDialog() override;
protected:
    void HandleMessageRecieved(const EditorServer::Msg& msg);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    UINT_PTR m_RecieveQueueTimer = 0;
    CEntitiesPropertyPage* m_pEntitiesPropertyPage = nullptr;
    TexturesPropertyPage* m_pAssetsPropertyPage = nullptr;
    pugi::xml_document m_Doc;
    pugi::xml_node m_Scn;
    DECLARE_MESSAGE_MAP()
};

