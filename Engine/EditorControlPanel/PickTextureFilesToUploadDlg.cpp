#include "pch.h"
#include "PickTextureFilesToUploadDlg.h"
#include "EditorClient.h"
#include "StringHelpers.h"
#include "FilePathHelpers.h"

BEGIN_MESSAGE_MAP(PickTextureFilesToUploadDlg, CDialog)
	ON_BN_CLICKED(IDC_ADD_TEXTURE_FILE_BTN, &OnAddBtnPressed)
	ON_LBN_SELCHANGE(IDC_TEXTURES_TO_LOAD_LIST, &OnListboxSelChanged)
	ON_CBN_SELCHANGE(IDC_REQUIRED_COMPONENTS_COMBO, &OnRequiredComponentsComboChange)
	ON_CBN_SELCHANGE(IDC_MIN_FILTERING, &OnMinFilteringComboChange)
	ON_CBN_SELCHANGE(IDC_MAG_FILTERING, &OnMagFilteringComboChange)
	ON_CBN_SELCHANGE(IDC_CLAMP_S, &OnClampSChange)
	ON_CBN_SELCHANGE(IDC_CLAMP_T, &OnClampTChange)


	ON_BN_CLICKED(IDC_GENERATE_MIPMAPS_CB, &OnGenerateMipmapsCBChange)
	ON_BN_CLICKED(IDC_RETAIN_CPU_CB, &OnRetainCBChange)
	ON_EN_KILLFOCUS(IDC_NAME_EDIT, &OnNameEditKillFocus)
END_MESSAGE_MAP()

PickTextureFilesToUploadDlg::PickTextureFilesToUploadDlg()
	:CDialog(IDD_TEXTURE_FILE_PICKER)
{
}

void PickTextureFilesToUploadDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_TEXTURES_TO_LOAD_LIST, m_ctlListBox);
	DDX_Control(pDX, IDC_ADD_TEXTURE_FILE_BTN, m_ctlAddFileBtn);
	DDX_Control(pDX, IDC_NAME_EDIT, m_ctlNameEdit);
	DDX_Control(pDX, IDC_REQUIRED_COMPONENTS_COMBO, m_ctlRequiredComponentsCombo);
	DDX_Control(pDX, IDC_MIN_FILTERING, m_ctlMinFilterCombo);
	DDX_Control(pDX, IDC_MAG_FILTERING, m_ctlMagFilterCombo);
	DDX_Control(pDX, IDC_CLAMP_S, m_ctlClampS);
	DDX_Control(pDX, IDC_CLAMP_T, m_ctlClampT);
	DDX_Control(pDX, IDC_GENERATE_MIPMAPS_CB, m_ctlGenerateMipMaps);
	DDX_Control(pDX, IDC_RETAIN_CPU_CB, m_ctlRetainDataCPUSide);

}

BOOL PickTextureFilesToUploadDlg::OnInitDialog()
{
	BOOL b = CDialog::OnInitDialog();
	UpdateData(FALSE);

	m_ctlRequiredComponentsCombo.AddString(_T("0"));
	m_ctlRequiredComponentsCombo.AddString(_T("3"));
	m_ctlRequiredComponentsCombo.AddString(_T("4"));
	m_ctlRequiredComponentsCombo.SetCurSel(0);

	/* order in listbox has to match order in enum - easier*/
	m_ctlMinFilterCombo.AddString(_T("Nearest"));
	m_ctlMinFilterCombo.AddString(_T("Linear"));
	m_ctlMinFilterCombo.AddString(_T("Nearest_MipMapNearest"));
	m_ctlMinFilterCombo.AddString(_T("Linear_MipMapNearest"));
	m_ctlMinFilterCombo.AddString(_T("Nearest_MipMapLinear"));
	m_ctlMinFilterCombo.AddString(_T("Linear_MipMapLinear"));
	m_ctlMinFilterCombo.SetCurSel(0);

	m_ctlMagFilterCombo.AddString(_T("Nearest"));
	m_ctlMagFilterCombo.AddString(_T("Linear"));
	m_ctlMagFilterCombo.AddString(_T("Nearest_MipMapNearest"));
	m_ctlMagFilterCombo.AddString(_T("Linear_MipMapNearest"));
	m_ctlMagFilterCombo.AddString(_T("Nearest_MipMapLinear"));
	m_ctlMagFilterCombo.AddString(_T("Linear_MipMapLinear"));
	m_ctlMagFilterCombo.SetCurSel(0);

	m_ctlClampS.AddString(_T("Repeat"));
	m_ctlClampS.AddString(_T("MirroredRepeat"));
	m_ctlClampS.AddString(_T("ClampToEdge"));
	m_ctlClampS.AddString(_T("ClampToBorder"));
	m_ctlClampS.SetCurSel(0);

	m_ctlClampT.AddString(_T("Repeat"));
	m_ctlClampT.AddString(_T("MirroredRepeat"));
	m_ctlClampT.AddString(_T("ClampToEdge"));
	m_ctlClampT.AddString(_T("ClampToBorder"));
	m_ctlClampT.SetCurSel(0);

	m_ctlGenerateMipMaps.SetCheck(BST_CHECKED);
	m_ctlRetainDataCPUSide.SetCheck(BST_CHECKED);

	EnableDisableControls(FALSE);

	return b;
}

void PickTextureFilesToUploadDlg::EnableDisableControls(BOOL bEnable)
{
	m_ctlRetainDataCPUSide.EnableWindow(bEnable);
	m_ctlGenerateMipMaps.EnableWindow(bEnable);
	m_ctlClampT.EnableWindow(bEnable);
	m_ctlClampS.EnableWindow(bEnable);
	m_ctlMagFilterCombo.EnableWindow(bEnable);
	m_ctlMinFilterCombo.EnableWindow(bEnable);
	m_ctlRequiredComponentsCombo.EnableWindow(bEnable);
	m_ctlNameEdit.EnableWindow(bEnable);
}

void PickTextureFilesToUploadDlg::PopulateControlsFromSelectedMsg()
{
	ASSERT(m_pCurrentSelectedMsg);
	EnableDisableControls(TRUE);
	std::wstring ws = s2ws(m_pCurrentSelectedMsg->name.c_str());
	m_ctlNameEdit.SetWindowText(ws.c_str());
	m_ctlGenerateMipMaps.SetCheck(m_pCurrentSelectedMsg->options.bGenerateMipMaps ? BST_CHECKED : BST_UNCHECKED);
	m_ctlRetainDataCPUSide.SetCheck(m_pCurrentSelectedMsg->options.bRetainDataCPUMemory ? BST_CHECKED : BST_UNCHECKED);
	switch (m_pCurrentSelectedMsg->options.requiredComponents)
	{
	case 0:
		m_ctlRequiredComponentsCombo.SetCurSel(0);
		break;
	case 3:
		m_ctlRequiredComponentsCombo.SetCurSel(1);
		break;
	case 4:
		m_ctlRequiredComponentsCombo.SetCurSel(2);
		break;
	default:
		ASSERT(false);
	}
	m_ctlMinFilterCombo.SetCurSel((int)m_pCurrentSelectedMsg->options.MinifyFiltering);
	m_ctlMagFilterCombo.SetCurSel((int)m_pCurrentSelectedMsg->options.MagnifyFiltering);
	m_ctlClampS.SetCurSel((int)m_pCurrentSelectedMsg->options.TextureClampS);
	m_ctlClampT.SetCurSel((int)m_pCurrentSelectedMsg->options.TextureClampT);
}

void PickTextureFilesToUploadDlg::OnAddBtnPressed()
{
	EditorClient::GetAssetFolderPathAsync([this](const char* folder) {
		CString sFilter = _T("TGA Files (*.tga)|*.tga|PNG Files (*.png)|*.png|All Files (*.*)|*.*||");

		std::wstring w = s2ws(folder);

		w += '\\';

		CFileDialog my_file_dialog(TRUE, _T("xxx"), w.c_str(),
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT,
			sFilter, this);

		if (my_file_dialog.DoModal() != IDOK)
			return;

		POSITION pos(my_file_dialog.GetStartPosition());
		while (pos)
		{
			CString filename = my_file_dialog.GetNextPathName(pos);
			std::wstring ext = FilePath::GetFileExtension(filename);
			if (ext != L".png" && ext != L".tga")
			{
				CString msg;
				msg.Format(L"texture file: '%s'. invalid extension", filename);
				AfxMessageBox(msg, MB_OK);
				continue;
			}
			if (!FilePath::IsFilepathWithinFolder(w.c_str(), filename))
			{
				CString msg;
				msg.Format(L"texture file: '%s'. not within assets folder: '%s'", filename, w.c_str());
				AfxMessageBox(msg, MB_OK);
				continue;
			}
			std::wstring relativePath = FilePath::GetFilepathRelativeToFolder(w.c_str(), filename);
			std::wstring stem = FilePath::GetFileNameWithoutExtension(filename);

			if (m_filesToLoadMap.find(relativePath) != m_filesToLoadMap.end())
			{
				CString msg;
				msg.Format(L"texture file: '%s' already selected to load", relativePath.c_str());
				AfxMessageBox(msg, MB_OK);
				continue;
			}

			m_filesToLoadMap[relativePath] = {
				EditorServer::UploadTextureFileOptions{},
				ws2s(relativePath.c_str()),
				ws2s(stem.c_str())
			};

			m_ctlListBox.AddString(relativePath.c_str());
		}
	});
}

void PickTextureFilesToUploadDlg::OnListboxSelChanged()
{
	int sel = m_ctlListBox.GetCurSel();
	CString txt;
	m_ctlListBox.GetText(sel, txt);
	std::wstring ws = txt;
	if (m_filesToLoadMap.find(ws) != m_filesToLoadMap.end())
	{
		m_pCurrentSelectedMsg = &m_filesToLoadMap.at(ws);
		PopulateControlsFromSelectedMsg();
	}
	else
	{
		ASSERT(false);
	}
}

void PickTextureFilesToUploadDlg::OnRequiredComponentsComboChange()
{
	ASSERT(m_pCurrentSelectedMsg);
	switch (m_ctlRequiredComponentsCombo.GetCurSel())
	{
	case 0:
		m_pCurrentSelectedMsg->options.requiredComponents = 0;
		break;
	case 1:
		m_pCurrentSelectedMsg->options.requiredComponents = 3;
		break;
	case 2:
		m_pCurrentSelectedMsg->options.requiredComponents = 4;
		break;
	default:
		ASSERT(false);
	}
}

void PickTextureFilesToUploadDlg::OnMinFilteringComboChange()
{
	ASSERT(m_pCurrentSelectedMsg);
	ASSERT(m_ctlMinFilterCombo.GetCurSel() < (int)EditorServer::TextureFiltering::NumOptions);
	m_pCurrentSelectedMsg->options.MinifyFiltering = (EditorServer::TextureFiltering)m_ctlMinFilterCombo.GetCurSel();
}

void PickTextureFilesToUploadDlg::OnMagFilteringComboChange()
{
	ASSERT(m_pCurrentSelectedMsg);
	ASSERT(m_ctlMagFilterCombo.GetCurSel() < (int)EditorServer::TextureFiltering::NumOptions);
	m_pCurrentSelectedMsg->options.MagnifyFiltering = (EditorServer::TextureFiltering)m_ctlMagFilterCombo.GetCurSel();
}

void PickTextureFilesToUploadDlg::OnGenerateMipmapsCBChange()
{
	ASSERT(m_pCurrentSelectedMsg);
	m_pCurrentSelectedMsg->options.bGenerateMipMaps = m_ctlGenerateMipMaps.GetCheck() == BST_CHECKED ? true : false;
}

void PickTextureFilesToUploadDlg::OnRetainCBChange()
{
	ASSERT(m_pCurrentSelectedMsg);
	m_pCurrentSelectedMsg->options.bRetainDataCPUMemory = m_ctlRetainDataCPUSide.GetCheck() == BST_CHECKED ? true : false;
}

void PickTextureFilesToUploadDlg::OnNameEditKillFocus()
{
	ASSERT(m_pCurrentSelectedMsg);
	static wchar_t buf[1024];
	m_ctlNameEdit.GetWindowTextW(buf, 1024);
	m_pCurrentSelectedMsg->name = ws2s(buf);
}

void PickTextureFilesToUploadDlg::OnClampSChange()
{
	ASSERT(m_pCurrentSelectedMsg);
	ASSERT(m_ctlClampS.GetCurSel() < (int)EditorServer::TextureClamp::NumOptions);
	m_pCurrentSelectedMsg->options.TextureClampS = (EditorServer::TextureClamp)m_ctlClampS.GetCurSel();

}

void PickTextureFilesToUploadDlg::OnClampTChange()
{
	ASSERT(m_pCurrentSelectedMsg);
	ASSERT(m_ctlClampT.GetCurSel() < (int)EditorServer::TextureClamp::NumOptions);
	m_pCurrentSelectedMsg->options.TextureClampT = (EditorServer::TextureClamp)m_ctlClampT.GetCurSel();

}
