#pragma once
#include "EngineDLLInterface.h"
#include "EngineLib.h"
#include "PipelineTypes.h"
#include <glm/glm.hpp>
#include <type_traits>


#define MAX_SEMANTIC_BITSHIFT_VAL 13

class PipelineMeshData;

struct PipelinePropertyName
{
	PipelinePropertyType PropType;
	EString Name;
	PipelinePropertySemantics Semantics;
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

class ENGINE_FRONTEND_API PipeLine
{
public:
	void SetShaderCode(EVec<char>&& vertShaderCode, EVec<char>&& fragShaderCode);
	void SetShaderCode(EVec<char>& vertShaderCode, EVec<char>& fragShaderCode);
	void PushPerVertexAttribute(const PipelinePropertyName& prop);
	void PushPerInstanceAttribute(const PipelinePropertyName& prop);
	void PushPerInstanceUniformAttribute(const PipelinePropertyName& prop);
	bool HasErrors() const { return Errors.size() != 0; };
	void CheckNewProperty(const PipelinePropertyName& newSeman);
	bool IsMeshCompatible(const PipelineMeshData& meshData) const;
	const EVec<char>& GetVertShader() { return VertShaderCode; }
	const EVec<char>& GetFragShader() { return FragShaderCode; }
	const EVec<const PipelinePropertyName>& GetVertexAttributes() {return PerVertexAttributes;}
	const EVec<const PipelinePropertyName>& GetPerVertexAttributes() { return PerVertexAttributes; }
	const EVec<const PipelinePropertyName>& GetPerInstanceUniformAttributes() { return PerInstanceUniformAttributes; }
	std::underlying_type_t<PipelinePropertySemantics> GetSemantics() { return Semantics; }
private:
	EVec<PipelineError> Errors;
	EVec<char> VertShaderCode;
	EVec<char> FragShaderCode;
	EVec<const PipelinePropertyName> PerVertexAttributes;
	EVec<const PipelinePropertyName> PerInstanceAttributes;
	EVec<const PipelinePropertyName> PerInstanceUniformAttributes;
	std::underlying_type_t<PipelinePropertySemantics> Semantics = 0;
};