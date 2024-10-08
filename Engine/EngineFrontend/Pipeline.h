#pragma once
#include "EngineDLLInterface.h"
#include "EngineLib.h"
#include "PipelineTypes.h"
#include "Autolist.h"
#include <glm/glm.hpp>
#include <type_traits>
#include <numeric>

#define MAX_SEMANTIC_BITSHIFT_VAL 13
#define H_MAINWINDOW_FRAMEBUFFER std::numeric_limits<size_t>::max()


class PipelineMeshData;


struct PipelinePropertyName
{
	PipelinePropertyType PropType;
	EString Name;
	PipelinePropertySemantics Semantics;
	int location;
};

struct PipelinePropertyVal
{
	PipelinePropertyType PropType;
	union
	{
		glm::vec2 v2;
		glm::vec3 v3;
		glm::vec4 v4;
		glm::mat4 m4;
		float f;
		uint32_t u;
	} Val;

};

struct PipelineStageInitArgs
{
	const char* name = "Nord Stream";
	HFrameBuffer hFrameBuffer = H_MAINWINDOW_FRAMEBUFFER;
	RenderTargetBufType requiredRenderTargetBuffers = (RenderTargetBufType)0;
	EVec<char> vertShaderCode;
	EVec<char> fragShaderCode;
	std::underlying_type_t<PipelinePropertySemantics> Semantics;
};

class ENGINE_FRONTEND_API PipeLineStage
{
public:
	PipeLineStage(const PipelineStageInitArgs args = {});
	void PushPerVertexAttribute(const PipelinePropertyName& prop);
	void PushPerInstanceAttribute(const PipelinePropertyName& prop);
	void PushUniformAttribute(const PipelinePropertyName& prop);
	bool HasErrors() const { return Errors.size() != 0; };
	void CheckNewProperty(const PipelinePropertyName& newSeman);
	bool IsMeshCompatible(const PipelineMeshData& meshData) const;
	bool IsMeshSemanticsCompatible(PipelinePropertySemantics meshData) const;

	const EVec<char>& GetVertShader() const { return VertShaderCode; }
	const EVec<char>& GetFragShader() const { return FragShaderCode; }
	const EVec<PipelinePropertyName>& GetVertexAttributes() const {return PerVertexAttributes;}
	const EVec<PipelinePropertyName>& GetPerInstanceAttributes() const { return PerInstanceAttributes; }
	const EVec<PipelinePropertyName>& GetUniformAttributes() const { return Uniforms; }
	std::underlying_type_t<PipelinePropertySemantics> GetSemantics() const { return Semantics; }
	const char* GetName() const { return Name.c_str(); }

private:
	EVec<PipelineError> Errors;
	EVec<char> VertShaderCode;
	EVec<char> FragShaderCode;
	EVec<PipelinePropertyName> PerVertexAttributes;
	EVec<PipelinePropertyName> PerInstanceAttributes;
	EVec<PipelinePropertyName> Uniforms;
	std::underlying_type_t<PipelinePropertySemantics> Semantics = 0;
	EString Name;

	HFrameBuffer RenderTarget = H_MAINWINDOW_FRAMEBUFFER;
	RenderTargetBufType RequiredOutputBuffers;

};



class ENGINE_FRONTEND_API PipeLine : public Autolist<PipeLine>
{
public:
	PipeLine(const char* name);
	void PushStage(PipeLineStage& stage) { Stages.push_back(stage); }
	void PushStage(PipeLineStage&& stage) { Stages.push_back(stage); }
	const EVec<PipeLineStage>& GetStages() const { return Stages; }
	const char* GetName() { return Name.c_str(); }
	virtual void Create();
	HPipelineUniformProperty TryGetUniformProperty(const char* name) const;
	HDrawable GetDrawable(HMesh mesh, Entity e);
	HPipeline GetH() { return PipelineHandle; }
	static PipeLine* TryGetPipeline(const char* name);
	static PipeLine* TryGetPipeline(HPipeline h);

	static void IteratePipelines(bool(*callBack)(PipeLine* pn, void* pUser), void* pUser);
	static int NumPipelines();
private:

	static void PerDrawUnifomSetter(HPipeline pipeline, void* pUserData, int pipelineStage);
	static void PerInstanceUniformSetter(HDrawable drawable, HPipeline pipeline, int pipelineStage, void* pDrawableUserData, void* PipelineUserData);
protected:
	virtual void PerDrawUniform(int pipelineStage) = 0;
	virtual void PerInstanceUniform(int pipelineStage, HDrawable drawable, Entity e) = 0;
protected:
	EVec<PipeLineStage> Stages;
	EString Name;
	EMap<EString, HPipelineUniformProperty> UniformPropertyHandlesByName;
	HPipeline PipelineHandle;
};


