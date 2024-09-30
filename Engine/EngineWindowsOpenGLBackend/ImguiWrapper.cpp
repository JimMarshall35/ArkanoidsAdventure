#include "ImguiWrapper.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Sys.h"

void ImGuiWrapper::Init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(Sys::GetWindow(), true);
	const char* glsl_version2 = "#version 130";
	ImGui_ImplOpenGL3_Init(glsl_version2);
}

void ImGuiWrapper::Update()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	//ImGui::ShowDemoWindow();
	ImGui::Render();
}

void ImGuiWrapper::Draw()
{
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

bool ImGuiWrapper::WantsMouseInput()
{
	return ImGui::GetIO().WantCaptureMouse;
}

bool ImGuiWrapper::WantsKeyboardInput()
{
	return ImGui::GetIO().WantCaptureKeyboard;
}
