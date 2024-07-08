#include "pch.h"
#include "OBJExporter.h"
#include "resource.h"
#include "SerializeObj.h"
#include "stb_image_write.h"
#include "PrismMesh.h"

class OBJExporterSettingsDlg : public CDialog
{
    friend class OBJExporter;
public:
    OBJExporterSettingsDlg(const CString& folderPath)
        :CDialog(IDD),
        m_TextureName(_T("MyTexture")),
        m_TextureFileExt(_T(".tga")),
        m_MeshName(_T("MyMesh")),
        m_FolderPath(folderPath),
        m_TextureFileType(ftTGA)
    {
        m_TexturePath.Format(_T("%s\\%s%s"), m_FolderPath, m_TextureName, m_TextureFileExt);
        m_MeshPath.Format(_T("%s\\%s%s"), m_FolderPath, m_MeshName, _T(".obj"));
    }

    virtual void DoDataExchange(CDataExchange* pDX) override
    {
        CDialog::DoDataExchange(pDX);
        DDX_Control(pDX, IDC_OBJ_TEXTURE_TYPE_COMBO, m_ctlTextureType);
        DDX_Text(pDX, IDC_OBJ_TEXTURE_FILENAME_EDIT, m_TextureName);
        DDX_Text(pDX, IDC_OBJ_MESH_FILENAME_EDIT, m_MeshName);
        DDX_Text(pDX, IDC_OBJ_TEXTURE_FILEPATH, m_TexturePath);
        DDX_Text(pDX, IDC_OBJ_MESH_FILEPATH, m_MeshPath);
    }

    virtual BOOL OnInitDialog() override
    {
        BOOL b = CDialog::OnInitDialog();
        m_ctlTextureType.AddString(_T(".png"));
        m_ctlTextureType.AddString(_T(".tga"));
        m_ctlTextureType.SetCurSel(1);
        return b;
    }

    afx_msg void OnComboSelchange()
    {
        CString str;
        UpdateData(TRUE);
        int sel = m_ctlTextureType.GetCurSel();
        int n = m_ctlTextureType.GetLBTextLen(sel);

        m_ctlTextureType.GetLBText(sel, str.GetBuffer(n));

        if (str == _T(".tga"))
        {
            m_TextureFileType = ftTGA;
            m_TextureFileExt = _T(".tga");
        }
        else if (str == _T(".png"))
        {
            m_TextureFileType = ftPNG;
            m_TextureFileExt = _T(".png");
        }
        m_TexturePath.Format(_T("%s\\%s%s"), m_FolderPath, m_TextureName, m_TextureFileExt);
        str.ReleaseBuffer();

        UpdateData(FALSE);
    }

    afx_msg void OnLoseTexNameFocus()
    {
        UpdateData(TRUE);
        m_TexturePath.Format(_T("%s\\%s%s"), m_FolderPath, m_TextureName, m_TextureFileExt);
        UpdateData(FALSE);

    }

    afx_msg void OnLoseMeshNameFocus()
    {
        UpdateData(TRUE);
        m_MeshPath.Format(_T("%s\\%s%s"), m_FolderPath, m_MeshName, _T(".obj"));
        UpdateData(FALSE);
    }
private:
    CString m_TextureName;
    CString m_TextureFileExt;
    CString m_MeshName;
    CString m_FolderPath;

    CString m_MeshPath;
    CString m_TexturePath;

    TextureFileType m_TextureFileType;

    CComboBox m_ctlTextureType;
public:
    enum {IDD = IDD_OBJ_EXPORT_SETTINGS};
private:
    DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(OBJExporterSettingsDlg, CDialog)
    ON_CBN_SELCHANGE(IDC_OBJ_TEXTURE_TYPE_COMBO, &OnComboSelchange)
    ON_EN_KILLFOCUS(IDC_OBJ_TEXTURE_FILENAME_EDIT, &OnLoseTexNameFocus)
    ON_EN_KILLFOCUS(IDC_OBJ_MESH_FILENAME_EDIT, &OnLoseMeshNameFocus)

END_MESSAGE_MAP()

void OBJExporter::SetFolderPath(const CString& path)
{
    m_FolderPath = path;
}

bool OBJExporter::EditSettings()
{
    OBJExporterSettingsDlg dlg(m_FolderPath);
    if (dlg.DoModal() == IDOK)
    {
        GetDialogueData(dlg);
        return true;
    }

    return false;
}

char* TO_CHAR(const wchar_t* string)
{
    size_t len = wcslen(string) + 1;
    char* c_string = new char[len];
    size_t numCharsRead;
    wcstombs_s(&numCharsRead, c_string, len, string, _TRUNCATE);
    return c_string;
}

bool OBJExporter::Export(const PrismMesh& prismMesh)
{
    char* meshPath = TO_CHAR(m_MeshPath);
    char* texPath = TO_CHAR(m_TexturePath);
    SaveAsObj(prismMesh, meshPath);
    switch (m_TextureType)
    {
    case ftTGA:
        stbi_write_tga(texPath, prismMesh.GetTextureWidth(), prismMesh.GetTextureHeight(), 3, prismMesh.GetTexture());
        break;
    case ftPNG:
        stbi_write_png(texPath, prismMesh.GetTextureWidth(), prismMesh.GetTextureHeight(), 3, prismMesh.GetTexture(), 3);
        break;
    }
    delete meshPath;
    delete texPath;
    return false;
}

CString OBJExporter::GetName()
{
    return _T("OBJ");
}

void OBJExporter::GetDialogueData(const OBJExporterSettingsDlg& dlg)
{
    m_MeshPath = dlg.m_MeshPath;
    m_TexturePath = dlg.m_TexturePath;
    m_TextureType = dlg.m_TextureFileType;
}
