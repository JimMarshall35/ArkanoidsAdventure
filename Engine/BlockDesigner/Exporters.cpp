#include "pch.h"
#include "Exporters.h"
#include <memory>
#include "OBJExporter.h"

static std::vector<std::unique_ptr<IExporter>> Exporters;

static void PopulateExporters()
{
    Exporters.push_back(std::make_unique<OBJExporter>());
}

std::vector<CString> GetExporters()
{
    if (Exporters.empty())
    {
        PopulateExporters();
    }
    std::vector<CString> r;
    for (const std::unique_ptr<IExporter>& e : Exporters)
    {
        r.push_back(e->GetName());
    }
    return r;
}

IExporter* GetExporter(int index)
{
    if (Exporters.empty())
    {
        PopulateExporters();
    }
    return Exporters[index].get();
}
