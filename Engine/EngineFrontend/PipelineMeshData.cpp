#include "PipelineMeshData.h"
#include <algorithm>


void PipelineMeshBuffer::SetData(const EVec<glm::vec2>& data, PipelinePropertySemantics semantics)
{
	static const EString name = "vec2";
	EVec<PipelinePropertySemantics> allowed
	{
		psPerVertexUV,
		psUniformCustom,
		psPerInstanceCustomVal,
		psPerVertexCustomVal
	};
	SetBufferDataInternal(data, semantics, name, PipelinePropertyType::Vec2, allowed);
}

void PipelineMeshBuffer::SetData(const EVec<unsigned int>& data, PipelinePropertySemantics semantics)
{
	static const EString name = "uint32";
	EVec<PipelinePropertySemantics> allowed
	{
		psIndices,
		psUniformColourTexture,
		psUniformCustom,
		psPerInstanceCustomVal,
		psPerVertexCustomVal
	};
	SetBufferDataInternal(data, semantics, name, PipelinePropertyType::UInt32, allowed);
}

void PipelineMeshBuffer::SetData(const EVec<glm::vec3>& data, PipelinePropertySemantics semantics)
{
	static const EString name = "vec3";
	EVec<PipelinePropertySemantics> allowed
	{
		psPerInstanceCustomVal,
		psUniformCustom,
		psPerVertexCustomVal,
		psPerVertexColour,
		psPerVertexPos,
		psPerVertexNorm,
		psPerVertexUV
	};
	SetBufferDataInternal(data, semantics, name, PipelinePropertyType::Vec3, allowed);
}


void PipelineMeshBuffer::SetData(const EVec<glm::vec4>& data, PipelinePropertySemantics semantics)
{
	static const EString name = "vec4";
	EVec<PipelinePropertySemantics> allowed
	{
		psPerInstanceCustomVal,
		psUniformCustom,
		psPerVertexCustomVal,
		psPerVertexColour,
		psPerVertexPos,
		psPerVertexNorm,
		psPerVertexUV
	};
	SetBufferDataInternal(data, semantics, name, PipelinePropertyType::Vec4, allowed);
}

void PipelineMeshBuffer::SetData(const EVec<glm::mat4>& data, PipelinePropertySemantics semantics)
{
	static const EString name = "mat4";
	EVec<PipelinePropertySemantics> allowed
	{
		psPerInstanceCustomVal,
		psUniformCustom,
		psPerVertexCustomVal,
		psUniformModelMatrix,
		psUnifomViewMatrix,
		psUniformProjectionMatrix,
		psPerInstanceModelMatrix
	};
	SetBufferDataInternal(data, semantics, name, PipelinePropertyType::Mat4, allowed);
}

void PipelineMeshBuffer::SetData(const EVec<float>& data, PipelinePropertySemantics semantics)
{
	static const EString name = "float";
	EVec<PipelinePropertySemantics> allowed
	{
		psUniformCustom,
		psPerInstanceCustomVal,
		psPerVertexCustomVal
	};
	SetBufferDataInternal(data, semantics, name, PipelinePropertyType::Float, allowed);
}

size_t PipelineMeshBuffer::GetElementCount() const
{
#define A(T) std::get<EVec<T>>(Data).size()
	switch (Type)
	{
	case PipelinePropertyType::Unknwon:
	default:
		assert(false);
		break;
	case PipelinePropertyType::Vec2: return A(glm::vec2);
	case PipelinePropertyType::Vec3: return A(glm::vec3);
	case PipelinePropertyType::Vec4: return A(glm::vec4);
	case PipelinePropertyType::Mat4: return A(glm::mat4);
	case PipelinePropertyType::Float: return A(float);
	case PipelinePropertyType::UInt32: return A(uint32_t);
	case PipelinePropertyType::Texture: return A(uint32_t);
	}
#undef A
	return 0;
}

const void* PipelineMeshBuffer::GetStart() const
{
#define A(T) (const void*)std::get<EVec<T>>(Data).data()
	switch (Type)
	{
	case PipelinePropertyType::Unknwon:
	default:
		/*Errors.push_back(PipelineError{
				PipelineErrorType::UnknownPropertyType,
				"[PipelineMeshBuffer::GetStart]",
			});*/
		assert(false);
		break;
	case PipelinePropertyType::Vec2: return A(glm::vec2);
	case PipelinePropertyType::Vec3: return A(glm::vec3);
	case PipelinePropertyType::Vec4: return A(glm::vec4);
	case PipelinePropertyType::Mat4: return A(glm::mat4);
	case PipelinePropertyType::Float: return A(float);
	case PipelinePropertyType::UInt32: return A(uint32_t);
	case PipelinePropertyType::Texture: return A(uint32_t);
	}
#undef A
	return nullptr;
}

bool PipelineMeshBuffer::CheckBufferDataSetRequest(const EVec<PipelinePropertySemantics>& allowedSemantics, PipelinePropertySemantics semanticsSet)
{
	int numSet = NumSemanticsSet(semanticsSet);
	bool allowed = true;
	if (numSet != 1)
	{
		PipelineError e{
			PipelineErrorType::NotExactlyOneSemanticBitSet,
			e.Msg = "[PipelineMeshBuffer::CheckBufferDataSetRequest] " + std::to_string(numSet) + EString(" Semantics set. ")
		};
		Errors.push_back(e);
		allowed = false;
	}
	if (Semantics != psUnknown)
	{
		PipelineError e{
			PipelineErrorType::NotExactlyOneSemanticBitSet,
			"[PipelineMeshBuffer::CheckBufferDataSetRequest] already recieved data"
		};
		Errors.push_back(e);
		allowed = false;
	}
	if (std::find(allowedSemantics.begin(), allowedSemantics.end(), semanticsSet) == allowedSemantics.end())
	{
		PipelineError e{
			PipelineErrorType::InvalidSemanticsForBufferType,
			"[PipelineMeshBuffer::CheckBufferDataSetRequest] Invalid Data Type for " + GetPipelineSemName(semanticsSet)
		};
		Errors.push_back(e);
		allowed = false;
	}
	
	return allowed;
}

PipelineMeshData::PipelineMeshData(const char* name)
	:Name(name)
{
}

void PipelineMeshData::TryAddBuffer(const PipelineMeshBuffer& buffer)
{
	if (!buffer.Valid())
	{
		for (const PipelineError& e : buffer.GetErrors())
		{
			Errors.push_back(e);
		}
		return;
	}
	Buffers.push_back(buffer);
}

PipelinePropertySemantics PipelineMeshData::GetSemantics() const
{
	uint16_t s = 0;
	for (const PipelineMeshBuffer& buffer : Buffers)
	{
		s |= (uint16_t)buffer.GetSemantics();
	}
	return PipelinePropertySemantics(s);
}

