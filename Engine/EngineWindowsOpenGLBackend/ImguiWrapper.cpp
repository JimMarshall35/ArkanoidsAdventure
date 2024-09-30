#include "ImguiWrapper.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "ImGuizmo.h"
#include "Sys.h"
#include "TransformComponent.h"

static glm::mat4* pGizmoModelMatrix = nullptr;
static glm::mat4* pGizmoViewMatrix = nullptr;
static glm::mat4* pGizmoProjMatrix = nullptr;

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

void ImGuiWrapper::Update()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	//
	//ImGui::ShowDemoWindow();
	if (pGizmoModelMatrix)
	{
#define MAT_VAL_PTR(mat) &((*mat)[0][0])  // can probably just use the ptr to the matrix and cast to float ptr but dunno so I'm doing this 
		EAssert(pGizmoViewMatrix);
		EAssert(pGizmoProjMatrix);
		ImGuiIO& io = ImGui::GetIO();
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

		ImGuizmo::Manipulate(MAT_VAL_PTR(pGizmoViewMatrix), MAT_VAL_PTR(pGizmoProjMatrix), gGizmoOperation, gGizmoMode, MAT_VAL_PTR(pGizmoModelMatrix));
#undef MAT_VAL_PTR
	}
	ImGui::Render();
}

void ImGuiWrapper::Draw()
{
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool ImGuiWrapper::WantsMouseInput()
{
	return ImGui::GetIO().WantCaptureMouse || ImGuizmo::IsUsing();
}

bool ImGuiWrapper::WantsKeyboardInput()
{
	return ImGui::GetIO().WantCaptureKeyboard;
}

void ImGuiWrapper::SetGizmo(glm::mat4* pM, glm::mat4* pV, glm::mat4* pP)
{
	pGizmoModelMatrix = pM;
	pGizmoProjMatrix = pP;
	pGizmoViewMatrix = pV;
}

void ImGuiWrapper::ClearGizmo()
{
	pGizmoModelMatrix = nullptr;
	pGizmoProjMatrix = nullptr;
	pGizmoViewMatrix = nullptr;
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
