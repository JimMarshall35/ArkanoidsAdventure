#include "Scene.h"
#include <entt/entt.hpp>
#include <optional>
#include "EngineDLLInterface.h"
#include "IBackendApp.h"
#include "FrontendError.h"
#include "Pipeline.h"
#include "IBackendApp.h"
#include "IArchive.h"
#include "ComponentReg.h"

namespace Scn
{
	static std::optional<Scene> gTheScene;

	bool IsSceneLoaded()
	{
		return gTheScene.has_value();
	}

	Scene& GetScene()
	{
		return gTheScene.value();
	}

	void DeserialiseSceneV1(IArchive& archive)
	{
		Scene& s = GetScene();
		s.entities.Serialize(archive);
		s.meshReg.Serialize(archive);
		s.pipelines.Serialize(archive);
	}

	void SerializeScene(IArchive& archive)
	{
		const u32 version = 1;
		if (archive.IsStoring())
		{
			archive << version;
			Scene& s = GetScene();
			s.entities.Serialize(archive);
			s.meshReg.Serialize(archive);
			s.pipelines.Serialize(archive);
		}
		else
		{
			u32 v; 
			archive >> v;
			switch (v)
			{
			case 1:
				DeserialiseSceneV1(archive);
				break;
			default:
				break;
			}
		}
		

	}

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

	size_t GetRequiredUniformMem(PipeLine& piprlinr)
	{
		size_t s = 0;
		const EVec<PipeLineStage>& stages = piprlinr.GetStages();
		for (const PipeLineStage& st : stages)
		{
			const EVec<PipelinePropertyName>& attr = st.GetUniformAttributes();
			for (const PipelinePropertyName& n : attr)
			{
				switch (n.PropType)
				{
				case PipelinePropertyType::Float:
					s += sizeof(float);
					break;
				case PipelinePropertyType::Vec2:
					s += sizeof(glm::vec2);
					break;
				case PipelinePropertyType::Vec3:
					s += sizeof(glm::vec3);
					break;
				case PipelinePropertyType::Vec4:
					s += sizeof(glm::vec4);
					break;
				case PipelinePropertyType::UInt32:
					s += sizeof(u32);
					break;
				case PipelinePropertyType::Mat4:
					s += sizeof(glm::mat4);
					break;
				case PipelinePropertyType::Texture:
					s += sizeof(u32);
					break;
				}
			}
		}
		return s;
	}

	bool PipelineReg::UploadPipelineData(PipeLine& piprlinr)
	{
		if (m_NameToHandle.find(piprlinr.GetName()) != m_NameToHandle.end())
		{
			Err::ReportError(Err::FrontendErrorSeverity::Error, "Pipeline name already taken, name: '%s'", piprlinr.GetName());
			return false;
		}
		size_t mem = GetRequiredUniformMem(piprlinr);
		HPipeline p = Engine::GetAPI().UploadPipeline(piprlinr, pUserData);
		EVec<EVec<HPipelineUniformProperty>> propertyHandles(piprlinr.GetStages().size());

		if (p == ENGINE_NULL_HANDLE)
		{
			Err::ReportError(Err::FrontendErrorSeverity::Error, "Pipeline upload failed, name: %s", piprlinr.GetName());
			return false;
		}
		
		m_Pipelines.push_back({ piprlinr , p });
		m_NameToHandle[piprlinr.GetName()] = p;
		return true;
	}

	void PipelineReg::Clear()
	{
		const BackendAPI& api = Engine::GetAPI();
		for (EPair<PipeLine, HPipeline>& p : m_Pipelines)
		{
			api.DestroyPipeline(p.second);
		}
		m_Pipelines.clear();
		m_NameToHandle.clear();
	}

	void PipelineReg::Serialize(IArchive& archive)
	{
	}


	void EntityRegWrapper::Clear()
	{
		m_Reg.clear();
	}

	void EntityRegWrapper::Serialize(IArchive& archive)
	{
		const int version = 1;
		if (archive.IsStoring())
		{
			archive.PushObj("Entities");
			archive << version;
			auto v = m_Reg.view<entt::entity>();
			for (Entity e : v)
			{
				archive.PushObj("Entity");
				Autolist<Comp::ComponentMeta>* pMeta = Autolist<Comp::ComponentMeta>::GetHead();
				while (pMeta = pMeta->GetNext())
				{
					Comp::ComponentMeta* meta = (Comp::ComponentMeta*)pMeta;
					meta->Serialize(&archive, e, m_Reg);
				}
				archive.PopObj();
			}
			archive.PopObj();
		}
		else
		{
			archive.PushObj("Entities");
				int version;
				archive >> version;
				switch (version)
				{
				case 1:
				{
					int num = archive.CountChildren();
					for (int i = 0; i < num; i++)
					{
						archive.PushChild(i);
						Entity e = m_Reg.create();
						Autolist<Comp::ComponentMeta>* pMeta = Autolist<Comp::ComponentMeta>::GetHead();
						while (pMeta = pMeta->GetNext())
						{
							Comp::ComponentMeta* meta = (Comp::ComponentMeta*)pMeta;
							meta->Serialize(&archive, e, m_Reg);
						}
						archive.PopObj();
					}
					break;
				}
					
				default:
					// @ERROR
					break;
				}
			archive.PopObj();
		}
	}

	bool TextureReg::RegisterTexture(const TextureData& data, TextureDataFreeFn freer)
	{
		if (m_NameMap.find(data.Name) != m_NameMap.end())
		{
			Err::ReportError(Err::FrontendErrorSeverity::Warning, "Texture name '%s' taken", data.Name);
			return false;
		}
		HTexture t = Engine::GetAPI().UploadTexture(data);
		if (t == ENGINE_NULL_HANDLE)
		{
			Err::ReportError(Err::FrontendErrorSeverity::Warning, "Texture name '%s' upload failed", data.Name);
			return false;
		}
		m_Textures.push_back({ data, t });
		EAssert(m_Freeers.find(data.Name) == m_Freeers.end());
		m_Freeers[data.Name] = freer;
		EAssert(m_NameMap.find(data.Name) == m_NameMap.end());
		m_NameMap[data.Name] = &m_Textures[m_Textures.size() - 1];
		return true;
	}
	void TextureReg::Clear()
	{
	}
	void TextureReg::Serialize(IArchive& archive)
	{
	}
}
