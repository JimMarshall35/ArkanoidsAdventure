#pragma once
#include <afxdlgs.h>
#include "CommonPropertyPageDefines.h"
namespace EditorServer
{
    struct Msg;
}

class MeshesPropertyPage :
    public CPropertyPage
{
public:
    virtual void DoDataExchange(CDataExchange* pDX) override;
    virtual BOOL OnInitDialog() override;

    MeshesPropertyPage(GetXMLSceneFn getScn);
    void HandleMsgRecieved(const EditorServer::Msg& msg);
private:
    void PopulateUIFromScene();
    afx_msg void OnTextureListSelChange(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);
    afx_msg void AddMesh();
    afx_msg void EditTexture();
    afx_msg void DeleteTexture();

private:
    CListCtrl m_ctlLoadedMeshesList;
    GetXMLSceneFn m_fpGetScn;
    bool m_bSceneRecieved = false;
    int m_nSelectedMesh = -1;

    DECLARE_MESSAGE_MAP()
};

