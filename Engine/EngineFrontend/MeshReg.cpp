#include "MeshReg.h"
#include "IBackendApp.h"
#include "FrontendError.h"
#include "IArchive.h"
#include "PipelineMeshData.h"
#include "EngineDLLInterface.h"
#include "OBJ/ObjFile.h"

static const char* LoadFileIntoBuffer(const char* path, long& outBufferSize)
{
	char* source = NULL;

	FILE* fp = nullptr;
	fopen_s(&fp, path, "r");
	if (fp != NULL) {
		/* Go to the end of the file. */
		if (fseek(fp, 0L, SEEK_END) == 0) {
			/* Get the size of the file. */
			outBufferSize = ftell(fp);
			if (outBufferSize == -1) { /* Error */ }

			/* Allocate our buffer to that size. */
			source = (char*)malloc(sizeof(char) * (outBufferSize + 1));

			/* Go back to the start of the file. */
			if (fseek(fp, 0L, SEEK_SET) != 0) { /* Error */ }

			/* Read the entire file into memory. */
			size_t newLen = fread(source, sizeof(char), outBufferSize, fp);
			if (ferror(fp) != 0) {
				fputs("Error reading file", stderr);
			}
			else {
				source[newLen++] = '\0'; /* Just to be safe. */
			}
		}
		fclose(fp);
	}
	return source;
}


bool MeshReg::UploadMeshData(PipelineMeshData& data)
{
	const BackendAPI& api = Engine::GetAPI();
	auto itr = m_NameToHandle.find(data.GetName());
	if (itr != m_NameToHandle.end())
	{
		Err::LogWarning("Mesh named '%s' already taken", data.GetName().c_str());
		return false;
	}
	HMesh hmesh = api.UploadMesh(data);
	if (hmesh == ENGINE_NULL_HANDLE)
	{
		Err::LogError("Mesh named '%s' failed to load", data.GetName().c_str());
		return false;
	}
	m_Meshes.push_back(EPair<PipelineMeshData, HMesh>(data, hmesh));
	m_NameToHandle[data.GetName()] = hmesh;
	m_HandleToData[hmesh] = &(m_Meshes.back().first);
	Err::LogInfo("Mesh named '%s' successfully uploaded. Handle: %i", data.GetName().c_str(), hmesh);
	return true;
}

std::string GetFileExtension(const char* file)
{
	size_t len = strnlen_s(file, 260);
	while (file[--len] != '.')
	{
	}
	const char* r = &file[len];
	return r;
}

bool MeshReg::UploadMeshData(const char* filePath)
{
	EVec<EString> errors;
	long bufSize = 0;
	const BackendAPI& api = Engine::GetAPI();
	const char* fullPath = api.GetAssetFullPathFn(filePath);
	const char* fileData = LoadFileIntoBuffer(fullPath, bufSize);
	PipelineMeshData md(filePath);
	std::string extension = GetFileExtension(filePath);
	if (extension == ".obj")
	{
		if (OBJLoader::LoadFromFile(fileData, bufSize, errors, md))
		{
			for (const EString& error : errors)
			{
				Err::LogError(error.c_str());
			}
			return true;
		}
		else
		{
			UploadMeshData(md);
		}
	}
	else
	{
		Err::LogError("Invalid file extension '%s'", extension.c_str());
		return true;
	}
	
	if (fileData)
	{
		free((void*)fileData);
	}
	return false;
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
