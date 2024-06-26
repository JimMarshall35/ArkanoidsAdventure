#pragma once
#include "PipelineTypes.h"
#include "EngineLib.h"
#include <variant>
#include <glm/glm.hpp>

class PipelineMeshBuffer
{
public:
	void SetBufferData(const EVec<glm::vec2>& data, PipelinePropertySemantics semantics);
	void SetBufferData(const EVec<unsigned int>& data, PipelinePropertySemantics semantics);
	void SetBufferData(const EVec<glm::vec3>& data, PipelinePropertySemantics semantics);
	void SetBufferData(const EVec<glm::vec4>& data, PipelinePropertySemantics semantics);
	void SetBufferData(const EVec<glm::mat4>& data, PipelinePropertySemantics semantics);
	void SetBufferData(const EVec<float>& data, PipelinePropertySemantics semantics);
	bool Valid()const { return Errors.size(); }
	const EVec<PipelineError>& GetErrors() const { return Errors; };
	const void* GetStart() const;
	PipelinePropertySemantics GetSemantics() const { return Semantics; }
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
			EVec<T>& v = std::get<EVec<T>>(Data);
			v = data;
			Type = PT;
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
	EVec<PipelineError> Errors;
};

class PipelineMeshData
{
public:
	void TryAddBuffer(const PipelineMeshBuffer& buffer);
	PipelinePropertySemantics GetSemantics() const;
private:
	EVec<PipelineMeshBuffer> Buffers;
	EVec<PipelineError> Errors;

};

