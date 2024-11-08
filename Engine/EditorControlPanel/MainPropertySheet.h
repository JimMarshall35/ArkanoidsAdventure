#pragma once
#include <afxdlgs.h>
#include "pugixml.hpp"

namespace EditorServer
{
    struct Msg;
}
class CEntitiesPropertyPage;
class TexturesPropertyPage;
class MeshesPropertyPage;

class MainPropertySheet :
    public CPropertySheet
{
public:
    MainPropertySheet();
    ~MainPropertySheet();
    virtual BOOL OnInitDialog() override;
    void HandleMessageRecieved(const EditorServer::Msg& msg);
    void HandleGetSceneXmlResponseMsg(const EditorServer::Msg& msg);
    void HandleEditComponentMsg(const EditorServer::Msg& msg);
    const pugi::xml_document& GetDoc() const { return m_Doc; };
protected:
    CEntitiesPropertyPage* m_pEntitiesPropertyPage = nullptr;
    TexturesPropertyPage* m_pTexturesPropertySheetPage = nullptr;
    MeshesPropertyPage* m_pMeshesPropertyPage = nullptr;
    pugi::xml_document m_Doc;
    pugi::xml_node m_Scn;
    DECLARE_MESSAGE_MAP()
};

