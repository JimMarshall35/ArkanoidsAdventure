#include "PipelineMeshData.h"
#include <algorithm>
#include "FrontendError.h"
#include "IArchive.h"

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

void SerializeBufferData(IArchive& ar, PipelineMeshBuffer& buf)
{
	if (ar.IsStoring())
	{
		ar.PushObj("Data");
			ar << buf.GetSizeInElements();
			switch (buf.GetType())
			{
			case PipelinePropertyType::Float:
				for (float f : buf.GetData_f32())
				{
					ar.PushObj("Data");
						ar << f;
					ar.PopObj();
				}
				break;
			case PipelinePropertyType::Mat4:
				for (const glm::mat4x4& f : buf.GetData_M4())
				{
					ar.PushObj("Data");
						ar << f;
					ar.PopObj();
				}
				break;
			case PipelinePropertyType::UInt32:
				for (u32 f : buf.GetData_u32())
				{
					ar.PushObj("Data");
					ar << f;
					ar.PopObj();
				}
				break;
			case PipelinePropertyType::Vec2:
				for (const glm::vec2& f : buf.GetData_V2())
				{
					ar.PushObj("Data");
					ar << f;
					ar.PopObj();
				}
				break;
			case PipelinePropertyType::Vec3:
				for (const glm::vec3& f : buf.GetData_V3())
				{
					ar.PushObj("Data");
					ar << f;
					ar.PopObj();
				}
				break;
			case PipelinePropertyType::Vec4:
				for (const glm::vec4& f : buf.GetData_V4())
				{
					ar.PushObj("Data");
					ar << f;
					ar.PopObj();
				}
				break;
			default:
				// @WARNING
				break;
			}
		ar.PopObj();
	}
	else
	{
		ar.PushObj("Data");

			size_t num;
			ar >> num;
			switch (buf.GetType())
			{
			case PipelinePropertyType::Float:
			{
				EVec<float> v;
				for (int i = 0; i < num; i++)
				{
					ar.PushChild(i);
						float f;
						ar >> f;
						v.push_back(f);
					ar.PopObj();
				}
				buf.SetData(v, buf.GetSemantics());
				break;
			}
			case PipelinePropertyType::Mat4:
			{
				EVec<glm::mat4> v;
				for (int i = 0; i < num; i++)
				{
					ar.PushChild(i);
					glm::mat4 f;
					ar >> f;
					v.push_back(f);
					ar.PopObj();
				}
				buf.SetData(v, buf.GetSemantics());
				break;
			}
			case PipelinePropertyType::UInt32:
			{
				EVec<u32> v;
				for (int i = 0; i < num; i++)
				{
					ar.PushChild(i);
					u32 f;
					ar >> f;
					v.push_back(f);
					ar.PopObj();
				}
				buf.SetData(v, buf.GetSemantics());
				break;
			}
			case PipelinePropertyType::Vec2:
			{
				EVec<glm::vec2> v;
				for (int i = 0; i < num; i++)
				{
					ar.PushChild(i);
					glm::vec2 f;
					ar >> f;
					v.push_back(f);
					ar.PopObj();
				}
				buf.SetData(v, buf.GetSemantics());
				break;
			}
			case PipelinePropertyType::Vec3:
			{
				EVec<glm::vec3> v;
				for (int i = 0; i < num; i++)
				{
					ar.PushChild(i);
					glm::vec3 f;
					ar >> f;
					v.push_back(f);
					ar.PopObj();
				}
				buf.SetData(v, buf.GetSemantics());
				break;
			}
			case PipelinePropertyType::Vec4:
			{
				EVec<glm::vec4> v;
				for (int i = 0; i < num; i++)
				{
					ar.PushChild(i);
					glm::vec4 f;
					ar >> f;
					v.push_back(f);
					ar.PopObj();
				}
				buf.SetData(v, buf.GetSemantics());
				break;
			}
			default:
				// @WARNING
				break;
			}
		ar.PopObj();
	}
}

void PipelineMeshBuffer::Serialize(IArchive& archive)
{
	int version = 1;
	if (archive.IsStoring())
	{
		archive.PushObj("Buffer");
			archive << version;
			archive.PushObj("Type");
				archive << (u32)Type;
			archive.PopObj();
			archive.PushObj("Semantics");
				archive << Semantics;
			archive.PopObj();
			SerializeBufferData(archive, *this);
		archive.PopObj();
	}
	else
	{
		int v;
		archive >> v;
		switch (v)
		{
		case 1:
			archive.PushObj("Type");
				archive >> (u32&)Type;
			archive.PopObj();
			
			archive.PushObj("Semantics");
				archive >> (u16&)Semantics;
			archive.PopObj();
			SerializeBufferData(archive, *this);
			break;
		}
	}
}

size_t PipelineMeshBuffer::GetSizeInElements()
{
	switch (Type)
	{
	case PipelinePropertyType::Unknwon:
	default: assert(false); break;
	case PipelinePropertyType::Vec2: return GetData_V2().size();
	case PipelinePropertyType::Vec3: return GetData_V3().size();
	case PipelinePropertyType::Vec4: return GetData_V4().size();
	case PipelinePropertyType::Mat4: return GetData_M4().size();
	case PipelinePropertyType::Float: return GetData_f32().size();
	case PipelinePropertyType::UInt32: return GetData_u32().size();
	}
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
	//if (Semantics != psUnknown)
	//{
	//	PipelineError e{
	//		PipelineErrorType::NotExactlyOneSemanticBitSet,
	//		"[PipelineMeshBuffer::CheckBufferDataSetRequest] already recieved data"
	//	};
	//	Errors.push_back(e);
	//	allowed = false;
	//}
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
	if (buffer.GetSemantics() == psPerVertexPos)
	{
		if (buffer.GetType() != PipelinePropertyType::Vec3)
		{
			Err::LogError("invalid semantics: psPeVertexPos should be vec3");
			return;
		}
		EAssert(!m_bBoundingSphereSet);
		m_bBoundingSphereSet = true;
		m_Sphere = RitterBoundingSphere(buffer.GetData_V3());
	}

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

void PipelineMeshData::Serialize(IArchive& archive)
{
	const int version = 1;
	if (archive.IsStoring())
	{
		archive.PushObj("PipelineMeshData");
			archive << version;
			archive.PushObj("Name");
				archive << Name;
			archive.PopObj();
			archive.PushObj("Buffers");
				archive << Buffers.size();
				for (PipelineMeshBuffer& buf : Buffers)
				{
					buf.Serialize(archive);
				}
			archive.PopObj(); 
			m_Sphere.Serialize(archive);
			archive.PushObj("m_bBoundingSphereSet");
				archive << m_bBoundingSphereSet;
			archive.PopObj();
		archive.PopObj();
	}
	else
	{
		archive.PushObj("PipelineMeshData");
			int v;
			archive >> v;
			switch (v)
			{
			case 1:
				archive.PushObj("Name");
					archive >> Name;
				archive.PopObj();
				archive.PushObj("Buffers");
					size_t sz;
					archive >> sz;
					Buffers.resize(sz);
					for (int i=0; i<sz; i++)
					{
						archive.PushChild(i);
						Buffers[i].Serialize(archive);
						archive.PopObj();
					}
				archive.PopObj();
				m_Sphere.Serialize(archive);
				archive.PushObj("m_bBoundingSphereSet");
					archive >> m_bBoundingSphereSet;
				archive.PopObj();
				break;
			default:
				break;
			}
		archive.PopObj();

	}
}

void PipelineMeshData::CalcBoundingSphere()
{
	auto bufItr = std::find_if(Buffers.begin(), Buffers.end(), [](const PipelineMeshBuffer & b) -> bool 
	{
		return (b.GetSemantics() & PipelinePropertySemantics::psPerVertexPos) != 0 && b.GetType() == PipelinePropertyType::Vec3;
	});
	if (bufItr == Buffers.end())
	{
		EAssert(false);
		return;
	}
	const EVec<glm::vec3>& d = bufItr->GetData_V3();
	m_Sphere = RitterBoundingSphere(d);
}

