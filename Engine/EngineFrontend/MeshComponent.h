#pragma once
#include "PipelineTypes.h"
#include <glm/glm.hpp>
#include "PipelineTypes.h"
#include "Sphere.h"
#include "ComponentReg.h"
#include "EngineLib.h"
#include "ExportMacro.h"

class Transform;
class IArchive;
class PipeLine;
/// <summary>
/// 
/// </summary>
class ENGINE_FRONTEND_API MeshComponent
{
public:
	MeshComponent(){}
	//MeshComponent(HMesh mesh, PipeLine& pipeline, entt::entity e) { Create(mesh, pipeline, e); }
	void Create(HMesh mesh, PipeLine& pipeline, entt::entity e);
	void Destroy();

	bool Created() { return m_hDrawable != ENGINE_NULL_HANDLE; }
	HDrawable GetDrawable() const { return m_hDrawable; }
	HMesh GetMesh() const { return m_hMesh; }
	HPipeline GetPipeline() const{ return m_hPipeline; }
	Sphere GetMeshBoundingSphere() const;
	static void SerializeC(Comp::ComponentMeta* m, IArchive* ar, Entity e, EntityReg& reg);
//private:
	HMesh m_hMesh = ENGINE_NULL_HANDLE;
	HPipeline m_hPipeline = ENGINE_NULL_HANDLE;
	HDrawable m_hDrawable = ENGINE_NULL_HANDLE;
};

META_DECL(MeshComponent)
