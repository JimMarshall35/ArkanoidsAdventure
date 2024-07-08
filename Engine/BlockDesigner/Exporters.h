#pragma once
#include "afx.h"
#include <vector>

class PrismMesh;

class IExporter
{
public:
	virtual void SetFolderPath(const CString& path) = 0;
	virtual bool EditSettings() = 0;
	virtual bool Export(const PrismMesh& prismMesh) = 0;
	virtual CString GetName() = 0;
};

std::vector<CString> GetExporters();

IExporter* GetExporter(int index);