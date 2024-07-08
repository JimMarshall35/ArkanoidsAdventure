#include "Pipeline.h"
#include "PipelineMeshData.h"

PipeLineStage::PipeLineStage(const PipelineStageInitArgs args)
	:Name(args.name),
	RenderTarget(args.hFrameBuffer),
	RequiredOutputBuffers(args.requiredRenderTargetBuffers),
	VertShaderCode(std::move(args.vertShaderCode)),
	FragShaderCode(std::move(args.fragShaderCode)),
	Semantics(args.Semantics)
{
}

void PipeLineStage::PushPerVertexAttribute(const PipelinePropertyName& prop)
{
	CheckNewProperty(prop);
	PerVertexAttributes.push_back(prop);
}

void PipeLineStage::PushPerInstanceAttribute(const PipelinePropertyName& prop)
{
	CheckNewProperty(prop);
	PerInstanceAttributes.push_back(prop);
}

void PipeLineStage::PushUniformAttribute(const PipelinePropertyName& prop)
{
	CheckNewProperty(prop);
	Uniforms.push_back(prop);
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

void PipeLineStage::CheckNewProperty(const PipelinePropertyName& newName)
{
	AreMultipleSemanticsSet(newName.Semantics, Errors, newName.Name);
	if (newName.Semantics < psCustomAttributesBegin && (Semantics & newName.Semantics))
	{
		PipelineError error{};
		error.Type = PipelineErrorType::DuplicateSemantic;
		//char buf[16];
		//itoa((int)newName.Semantics, buf, 10);
		error.Msg = newName.Name + EString(" is Duplicate Semantic : ");//) + EString(buf);
		Errors.push_back(error);
	}
}

bool PipeLineStage::IsMeshCompatible(const PipelineMeshData& meshData) const
{
	return IsMeshSemanticsCompatible(meshData.GetSemantics());
}

bool PipeLineStage::IsMeshSemanticsCompatible(PipelinePropertySemantics meshData) const
{
	// mesh is compatible if pipeline fulfils all its semantic requirements
	uint16_t r = (~Semantics) & meshData;
	return r == 0;

}
 