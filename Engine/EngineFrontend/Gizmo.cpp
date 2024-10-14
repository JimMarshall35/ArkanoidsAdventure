#include "Gizmo.h"
#include "Scene.h"
#include "EngineDLLInterface.h"
#include "TransformComponent.h"
#include "IBackendApp.h"
#include "CameraComponent.h"
#include "XMLArchive.h"
#include "EditComponentMessage.h"
#include "EditorServerMsg.h"
#include "EditorServer.h"

namespace Gizmo
{
	static Entity gCurrentGizmoEntity = entt::null;

	void OnGizmoUpdated()
	{
		EntityReg& reg = Scn::GetScene().entities.GetReg();
		Transform& t = reg.get<Transform>(gCurrentGizmoEntity);
		XMLArchive ar("", true);
		if (Comp::ComponentMeta* pMeta = Comp::ComponentMeta::FindByName("Transform"))
		{
			pMeta->Serialize(&ar, gCurrentGizmoEntity, reg);
		}
		else
		{
			Err::LogErrorLocal("[OnGizmoUpdated] Can't find transform meta");
		}
		EditorServer::Msg msg;
		msg.Type = EditorServer::MsgType::EditComponent;
		msg.Data = EditorServer::EditComponentMsg(
			(size_t)gCurrentGizmoEntity,
			ar.AsString().c_str()
		);
		Editor::EnqueueMsg(msg);
	}
	
	void SetGizmo(Entity e)
	{
		const BackendAPI& api = Engine::GetAPI();
		Scn::Scene& sn = Scn::GetScene();
		EntityReg& r = sn.entities.GetReg();
		if (Transform* et = r.try_get<Transform>(e))
		{
			api.SetGizmo(et);
		}
		gCurrentGizmoEntity = e;
	}

	void DismissGizmo()
	{
		const BackendAPI& api = Engine::GetAPI();
		api.ClearGizmo();
		gCurrentGizmoEntity = entt::null;
	}

	void UpdateGizmo()
	{
		const BackendAPI& api = Engine::GetAPI();
		Scn::Scene& sn = Scn::GetScene();
		EntityReg& r = sn.entities.GetReg();
		Transform& camT = r.get<Transform>(sn.activeCameraAntity);
		CameraComponent& cam = r.get<CameraComponent>(sn.activeCameraAntity);
		glm::mat4 view = cam.GetView(camT);
		glm::mat4 proj = cam.GetProj();
		if (api.UpdateGizmo(&view, &proj))
		{
			OnGizmoUpdated();
		}
	}

	void SetGizmoOperation(GizmoOperation op)
	{
		const BackendAPI& api = Engine::GetAPI();
		api.SetGizmoOperation(op);
	}

}
