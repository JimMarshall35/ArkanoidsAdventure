#include "Exports.h"
#include "Pipeline.h"
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

//static const uint32_t WIDTH = 800;
//static const uint32_t HEIGHT = 600;
//
//static GLFWwindow* gWindow = nullptr;
//void InitWindow()
//{
//	glfwInit();
//
//	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
//	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
//
//	gWindow = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
//	//glfwSetWindowUserPointer(gWindow, this);
//	//glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
//}
//
//void InitRenderer(){}
//bool ShouldGameContinue() { return !glfwWindowShouldClose(gWindow); }
//void Cleanup(){}
//double GetTime() { return glfwGetTime(); }
//void PollInput(BackendInputState& s)
//{
//	glfwPollEvents();
//}
//void RegisterErrorHandler(BackendErrorHandlerFn handler){}
//HPipeline UploadPipeline(const PipeLine& p, void* pUserData){ return ENGINE_NULL_HANDLE; }
//HMesh UploadMesh(HPipeline hPipeline, const PipelineMeshData& mesh){ return ENGINE_NULL_HANDLE; }
//HDrawable CreateDrawable(HPipeline pipeline, HMesh mesh, void* pUserData) { return ENGINE_NULL_HANDLE; }
//HPipeline GetDrawablePipeline(HDrawable drawable) { return ENGINE_NULL_HANDLE; }
//HPipeline ChangeDrawablePipeline(HDrawable d, HPipeline newPipeline) { return ENGINE_NULL_HANDLE; }
//HPipelinePerInstanceProperty GetPipelinePerInstancePropertyH(HPipeline p, const EString& name){ return ENGINE_NULL_HANDLE; }
//HPipelineUniformProperty GetPipelineUniformPropertyH(HPipeline p, const EString& name){ return ENGINE_NULL_HANDLE; }
//void SetPipelineUniform_Vec2(HPipelineUniformProperty hProp, const glm::vec2& v){}
//void SetPipelineUniform_Vec3(HPipelineUniformProperty hProp, const glm::vec3& v){}
//void SetPipelineUniform_Vec4(HPipelineUniformProperty hProp, const glm::vec4& v){}
//void SetPipelineUniform_Mat4(HPipelineUniformProperty hProp, const glm::mat4& v){}
//void SetPipelineUniform_Float(HPipelineUniformProperty hProp, float v) {}
//void SetPipelineUniform_UInt32(HPipelineUniformProperty hProp, uint32_t v) {}
//void SetPipelineUniform_Texture(HPipelineUniformProperty hProp, HTexture v) {}
//void SetPipelinePerInstance_Vec2(HPipelinePerInstanceProperty hProp, const glm::vec2& v) {}
//void SetPipelinePerInstance_Vec3(HPipelinePerInstanceProperty hProp, const glm::vec3& v) {}
//void SetPipelinePerInstance_Vec4(HPipelinePerInstanceProperty hProp, const glm::vec4& v) {}
//void SetPipelinePerInstance_Mat4(HPipelinePerInstanceProperty hProp, const glm::mat4& v) {}
//void SetPipelinePerInstance_Float(HPipelinePerInstanceProperty hProp, float v) {}
//void SetPipelinePerInstance_UInt32(HPipelinePerInstanceProperty hProp, uint32_t v) {}
//void SetPipelinePerInstance_Texture(HPipelinePerInstanceProperty hProp, HTexture v) {}
//void DrawDrawables(HDrawable* drawables, size_t numDrawables) {}
//void DestroyDrawable(HDrawable d){}
//void DestroyMesh(HMesh m){}
//void DestroyPipeline(HPipeline p){}
//
//void PreRender() {}
//void PostRender(){}
//
//void RegisterPerDrawUniformSetter(HDrawable hDrawable, PerDrawUniformSetterFn fn){}
//void RegisterPerInstanceUniformSetter(HDrawable hDrawable, PerInstanceUniformSetterFn fn) {}
//void RegisterPerInstanceAttributeSetter(HDrawable hDrawable, PerInstanceAttributeSetterFn fn) {}
//void SetDrawInstanced(HDrawable hDrawable, bool bInstanced){}

extern "C"
{
	BackendAPI GetApp()
	{
		return BackendAPI{};
		/*return BackendAPI{
			&InitWindow,
			&InitRenderer,
			&ShouldGameContinue,
			&Cleanup,
			&GetTime,
			&PollInput,
			&RegisterErrorHandler,
			&UploadPipeline,
			&UploadMesh,
			&CreateDrawable,
			&GetDrawablePipeline,
			&ChangeDrawablePipeline,
			&GetPipelinePerInstancePropertyH,
			&GetPipelineUniformPropertyH,
			&SetPipelineUniform_Vec2,
			&SetPipelineUniform_Vec3,
			&SetPipelineUniform_Vec4,
			&SetPipelineUniform_Mat4,
			&SetPipelineUniform_Float,
			&SetPipelineUniform_UInt32,
			&SetPipelineUniform_Texture,
			&SetPipelinePerInstance_Vec2,
			&SetPipelinePerInstance_Vec3,
			&SetPipelinePerInstance_Vec4,
			&SetPipelinePerInstance_Mat4,
			&SetPipelinePerInstance_Float,
			&SetPipelinePerInstance_UInt32,
			&SetPipelinePerInstance_Texture,
			&DrawDrawables,
			&DestroyDrawable,
			&DestroyMesh,
			&DestroyPipeline,
			&PreRender,
            &PostRender,
			&RegisterPerDrawUniformSetter,
			&RegisterPerInstanceUniformSetter,
			&RegisterPerInstanceAttributeSetter,
			&SetDrawInstanced,
		};*/
	}
}