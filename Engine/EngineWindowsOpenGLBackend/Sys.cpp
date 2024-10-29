#include "Sys.h"
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "IBackendApp.h"
#include "OGLRenderer.h"
#include "Input.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#ifdef _WIN32
#include <windows.h>
#endif

namespace Sys
{
	static uint32_t WIDTH = 800;
	static uint32_t HEIGHT = 600;

	static GLFWwindow* gWindow = nullptr;
	static BackendWindowResizeFn ResizeHandler = nullptr;

	GLFWwindow* GetWindow()
	{
		return gWindow;
	}

	void OnResize(GLFWwindow* win, int w, int h)
	{
		WIDTH = w;
		HEIGHT = h;
		OGL::OnResize(w, h);
		if (ResizeHandler)
		{
			ResizeHandler(w, h);
		}
	}

	void InitWindow(int& outW, int& outH)
	{
		outW = WIDTH;
		outH = HEIGHT;

		// Setup Window
		if (!glfwInit())
			return;

		// GL 3.0 + GLSL 130
		const char* glsl_version = "#version 130";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

		// Create Window with graphics context
		gWindow = glfwCreateWindow(WIDTH, HEIGHT, "Engine", NULL, NULL);
		BAInput::Init(gWindow);
		if (gWindow == NULL)
			return;

		glfwMakeContextCurrent(gWindow);
		glfwSwapInterval(1); // Enable vsync

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			throw("unable to context to OpenGL");

		glfwSetFramebufferSizeCallback(gWindow, &OnResize);

		
		//SetWindowSize(gWindow, WIDTH, HEIGHT);
	}

	bool ShouldGameContinue() { return !glfwWindowShouldClose(gWindow); }
	void Cleanup() 
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

	}
	double GetTime() { return glfwGetTime() * 1000.0; }

	void SwapBuffers()
	{
		glfwSwapBuffers(gWindow);
	}

	void RegisterWindowResizeHandler(BackendWindowResizeFn fn)
	{
		ResizeHandler = fn;
	}

	int GetW()
	{
		return WIDTH;
	}

	int GetH()
	{
		return HEIGHT;
	}

	const char* GetAssetsFolderFullPath()
	{
#ifdef _WIN32
		static char buf[MAX_PATH];
		LPSTR filePart;
		GetFullPathNameA("./Assets", MAX_PATH, buf, &filePart);
		return buf;
#endif
	}

	const char* GetAssetFilePath(const char* assetFolderRelative)
	{
		static char buf[MAX_PATH];
		const char* assetsFolderPath = GetAssetsFolderFullPath();
		strcpy(buf, assetsFolderPath);
		size_t len = strlen(buf);
		buf[len++] = '\\';
		strcpy(&buf[len], assetFolderRelative);
		return buf;
	}


}
