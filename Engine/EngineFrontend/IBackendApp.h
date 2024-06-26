#pragma once
#include <bitset>
#include <glm/glm.hpp>
#include <stdint.h>
#include "EngineLib.h"
#include "PipelineTypes.h"

class BackendAPI;
class PipeLine;
class PipelineMeshData;

#define ENGINE_NULL_HANDLE 0
typedef uint32_t HDrawable;
typedef uint32_t HMesh;

typedef uint32_t HPipeline;
typedef uint32_t HPipelineUniformProperty;
typedef uint32_t HPipelinePerInstanceProperty;
typedef uint32_t HTexture;

struct BackendInputState
{
	std::bitset<256> KeyboardState;
	std::bitset<3> MouseButtonState;
	glm::vec2 MousePosition;
	int MouseWheelState;
};

enum class BackendErrorSeverity
{
	Info,
	Warning,
	Error
};

struct BackendError
{
	EString Msg;
	BackendErrorSeverity Severity;
};

typedef BackendAPI (*BackEndAppFactory)(void);
typedef void(*BackendErrorHandlerFn)(BackendError error);

typedef void                                   (*VoidFn)                           (void);
typedef bool                                   (*BoolFn)                           (void);
typedef double                                 (*GetTimeFn)                        (void);

typedef void                                   (*PollInputFn)                      (BackendInputState&);
typedef HPipeline                              (*UploadPipelineFn)                 (const PipeLine& p);
typedef HMesh                                  (*UploadMeshFn)                     (HPipeline pipeline, const PipelineMeshData& mesh);
typedef HDrawable                              (*CreateDrawableFn)                   (HPipeline pipeline, HMesh mesh);
typedef void                                   (*RegisterErrorHandlerFn)           (BackendErrorHandlerFn);
typedef HPipeline                              (*ChangeDrawablePipelineFn)         (HDrawable mesh, HPipeline newPipeline); // returns old pipeline
typedef HPipelinePerInstanceProperty           (*GetPipelinePerInstancePropHFn)    (HPipeline, const EString& name);
typedef HPipelineUniformProperty               (*GetPipelineUniformPropertyHFn)    (HPipeline, const EString& name);
typedef void                                   (*SetPipelineUniform_Vec2Fn)        (HPipelineUniformProperty, const glm::vec2&);
typedef void                                   (*SetPipelineUniform_Vec3Fn)        (HPipelineUniformProperty, const glm::vec3&);
typedef void                                   (*SetPipelineUniform_Vec4Fn)        (HPipelineUniformProperty, const glm::vec4&);
typedef void                                   (*SetPipelineUniform_Mat4Fn)        (HPipelineUniformProperty, const glm::mat4&);
typedef void                                   (*SetPipelineUniform_FloatFn)       (HPipelineUniformProperty, float);
typedef void                                   (*SetPipelineUniform_UInt32Fn)      (HPipelineUniformProperty, uint32_t);
typedef void                                   (*SetPipelineUniform_TextureFn)     (HPipelineUniformProperty, HTexture);
typedef void                                   (*SetPipelinePerInstance_Vec2Fn)    (HPipelinePerInstanceProperty, const glm::vec2&);
typedef void                                   (*SetPipelinePerInstance_Vec3Fn)    (HPipelinePerInstanceProperty, const glm::vec3&);
typedef void                                   (*SetPipelinePerInstance_Vec4Fn)    (HPipelinePerInstanceProperty, const glm::vec4&);
typedef void                                   (*SetPipelinePerInstance_Mat4Fn)    (HPipelinePerInstanceProperty, const glm::mat4&);
typedef void                                   (*SetPipelinePerInstance_FloatFn)   (HPipelinePerInstanceProperty, float);
typedef void                                   (*SetPipelinePerInstance_UInt32Fn)  (HPipelinePerInstanceProperty, uint32_t);
typedef void                                   (*SetPipelinePerInstance_TextureFn) (HPipelinePerInstanceProperty, HTexture);
typedef void                                   (*DrawDrawablesFn)                  (HDrawable* drawables, size_t numDrawables);
typedef void                                   (*DestroyDrawableFn)                (HDrawable drawable);
typedef void                                   (*DestroyMeshFn)                    (HMesh drawable);
typedef void                                   (*DestroyPipelineFn)                (HPipeline drawable);

struct BackendAPI 
{
	// window, input, ect
	VoidFn InitWindow = nullptr;
	VoidFn InitRenderer = nullptr;
	BoolFn ShouldGameContinue = nullptr;
	VoidFn Cleanup = nullptr;
	GetTimeFn GetTime = nullptr;
	PollInputFn PollInput = nullptr;

	// error handling
	RegisterErrorHandlerFn RegisterErrorHandler = nullptr;

	// graphics
	UploadPipelineFn UploadPipeline = nullptr;
	UploadMeshFn UploadMesh = nullptr;
	CreateDrawableFn CreateDrawable = nullptr;
	ChangeDrawablePipelineFn ChangeDrawablePipeline = nullptr;
	GetPipelinePerInstancePropHFn GetPipelinePerInstancePropertyH = nullptr;
	GetPipelineUniformPropertyHFn GetPipelineUniformPropertyH = nullptr;

	SetPipelineUniform_Vec2Fn SetPipelineUniform_Vec2 = nullptr;
	SetPipelineUniform_Vec3Fn SetPipelineUniform_Vec3 = nullptr;
	SetPipelineUniform_Vec4Fn SetPipelineUniform_Vec4 = nullptr;
	SetPipelineUniform_Mat4Fn SetPipelineUniform_Mat4 = nullptr;
	SetPipelineUniform_FloatFn SetPipelineUniform_Float = nullptr;
	SetPipelineUniform_UInt32Fn SetPipelineUniform_UInt32 = nullptr;
	SetPipelineUniform_TextureFn SetPipelineUniform_Texture = nullptr;

	SetPipelinePerInstance_Vec2Fn SetPipelinePerInstance_Vec2 = nullptr;
	SetPipelinePerInstance_Vec3Fn SetPipelinePerInstance_Vec3 = nullptr;
	SetPipelinePerInstance_Vec4Fn SetPipelinePerInstance_Vec4 = nullptr;
	SetPipelinePerInstance_Mat4Fn SetPipelinePerInstance_Mat4 = nullptr;
	SetPipelinePerInstance_FloatFn SetPipelinePerInstance_Float = nullptr;
	SetPipelinePerInstance_UInt32Fn SetPipelinePerInstance_UInt32 = nullptr;
	SetPipelinePerInstance_TextureFn SetPipelinePerInstance_Texture = nullptr;
	DrawDrawablesFn DrawDrawables = nullptr;

	DestroyDrawableFn DestroyDrawable = nullptr;
	DestroyMeshFn DestroyMesh = nullptr;
	DestroyPipelineFn DestroyPipeline = nullptr;
	VoidFn PreRender = nullptr;
	VoidFn PostRender = nullptr;
};
