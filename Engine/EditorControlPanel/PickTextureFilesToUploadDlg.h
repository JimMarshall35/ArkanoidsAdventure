#pragma once
#include <afxwin.h>
#include "resource.h"
#include "UploadTextureFileMsg.h"
#include <unordered_map>

class PickTextureFilesToUploadDlg :
    public CDialog
{
public:
    PickTextureFilesToUploadDlg();
    virtual void DoDataExchange(CDataExchange* pDX) override;
    virtual BOOL OnInitDialog() override;

    const std::unordered_map<std::wstring, EditorServer::UploadTextureFileMsg>& GetData() const { return m_filesToLoadMap; }
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_TEXTURE_FILE_PICKER };
#endif
private:
    void EnableDisableControls(BOOL bEnable);
    void PopulateControlsFromSelectedMsg();

    afx_msg void OnAddBtnPressed();
    afx_msg void OnListboxSelChanged();
    afx_msg void OnRequiredComponentsComboChange();
    afx_msg void OnMinFilteringComboChange();
    afx_msg void OnMagFilteringComboChange();
    afx_msg void OnGenerateMipmapsCBChange();
    afx_msg void OnRetainCBChange();
    afx_msg void OnNameEditKillFocus();
    afx_msg void OnClampSChange();
    afx_msg void OnClampTChange();

private:
    CListBox m_ctlListBox;
    CButton m_ctlAddFileBtn;
    CEdit m_ctlNameEdit;
    CComboBox m_ctlRequiredComponentsCombo;
    CComboBox m_ctlMinFilterCombo;
    CComboBox m_ctlMagFilterCombo;
    CComboBox m_ctlClampS;
    CComboBox m_ctlClampT;
    CButton m_ctlGenerateMipMaps;
    CButton m_ctlRetainDataCPUSide;
    EditorServer::UploadTextureFileMsg* m_pCurrentSelectedMsg;

    std::unordered_map<std::wstring, EditorServer::UploadTextureFileMsg> m_filesToLoadMap;
private:
    DECLARE_MESSAGE_MAP()
};

