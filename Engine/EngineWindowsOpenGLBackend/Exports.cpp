#include "Exports.h"
#include "Pipeline.h"
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "OGLRenderer.h"
#include "Error.h"
#include "Sys.h"
#include "Input.h"

extern "C"
{
	BackendAPI GetApp()
	{
		return BackendAPI{
			&Sys::InitWindow,
			&OGL::InitRenderer,
			&Sys::ShouldGameContinue,
			&Sys::Cleanup,
			&Sys::GetTime,
			&BAInput::PollInput,
			&BAInput::GetInputCodeForAscii,
			&BAInput::GetInputCodeForMouseBtn,
			&BAInput::SetCursorMode,
			&Sys::RegisterWindowResizeHandler,
			&Log::RegisterErrorHandler,
			&OGL::UploadPipeline,
			&OGL::UploadMesh,
			&OGL::CreateDrawable,
			&OGL::GetDrawablePipeline,
			&OGL::ChangeDrawablePipeline,
			&OGL::GetPipelineUniformPropertyH,
			&OGL::SetPipelineUniform_Vec2,
			&OGL::SetPipelineUniform_Vec3,
			&OGL::SetPipelineUniform_Vec4,
			&OGL::SetPipelineUniform_Mat4,
			&OGL::SetPipelineUniform_Float,
			&OGL::SetPipelineUniform_UInt32,
			&OGL::SetPipelineUniform_Texture,
			&OGL::DrawDrawables,
			&OGL::DestroyDrawable,
			&OGL::DestroyMesh,
			&OGL::DestroyPipeline,
			&OGL::PreRender,
			&OGL::PostRender,
			&OGL::RegisterPerDrawUniformSetter,
			&OGL::RegisterPerInstanceUniformSetter,
			&OGL::RegisterPerInstanceAttributeSetter,
			&OGL::SetDrawInstanced,
			&OGL::UploadTexture,
			&OGL::DestroyTexture,
			&OGL::GetDrawableUserData,
			&OGL::GetPipelineUserData
		};
	}
}