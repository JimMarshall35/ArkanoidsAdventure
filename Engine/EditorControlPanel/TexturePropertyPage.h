#pragma once
#include <afxdlgs.h>
#include "CommonPropertyPageDefines.h"
#include "TexturePreviewStatic.h"

namespace EditorServer
{
    struct Msg;
}

class TexturesPropertyPage :
    public CPropertyPage
{
public:
    virtual void DoDataExchange(CDataExchange* pDX) override;
    virtual BOOL OnInitDialog() override;

    TexturesPropertyPage(GetXMLSceneFn getScn);
    void HandleMsgRecieved(const EditorServer::Msg& msg);
private:
    void PopulateUIFromScene();
    afx_msg void OnTextureListSelChange(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
    afx_msg void AddTexture();
    afx_msg void EditTexture();
    afx_msg void DeleteTexture();

private:
    CListCtrl m_ctlLoadedTexturesList;
    TexturePreviewStatic m_ctlTexturePreviewStatic;
    GetXMLSceneFn m_fpGetScn;
    bool m_bSceneRecieved = false;
    int m_nSelectedTexture = -1;
    
    DECLARE_MESSAGE_MAP()
};

