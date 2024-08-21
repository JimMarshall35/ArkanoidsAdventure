#pragma once
#include "PipelineTypes.h"
#include "EngineLib.h"
#include <variant>
#include <glm/glm.hpp>
#include "ExportMacro.h"
#include "Sphere.h"
class IArchive;

class ENGINE_FRONTEND_API PipelineMeshBuffer
{
public:
	void SetData(const EVec<glm::vec2>& data, PipelinePropertySemantics semantics);
	void SetData(const EVec<unsigned int>& data, PipelinePropertySemantics semantics);
	void SetData(const EVec<glm::vec3>& data, PipelinePropertySemantics semantics);
	void SetData(const EVec<glm::vec4>& data, PipelinePropertySemantics semantics);
	void SetData(const EVec<glm::mat4>& data, PipelinePropertySemantics semantics);
	void SetData(const EVec<float>& data, PipelinePropertySemantics semantics);

	const EVec<unsigned int>& GetData_u32() { return std::get<EVec<unsigned int>>(Data); }
	const EVec<float>& GetData_f32()    const     { return std::get<EVec<float>>(Data); }
	const EVec<glm::vec2>& GetData_V2() const     { return std::get<EVec<glm::vec2>>(Data); }
	const EVec<glm::vec3>& GetData_V3() const     { return std::get<EVec<glm::vec3>>(Data); }
	const EVec<glm::vec4>& GetData_V4() const     { return std::get<EVec<glm::vec4>>(Data); }
	const EVec<glm::mat4>& GetData_M4() const     { return std::get<EVec<glm::mat4>>(Data); }

	PipelinePropertyType GetType() const { return Type; }
	bool Valid()const { return Errors.size() == 0; }
	const EVec<PipelineError>& GetErrors() const { return Errors; };
	const void* GetStart() const;
	PipelinePropertySemantics GetSemantics() const { return Semantics; }
	size_t GetElementCount() const;
	void Serialize(IArchive& archive);
private:
	template<typename T>
	void SetBufferDataInternal(const EVec<T>& data, PipelinePropertySemantics semanticsSet, const EString& type, PipelinePropertyType PT, const EVec<PipelinePropertySemantics>& allowedSemantics)
	{
		bool bAllowed = CheckBufferDataSetRequest(allowedSemantics, semanticsSet);
		if (!bAllowed)
		{
			Errors.push_back(PipelineError{
					PipelineErrorType::InvalidSemanticsForBufferType,
					"type" + type + " invalid for semantic: " + GetPipelineSemName(semanticsSet)
				});
		}
		else
		{
			Type = PT;
			Data = data;
			Semantics = semanticsSet;
		}
	}
	bool CheckBufferDataSetRequest(const EVec<PipelinePropertySemantics>& allowedSemantics, PipelinePropertySemantics semanticsSet);
private:
	std::variant<
		EVec<glm::vec2>,
		EVec<unsigned int>,
		EVec<glm::vec3>,
		EVec<glm::vec4>,
		EVec<glm::mat4>,
		EVec<float>
	> Data;
	PipelinePropertySemantics Semantics = psUnknown;
	PipelinePropertyType Type = PipelinePropertyType::Unknwon;
	EVec<PipelineError> 
		
		Errors;
};

class ENGINE_FRONTEND_API PipelineMeshData
{
public:
	PipelineMeshData() {}
	PipelineMeshData(const char* name);
	void TryAddBuffer(const PipelineMeshBuffer& buffer);
	PipelinePropertySemantics GetSemantics() const;
	EVec<PipelineMeshBuffer>& GetBuffers() { return Buffers; }
	const EVec<PipelineMeshBuffer>& GetBuffers() const { return Buffers; }

	bool HasErrors() const { return !Errors.empty(); }
	const EVec<PipelineError>& GetErrors() { return Errors; }
	const EString& GetName() const { return Name; }
	const Sphere& GetBoundingSphere() const { EAssert(m_bBoundingSphereSet); return m_Sphere; }
	void Serialize(IArchive& archive);

private:
	EVec<PipelineMeshBuffer> Buffers;
	EVec<PipelineError> Errors;
	EString Name;
	Sphere m_Sphere{ {0,0,0}, 0 };
	bool m_bBoundingSphereSet = false;
};

