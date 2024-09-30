#pragma once
#include <array>
#include <bitset>
#include <glm/glm.hpp>
#include <stdint.h>
#include "EngineLib.h"
#include "PipelineTypes.h"
#include "InputFrontend.h"
#include "GizmoOperation.h"

class BackendAPI;
class PipeLine;
class PipelineMeshData;
struct TextureData;
struct BackendInputState;


enum class BackendErrorSeverity
{
	Info,
	Warning,
	Error,
	Unknown,
	Num
};

struct BackendLog
{
	EString Msg;
	BackendErrorSeverity Severity;
};

typedef BackendAPI (*BackEndAppFactory)(void);
typedef void(*BackendErrorHandlerFn)(const BackendLog& error);
typedef void(*BackendWindowResizeFn)(int newW, int newH);

typedef void(*PerDrawUniformSetterFn)(HPipeline pipeline, void* pUserData, int pipelineStage);
typedef void(*PerInstanceUniformSetterFn)(HDrawable drawable, HPipeline pipeline, int pipelineStage, void* pDrawableUserData, void* PipelineUserData);
typedef void(*PerInstanceAttributeSetterFn)(HDrawable drawable, HPipeline pipeline, int pipelineStage, void* pDrawableUserData, void* PipelineUserData, void* pInstanceDataToWrite);

typedef void                                   (*VoidFn)                              (void);
typedef bool                                   (*BoolFn)                              (void);
typedef double                                 (*GetTimeFn)                           (void);
typedef void                                   (*BoolArgFn)                           (bool);
typedef void                                   (*InitWindowFn)                        (int& outW, int& outH);
typedef void                                   (*PollInputFn)                         (BackendInputState&);
typedef void                                   (*RegisterWindowResizeHandlerFn)       (BackendWindowResizeFn fn);
typedef HPipeline                              (*UploadPipelineFn)                    (const PipeLine& p, void* pUserData);
typedef HMesh                                  (*UploadMeshFn)                        (const PipelineMeshData& mesh);
typedef HDrawable                              (*CreateDrawableFn)                    (HPipeline pipeline, HMesh meshe, void* pUserData);
typedef HPipeline                              (*GetDrawablePipelineFn)               (HDrawable drawable);
typedef void                                   (*RegisterErrorHandlerFn)              (BackendErrorHandlerFn);
typedef HPipeline                              (*ChangeDrawablePipelineFn)            (HDrawable mesh, HPipeline newPipeline); // returns old pipeline
typedef HPipelinePerInstanceProperty           (*GetPipelinePerInstancePropHFn)       (HPipeline, const EString& name);
typedef HPipelineUniformProperty               (*GetPipelineUniformPropertyHFn)       (HPipeline, const EString& name, int stage);
typedef void                                   (*SetPipelineUniform_Vec2Fn)           (HPipeline, HPipelineUniformProperty, int pipelineStage, const glm::vec2&);
typedef void                                   (*SetPipelineUniform_Vec3Fn)           (HPipeline, HPipelineUniformProperty, int pipelineStage, const glm::vec3&);
typedef void                                   (*SetPipelineUniform_Vec4Fn)           (HPipeline, HPipelineUniformProperty, int pipelineStage, const glm::vec4&);
typedef void                                   (*SetPipelineUniform_Mat4Fn)           (HPipeline, HPipelineUniformProperty, int pipelineStage, const glm::mat4&);
typedef void                                   (*SetPipelineUniform_FloatFn)          (HPipeline, HPipelineUniformProperty, int pipelineStage, float);
typedef void                                   (*SetPipelineUniform_UInt32Fn)         (HPipeline, HPipelineUniformProperty, int pipelineStage, uint32_t);
typedef void                                   (*SetPipelineUniform_TextureFn)        (HPipeline, HPipelineUniformProperty, int pipelineStage, HTexture);


typedef void                                   (*DrawDrawablesFn)                     (HDrawable* drawables, size_t numDrawables);
typedef void                                   (*DestroyDrawableFn)                   (HDrawable drawable);
typedef void                                   (*DestroyMeshFn)                       (HMesh drawable);
typedef void                                   (*DestroyPipelineFn)                   (HPipeline drawable);
typedef void                                   (*RegisterPerFrawUniformSetterFn)      (HPipeline, PerDrawUniformSetterFn);
typedef void                                   (*RegisterPerInstanceUniformSetterFn)  (HDrawable, PerInstanceUniformSetterFn);
typedef void                                   (*RegisterPerInstanceAttributeSetterFn)(HDrawable, PerInstanceAttributeSetterFn);
typedef void                                   (*SetDrawInstanced)                    (HDrawable, bool bDrawInstanced);
typedef HTexture                               (*UploadTextureFn)                     (const TextureData& data);
typedef void                                   (*DestroyTextureFn)                    (HTexture tex);
typedef void*                                  (*GetDrawableUserDataFn)               (HDrawable drawable);
typedef void*                                  (*GetPipelineUserDataFn)               (HDrawable drawable);

typedef int                                    (*GetCodeForAsciiCharFn)               (char);               // get the platform specific code for the keyboard key of an ascii char
typedef int                                    (*GetCodeForMouseBtnFn)                (MouseButtons);        // get platform specific code for mouse btn

typedef void                                   (*SetCursorModeFn)                     (In::CursorMode);

typedef void                                   (*SetGizmoFn)                          (glm::mat4* pM, glm::mat4* pV, glm::mat4* pP);
typedef void                                   (*ClearGizmoFn)                        (void);
typedef void                                   (*SetGizmoOperationFn)                 (GizmoOperation op);

struct BackendAPI 
{
	// window, input, ect
	InitWindowFn InitWindow = nullptr;
	VoidFn InitRenderer = nullptr;
	BoolFn ShouldGameContinue = nullptr;
	VoidFn Cleanup = nullptr;
	GetTimeFn GetTime = nullptr;
	PollInputFn PollInput = nullptr;
	GetCodeForAsciiCharFn GetInputCodeForAscii = nullptr;
	GetCodeForMouseBtnFn GetInputCodeForMouseBtn = nullptr;
	SetCursorModeFn SetCursorMode = nullptr;

	RegisterWindowResizeHandlerFn RegisterResize = nullptr;

	// error handling
	RegisterErrorHandlerFn RegisterErrorHandler = nullptr;

	// graphics
	UploadPipelineFn UploadPipeline = nullptr;
	UploadMeshFn UploadMesh = nullptr;
	CreateDrawableFn CreateDrawable = nullptr;
	GetDrawablePipelineFn GetDrawablePipeline = nullptr;
	ChangeDrawablePipelineFn ChangeMeshPipelineFn = nullptr;
	GetPipelineUniformPropertyHFn GetPipelineUniformPropertyH = nullptr;

	SetPipelineUniform_Vec2Fn SetPipelineUniform_Vec2 = nullptr;
	SetPipelineUniform_Vec3Fn SetPipelineUniform_Vec3 = nullptr;
	SetPipelineUniform_Vec4Fn SetPipelineUniform_Vec4 = nullptr;
	SetPipelineUniform_Mat4Fn SetPipelineUniform_Mat4 = nullptr;
	SetPipelineUniform_FloatFn SetPipelineUniform_Float = nullptr;
	SetPipelineUniform_UInt32Fn SetPipelineUniform_UInt32 = nullptr;
	SetPipelineUniform_TextureFn SetPipelineUniform_Texture = nullptr;

	DrawDrawablesFn DrawDrawables = nullptr;

	DestroyDrawableFn DestroyDrawable = nullptr;
	DestroyMeshFn DestroyMesh = nullptr;
	DestroyPipelineFn DestroyPipeline = nullptr;
	VoidFn PreRender = nullptr;
	VoidFn PostRender = nullptr;

	RegisterPerFrawUniformSetterFn RegisterPerDrawUniformSetter = nullptr;
	RegisterPerInstanceUniformSetterFn RegisterPerInstanceUniformSetter = nullptr;
	RegisterPerInstanceAttributeSetterFn RegisterPerInstanceAttributeSetter = nullptr;

	SetDrawInstanced SetDrawInstanced = nullptr;

	UploadTextureFn UploadTexture = nullptr;
	DestroyTextureFn DestroyTexture = nullptr; 

	GetDrawableUserDataFn GetDrawableUserData = nullptr;
	GetPipelineUserDataFn GetPipelineUserData = nullptr;

	SetGizmoFn SetGizmo = nullptr;
	ClearGizmoFn ClearGizmo = nullptr;
	SetGizmoOperationFn SetGizmoOperation = nullptr;
};
