#include "ImguiWrapper.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ImGuizmo.h"
#include "Sys.h"
#include "TransformComponent.h"

static Transform* pGizmoTransform = nullptr;

static ImGuizmo::MODE gGizmoMode = ImGuizmo::WORLD;
static ImGuizmo::OPERATION gGizmoOperation = ImGuizmo::TRANSLATE;

void ImGuiWrapper::Init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(Sys::GetWindow(), true);
	const char* glsl_version2 = "#version 130";
	ImGui_ImplOpenGL3_Init(glsl_version2);
	ImGui::GetIO().DisplaySize.x = Sys::GetW();
	ImGui::GetIO().DisplaySize.y = Sys::GetH();
}

static void RecalcLocalTransform()
{
	// TODO NEXT: implement this correctly
	
	//EAssert(pGizmoTransform);
	//const glm::vec3& globalPos = pGizmoTransform->getGlobalPosition();
	////const glm::vec3& globalRot = pGizmoTransform->getGlobalR();
	////const glm::vec3& globalScale = pGizmoTransform->getGlobalPosition();
	//if (Transform* pParent = pGizmoTransform->GetParent())
	//{
	//	const glm::vec3& parentGlobalPos = pParent->getGlobalPosition();
	//	pGizmoTransform->setLocalPosition(globalPos - parentGlobalPos);
	//	pGizmoTransform->ForceSetDirty(false); // just in case
	//}
	//else
	//{
	//	EAssert(false);
	//}
}

bool ImGuiWrapper::UpdateGizmo(glm::mat4* pV, glm::mat4* pP)
{
	bool rVal = false;
	if (pGizmoTransform)
	{
#define MAT_VAL_PTR(mat) &((*mat)[0][0])  // can probably just use the ptr to the matrix and cast to float ptr but dunno so I'm doing this 
		ImGuiIO& io = ImGui::GetIO();
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

		if (ImGuizmo::Manipulate(MAT_VAL_PTR(pV), MAT_VAL_PTR(pP), gGizmoOperation, gGizmoMode, MAT_VAL_PTR(&pGizmoTransform->getModelMatrixMut())))
		{
			RecalcLocalTransform();
			pGizmoTransform->SetChildrenDirty();
			rVal = true;
		}
#undef MAT_VAL_PTR
	}
	return rVal;
}

void ImGuiWrapper::EndFrame()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiWrapper::BeginFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();
}

bool ImGuiWrapper::WantsMouseInput()
{
	return ImGui::GetIO().WantCaptureMouse || ImGuizmo::IsUsing();
}

bool ImGuiWrapper::WantsKeyboardInput()
{
	return ImGui::GetIO().WantCaptureKeyboard;
}

void ImGuiWrapper::SetGizmo(Transform* pM)
{
	pGizmoTransform = pM;
}

void ImGuiWrapper::ClearGizmo()
{
	pGizmoTransform = nullptr;
}

void ImGuiWrapper::SetGizmoOperation(GizmoOperation gizmoOp)
{
	switch (gizmoOp)
	{
	case GizmoOperation::Rotate:    gGizmoOperation = ImGuizmo::OPERATION::ROTATE; return;
	case GizmoOperation::Scale:     gGizmoOperation = ImGuizmo::OPERATION::SCALE; return;
	case GizmoOperation::Translate: gGizmoOperation = ImGuizmo::OPERATION::TRANSLATE; return;
	}
}
