#pragma once
#include "IBackendApp.h"
class Pipeline;
namespace OGL
{
	HPipeline UploadPipeline(const PipeLine& p, void* pUserData);
	HMesh UploadMesh(const PipelineMeshData& mesh);
	HPipeline GetDrawablePipeline(HDrawable drawable);
	HDrawable CreateDrawable(HPipeline pipeline, HMesh mesh, void* pUserData);
	HPipeline ChangeDrawablePipeline(HDrawable d, HPipeline newPipeline);

	void DrawDrawables(HDrawable* drawables, size_t numDrawables);
	void DestroyDrawable(HDrawable d);
	void DestroyMesh(HMesh m);
	void DestroyPipeline(HPipeline p);
	void InitRenderer();
	void PreRender();
	void PostRender();

	HPipelineUniformProperty GetPipelineUniformPropertyH(HPipeline p, const EString& name, int stage);
	void SetPipelineUniform_Vec2(HPipeline pipeline, HPipelineUniformProperty hProp, int pipelineStage, const glm::vec2& v);
	void SetPipelineUniform_Vec3(HPipeline pipeline, HPipelineUniformProperty hProp, int pipelineStage, const glm::vec3& v);
	void SetPipelineUniform_Vec4(HPipeline pipeline, HPipelineUniformProperty hProp, int pipelineStage, const glm::vec4& v);
	void SetPipelineUniform_Mat4(HPipeline pipeline, HPipelineUniformProperty hProp, int pipelineStage, const glm::mat4& v);
	void SetPipelineUniform_Float(HPipeline pipeline, HPipelineUniformProperty hProp, int pipelineStage, float v);
	void SetPipelineUniform_UInt32(HPipeline pipeline, HPipelineUniformProperty hProp, int pipelineStage, uint32_t v);
	void SetPipelineUniform_Texture(HPipeline pipeline, HPipelineUniformProperty hProp, int pipelineStage, HTexture v);

	void RegisterPerDrawUniformSetter(HPipeline hPipe, PerDrawUniformSetterFn fn);
	void RegisterPerInstanceUniformSetter(HDrawable hDrawable, PerInstanceUniformSetterFn fn);
	void RegisterPerInstanceAttributeSetter(HDrawable hDrawable, PerInstanceAttributeSetterFn fn);
	void SetDrawInstanced(HDrawable hDrawable, bool bInstanced);
	HTexture UploadTexture(const TextureData& data);
	void DestroyTexture(HTexture tex);

	void* GetDrawableUserData(HDrawable drawable);
	void* GetPipelineUserData(HPipeline pipeline);

	/* INTERNAL */
	void OnResize(int w, int h);
}