#include "MeshComponent.h"
#include "EngineDLLInterface.h"
#include "IBackendApp.h"

MeshComponent::MeshComponent(HMesh mesh, HPipeline pipeline, entt::entity e)
{
	m_hDrawable = Engine::GetAPI().CreateDrawable(pipeline, mesh, (void*)e);
}

MeshComponent::~MeshComponent()
{
}
