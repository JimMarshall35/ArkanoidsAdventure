#pragma once
#include "Exporters.h"
#include <string>
#include <memory>

enum TextureFileType
{
	ftTGA,
	ftPNG
};

class OBJExporterSettingsDlg;

class OBJExporter : public IExporter
{
public:

	// Inherited via IExporter
	virtual void SetFolderPath(const CString& path) override;
	virtual bool EditSettings() override;
	virtual bool Export(const PrismMesh& prismMesh) override;
	virtual CString GetName() override;

private:
	void GetDialogueData(const OBJExporterSettingsDlg& dlg);
private:
	CString m_FolderPath;


	CString m_MeshPath;
	CString m_TexturePath;
	TextureFileType m_TextureType;
};

