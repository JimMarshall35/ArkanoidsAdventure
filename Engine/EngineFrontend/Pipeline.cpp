#include "Pipeline.h"
#include "PipelineMeshData.h"

void PipeLine::SetShaderCode(EVec<char>&& vertShaderCode, EVec<char>&& fragShaderCode)
{
	VertShaderCode = vertShaderCode;
	FragShaderCode = fragShaderCode;
}

void PipeLine::SetShaderCode(EVec<char>& vertShaderCode, EVec<char>& fragShaderCode)
{
	VertShaderCode = vertShaderCode;
	FragShaderCode = fragShaderCode;
}

void PipeLine::PushPerVertexAttribute(const PipelinePropertyName& prop)
{
	PerVertexAttributes.push_back(prop);
}

void PipeLine::PushPerInstanceAttribute(const PipelinePropertyName& prop)
{
	PerInstanceAttributes.push_back(prop);
}

void PipeLine::PushPerInstanceUniformAttribute(const PipelinePropertyName& prop)
{
	PerInstanceUniformAttributes.push_back(prop);
}

bool AreMultipleSemanticsSet(uint16_t val, EVec<PipelineError>& errorOut, const EString& name)
{
	int matches = NumSemanticsSet((PipelinePropertySemantics)val);
	if (matches != 1)
	{
		errorOut.push_back({ PipelineErrorType::NotExactlyOneSemanticBitSet, EString("Not Exactly One Semantic Bit Set, property ") + name });
	}
	return matches != 1;
}

void PipeLine::CheckNewProperty(const PipelinePropertyName& newName)
{
	AreMultipleSemanticsSet(newName.Semantics, Errors, newName.Name);
	if (Semantics & newName.Semantics)
	{
		PipelineError error{};
		error.Type = PipelineErrorType::DuplicateSemantic;
		//char buf[16];
		//itoa((int)newName.Semantics, buf, 10);
		error.Msg = newName.Name + EString(" is Duplicate Semantic : ");//) + EString(buf);
		Errors.push_back(error);
	}
}

bool PipeLine::IsMeshCompatible(const PipelineMeshData& meshData) const
{
	// mesh is compatible if pipeline fulfils all its semantic requirements
	uint16_t mesh = (int16_t)meshData.GetSemantics();
	uint16_t pipeline = (int16_t)Semantics;
	uint16_t r = (~pipeline) & mesh;
	return r == 0; 
}
 