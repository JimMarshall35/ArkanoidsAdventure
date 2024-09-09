#include "MeshReg.h"
#include "IBackendApp.h"
#include "FrontendError.h"
#include "IArchive.h"
#include "PipelineMeshData.h"
#include "EngineDLLInterface.h"

bool MeshReg::UploadMeshData(PipelineMeshData& data)
{
	const BackendAPI& api = Engine::GetAPI();
	auto itr = m_NameToHandle.find(data.GetName());
	if (itr != m_NameToHandle.end())
	{
		Err::ReportError(Err::FrontendErrorSeverity::Warning, "Mesh named '%s' already taken", data.GetName().c_str());
		return false;
	}
	HMesh hmesh = api.UploadMesh(data);
	if (hmesh == ENGINE_NULL_HANDLE)
	{
		Err::ReportError(Err::FrontendErrorSeverity::Error, "Mesh named '%s' failed to load", data.GetName().c_str());
		return false;
	}
	m_Meshes.push_back(EPair<PipelineMeshData, HMesh>(data, hmesh));
	m_NameToHandle[data.GetName()] = hmesh;
	m_HandleToData[hmesh] = &(m_Meshes.back().first);
	Err::ReportError(Err::FrontendErrorSeverity::Info, "Mesh named '%s' successfully uploaded. Handle: %i", data.GetName().c_str(), hmesh);
	return true;
}


void MeshReg::Clear()
{
	const BackendAPI& api = Engine::GetAPI();
	for (EPair<PipelineMeshData, HMesh>& p : m_Meshes)
	{
		api.DestroyMesh(p.second);
	}
	m_Meshes.clear();
	m_NameToHandle.clear();
	m_HandleToData.clear();
}

PipelineMeshData* MeshReg::TryGetPipelineMeshData(HMesh mesh)
{
	if (m_HandleToData.find(mesh) != m_HandleToData.end())
	{
		return m_HandleToData.at(mesh);
	}
	return nullptr;
}

void MeshReg::Serialize(IArchive& archive)
{
	int version = 1;

	if (archive.IsStoring())
	{
		archive.PushObj("MeshReg");
		archive.PushObj("Version");
		archive << version;
		archive.PopObj();
		archive.PushObj("Meshes");
		archive << m_Meshes.size();
		for (EPair<PipelineMeshData, HMesh>& p : m_Meshes)
		{
			archive.PushObj("Mesh");
			archive.PushObj("Handle");
			archive << p.second;
			archive.PopObj();
			p.first.Serialize(archive);
			archive.PopObj();
		}
		archive.PopObj();
		archive.PopObj();
	}
	else
	{
		archive.PushObj("MeshReg");
		int v;
		archive.PushObj("Version");
		archive >> v;
		archive.PopObj();

		switch (v)
		{
		case 1:
			archive.PushObj("Meshes");
			size_t numMeshes;
			archive >> numMeshes;
			m_Meshes.resize(numMeshes);
			for (EPair<PipelineMeshData, HMesh>& p : m_Meshes)
			{
				archive.PushObj("Mesh");
				archive.PushObj("Handle");
				archive >> p.second;
				archive.PopObj();
				p.first.Serialize(archive);
				archive.PopObj();
			}
			archive.PopObj();
			break;
		default:
			break;
		}
		archive.PopObj();
	}
}

void MeshReg::ReloadMeshes(EMap<HMesh, HMesh>& outOldToNewHandleMap)
{
	const BackendAPI& api = Engine::GetAPI();
	for (EPair<PipelineMeshData, HMesh>& p : m_Meshes)
	{
		HMesh m = api.UploadMesh(p.first);
		m_HandleToData[m] = &p.first;
		m_NameToHandle[p.first.GetName()] = m;
		outOldToNewHandleMap[p.second] = m;
		p.second = m;
	}
}

HMesh MeshReg::TryGetMesh(const EString& name) const
{
	if (m_NameToHandle.find(name) != m_NameToHandle.end())
	{
		return m_NameToHandle.at(name);
	}
	return ENGINE_NULL_HANDLE;
}
