#include "Pipeline.h"
#include "PipelineMeshData.h"
#include "EngineDLLInterface.h"
#include "IBackendApp.h"
#include "FrontendError.h"

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

PipeLine::PipeLine(const char* name)
	:Autolist<PipeLine>(true)//Name(name)
{
	Name = name;
}

void PipeLine::Create()
{
	const BackendAPI& api = Engine::GetAPI();
	PipelineHandle = api.UploadPipeline(*this, this);
	int nStage = 0;
	for (const PipeLineStage& stage : Stages)
	{
		const EVec<PipelinePropertyName>&  props = stage.GetUniformAttributes();
		for (const PipelinePropertyName& p : props)
		{
			if (UniformPropertyHandlesByName.find(p.Name) != UniformPropertyHandlesByName.end())
			{
				Err::LogError("already got a property handle for property '%s'. Did you call PipeLine::Create twice?");
			}
			UniformPropertyHandlesByName[p.Name] = api.GetPipelineUniformPropertyH(PipelineHandle, p.Name, nStage);
		}
		++nStage;
	}
}

HPipelineUniformProperty PipeLine::TryGetUniformProperty(const char* name) const
{
	return HPipelineUniformProperty();
}

HDrawable PipeLine::GetDrawable(HMesh mesh, Entity e)
{
	const BackendAPI& api = Engine::GetAPI();
	HDrawable hDrawable = api.CreateDrawable(PipelineHandle, mesh, (void*)e);
	api.RegisterPerInstanceUniformSetter(hDrawable, &PerInstanceUniformSetter);
	api.RegisterPerDrawUniformSetter(PipelineHandle, &PerDrawUnifomSetter);
	return hDrawable;
}

PipeLine* PipeLine::TryGetPipeline(const char* name)
{
	std::pair<const char*, PipeLine*> pair;
	pair.first = name;
	pair.second = nullptr;
	auto fn = [](PipeLine* p, void* pUser) -> bool
	{
		std::pair<const char*, PipeLine*>* pair = (std::pair<const char*, PipeLine*>*)pUser;
		if (strcmp(pair->first, p->GetName()) == 0)
		{
			pair->second = p;
			return true;
		}
		return false; 
	};
	IteratePipelines(fn, &pair);
	return pair.second;
}

PipeLine* PipeLine::TryGetPipeline(HPipeline h)
{
	std::pair<HPipeline, PipeLine*> pair;
	pair.first = h;
	pair.second = nullptr;
	auto fn = [](PipeLine* p, void* pUser) -> bool
	{
		std::pair<HPipeline, PipeLine*>* pair = (std::pair<HPipeline, PipeLine*>*)pUser;
		if (pair->first == p->GetH())
		{
			pair->second = p;
			return true;
		}
		return false;
	};
	IteratePipelines(fn, &pair);
	return pair.second;
}

void PipeLine::IteratePipelines(bool(*callBack)(PipeLine* pn, void* pUser), void* pUser)
{
	Autolist<PipeLine>* head = Autolist<PipeLine>::GetHead();
	while (head)
	{
		PipeLine* p = (PipeLine*)head;
		if (callBack(p, pUser))
		{
			break;
		}
		head = head->GetNext();
	}
}

int PipeLine::NumPipelines()
{
	int num;
	auto fn = [](PipeLine* p, void* pUser) -> bool
	{
		int* pNum = (int*)pUser;
		int& r = *pNum;
		r++;
		return false;
	};
	IteratePipelines(fn, &num);
	return num;
}

void PipeLine::PerDrawUnifomSetter(HPipeline pipeline, void* pUserData, int pipelineStage)
{
	PipeLine* p = (PipeLine*)pUserData;
	p->PerDrawUniform(pipelineStage);
}

void PipeLine::PerInstanceUniformSetter(HDrawable drawable, HPipeline pipeline, int pipelineStage, void* pDrawableUserData, void* pPipelineUserData)
{
	PipeLine* p = (PipeLine*)pPipelineUserData;
	Entity e = (Entity)(size_t)pDrawableUserData;
	p->PerInstanceUniform(pipelineStage, drawable, e);
}
