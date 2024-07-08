#include "OGLRenderer.h"
#include "Pipeline.h"
#include "Shader.h"
#include "ObjectPool.h"
#include "Error.h"
#include "PipelineMeshData.h"
#include <unordered_map>
#include <algorithm>
#include "Sys.h"
#include "Texture.h"
#include <GLFW/glfw3.h>

namespace OGL
{
	//////////////////////////////////////////////////////////////////////// Defines

	static constexpr size_t MESH_POOL_SIZE = 256;
	static constexpr size_t PIPELINE_POOL_SIZE = 256;
	static constexpr size_t DRAWABLE_POOL_SIZE = 1024;


	//////////////////////////////////////////////////////////////////////// Typedefs

	struct OGLPipeline
	{
		EVec<Shader> StagesShaders;
		PipeLine Pipeline;
		void* pUserData = nullptr;
		EVec<EMap<EString, std::pair<HPipelineUniformProperty, Shader*>>> UniformProperties; // by stage
		PerDrawUniformSetterFn PerDrawUnformSetterFn = nullptr;
		GLuint InstancedBufferData = 0;
	};

	struct OGLMesh
	{
		std::vector<GLuint> VertexBuffers;
		std::vector<PipelinePropertySemantics> BufferSemantics;
		std::vector<PipelinePropertyType> Types;
		EUPtr<char[]> pStagingBuffer;
		int StagingBufferSize = 0;
		bool bHasIndices = false;
		size_t NumIndsOrVertsToDraw = 0;
		GLuint InstanceBufferHandle = 0;
		GLuint InstanceBufferSize = 0;
	};

	struct OGLDrawable
	{
		GLuint VAO;
		HPipeline CurrentPipeline = ENGINE_NULL_HANDLE;
		HMesh CurrentMesh = ENGINE_NULL_HANDLE;
		void* pUserData;
		bool bDrawInstanced = false;

		PerInstanceUniformSetterFn UniformSetter = nullptr;
		PerInstanceAttributeSetterFn InstantancedAttributeSetter = nullptr;
	};

	//////////////////////////////////////////////////////////////////////// Variables

	EObjectPool<OGLPipeline, PIPELINE_POOL_SIZE> gPipelinePool;
	EObjectPool<OGLMesh, MESH_POOL_SIZE> gMeshPool;
	EObjectPool<OGLDrawable, DRAWABLE_POOL_SIZE> gDrawablePool;

	//////////////////////////////////////////////////////////////////////// Internal helpers

	

	static OGLPipeline* GetPipeline(HPipeline hPipeline)
	{
		if (hPipeline - 1 >= gPipelinePool.GetCapcity())
		{
			Error::ErrorMsg({ "[GetPipeline] pipeline handle out of range: " + std::to_string(hPipeline), BackendErrorSeverity::Error });
			return nullptr;
		}
		if (!gPipelinePool.IsIndexAlive(hPipeline - 1))
		{
			Error::ErrorMsg({ "handle corresponds to dead index", BackendErrorSeverity::Error });
			return nullptr;
		}
		
		return &gPipelinePool[hPipeline - 1];
	}

	static OGLMesh* GetMesh(HMesh hPipeline)
	{
		if (hPipeline - 1 >= gMeshPool.GetCapcity())
		{
			Error::ErrorMsg({ "[GetMesh] pipeline handle out of range: " + std::to_string(hPipeline), BackendErrorSeverity::Error });
			return nullptr;
		}
		if (!gMeshPool.IsIndexAlive(hPipeline - 1))
		{
			Error::ErrorMsg({ EString("[GetMesh] handle ")+std::to_string(hPipeline)+EString("corresponds to dead index"), BackendErrorSeverity::Error });
			return nullptr;
		}

		return &gMeshPool[hPipeline - 1];
	}

	static OGLDrawable* GetDrawable(HDrawable hDrawable)
	{
		if (hDrawable - 1 >= gDrawablePool.GetCapcity())
		{
			Error::ErrorMsg({ "[GetDrawable] drawable handle out of range: " + std::to_string(hDrawable), BackendErrorSeverity::Error });
			return nullptr;
		}
		if (!gMeshPool.IsIndexAlive(hDrawable - 1))
		{
			Error::ErrorMsg({ EString("[GetDrawable] handle ") + std::to_string(hDrawable) + EString("corresponds to dead index"), BackendErrorSeverity::Error });
			return nullptr;
		}

		return &gDrawablePool[hDrawable - 1];
	}

	void UploadMeshInternal(OGLMesh& oglMesh, const PipelineMeshData& data)
	{
		oglMesh.VertexBuffers.resize(data.GetBuffers().size());
		oglMesh.BufferSemantics.resize(data.GetBuffers().size());
		oglMesh.Types.resize(data.GetBuffers().size());

		glGenBuffers(oglMesh.VertexBuffers.size(), oglMesh.VertexBuffers.data());
		glGenBuffers(1, &oglMesh.InstanceBufferHandle);

		const EVec<PipelineMeshBuffer>& bufs = data.GetBuffers();
		int i = 0;
		for (const PipelineMeshBuffer& buf : bufs)
		{
			oglMesh.BufferSemantics[i] = buf.GetSemantics();
			oglMesh.Types[i] = buf.GetType();
			if (buf.GetSemantics() == psIndices)
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, oglMesh.VertexBuffers[i]);
			}
			else
			{
				glBindBuffer(GL_ARRAY_BUFFER, oglMesh.VertexBuffers[i]);
			}
			
			oglMesh.NumIndsOrVertsToDraw = 0;
			switch (buf.GetType())
			{
			break; case PipelinePropertyType::Float:
				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * buf.GetElementCount(), buf.GetStart(), GL_STATIC_DRAW);
			break; case PipelinePropertyType::UInt32:
				if (buf.GetSemantics() & psIndices)
				{
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * buf.GetElementCount(), buf.GetStart(), GL_STATIC_DRAW);
					oglMesh.bHasIndices = true;
					oglMesh.NumIndsOrVertsToDraw = buf.GetElementCount();
				}
				else
				{
					glBufferData(GL_ARRAY_BUFFER, sizeof(uint32_t) * buf.GetElementCount(), buf.GetStart(), GL_STATIC_DRAW);
				}
			break; case PipelinePropertyType::Vec2:
				glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * buf.GetElementCount(), buf.GetStart(), GL_STATIC_DRAW);
			break; case PipelinePropertyType::Vec3:
				glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * buf.GetElementCount(), buf.GetStart(), GL_STATIC_DRAW);
			break; case PipelinePropertyType::Vec4:
				glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * buf.GetElementCount(), buf.GetStart(), GL_STATIC_DRAW);
			break; default:
			case PipelinePropertyType::Texture:
			case PipelinePropertyType::Unknwon:
				//Error::ErrorMsg(BackendError{ "unknown buffer type!", BackendErrorSeverity::Error });
				assert(false);

			}
			i++;
		}
		if (oglMesh.NumIndsOrVertsToDraw == 0)
		{
			oglMesh.NumIndsOrVertsToDraw = bufs[0].GetElementCount();
		}
	}


	static Shader& GetShaderForStage(HPipeline pipeline, int stage)
	{
		OGLPipeline* pPipe = GetPipeline(pipeline);
		return pPipe->StagesShaders[stage];
	}

	static size_t GetPerInstanceStagingBufferSizeRequired(HPipeline p, int stage)
	{

		size_t rVal = 0;
		OGLPipeline* pPipeline = GetPipeline(p);

		if (stage < 0 || stage >= pPipeline->Pipeline.GetStages().size())
		{
			Error::ErrorMsg(BackendError{ "[GetPerInstanceStagingBufferSizeRequired] stage index out of range" });
			return rVal;
		}
		const PipeLineStage& stagePair = pPipeline->Pipeline.GetStages()[stage];
		const EVec<PipelinePropertyName>& perInstanceAttributes = stagePair.GetPerInstanceAttributes();
		for (const PipelinePropertyName& name : perInstanceAttributes)
		{
			static const size_t propTypeSizeLUT[(size_t)PipelinePropertyType::NUM_TYPE] =
			{
				0, // unknown
				sizeof(glm::vec2),//Vec2,
				sizeof(glm::vec3),//Vec3,
				sizeof(glm::vec4),//Vec4,
				sizeof(glm::mat4),//Mat4,
				sizeof(float),//Float,
				sizeof(uint32_t),//UInt32,
				//sizeof(glm::vec3),//Texture,*/
				0
			};
			rVal += propTypeSizeLUT[(int)name.PropType];
		}
		return rVal;
	}

	//////////////////////////////////////////////////////////////////////// Exposed functions
	
	HPipeline UploadPipeline(const PipeLine& p, void* pUserData)
	{
		HPipeline hPipeline = 0;
		OGLPipeline* pNewPipeline = nullptr;
		if (!gPipelinePool.AllocateNew(hPipeline, pNewPipeline))
		{
			return ENGINE_NULL_HANDLE;
		}
		const EVec<PipeLineStage>& stages = p.GetStages();
		for (const PipeLineStage& ps : stages)
		{
			pNewPipeline->StagesShaders.push_back(Shader());
			pNewPipeline->StagesShaders[pNewPipeline->StagesShaders.size() - 1].LoadFromString(ps.GetVertShader().data(), ps.GetFragShader().data());
		}
		pNewPipeline->Pipeline = p;
		pNewPipeline->pUserData = pUserData;

		EVec<EMap<EString, std::pair<HPipelineUniformProperty, Shader*>>>& uniforms = pNewPipeline->UniformProperties;
		const EVec<PipeLineStage>& newStages = pNewPipeline->Pipeline.GetStages();
		int i = 0;
		for (const PipeLineStage& stage : newStages)
		{
			pNewPipeline->UniformProperties.push_back({});

			EMap<EString, std::pair<HPipelineUniformProperty, Shader*>>& thisStage = pNewPipeline->UniformProperties.back();
			const Shader& shader = pNewPipeline->StagesShaders[i];
			shader.use();
			const EVec<PipelinePropertyName>& names = stage.GetUniformAttributes();
			bool bError = false;
			for (const PipelinePropertyName& name : names)
			{
				GLint loc = shader.GetUniformLocation(name.Name.c_str());
				if (loc == -1)
				{
					Error::ErrorMsg({ EString("Uniform with name ") + name.Name + EString(" not found in shader"), BackendErrorSeverity::Error });
					bError = false;
					continue;
				}
				thisStage[name.Name].first = loc;
				thisStage[name.Name].second = (Shader*)&shader;
			}
			i++;
		}

		return hPipeline + 1; // + 1 because 0 is valid for the raw index but its also the ENGINE_NULL_HANDLE
		                      // remember to subtract 1 when using to index into pool
	}

	HMesh UploadMesh(HPipeline hPipeline, const PipelineMeshData& mesh)
	{
		OGLPipeline* oglPipeline = GetPipeline(hPipeline);
		if (!oglPipeline)
		{
			return ENGINE_NULL_HANDLE;
		}
		const EVec<PipeLineStage>& stages = oglPipeline->Pipeline.GetStages();
		bool bIncompatible = false;
		for (const PipeLineStage& stage : stages)
		{
			if (!stage.IsMeshCompatible(mesh))
			{
				Error::ErrorMsg({ stage.GetName() + EString(" incompatible with mesh ") + mesh.GetName(), BackendErrorSeverity::Warning });
				bIncompatible = true;
			}
		}
		if (bIncompatible)
		{
			return ENGINE_NULL_HANDLE;
		}
		size_t index;
		OGLMesh* pMesh;
		if (!gMeshPool.AllocateNew(index, pMesh))
		{
			Error::ErrorMsg({ "[UploadMesh] Mesh Pool full ", BackendErrorSeverity::Warning });

			return ENGINE_NULL_HANDLE;
		}
		UploadMeshInternal(*pMesh, mesh);
		return index + 1;
	}

	HPipeline GetDrawablePipeline(HDrawable drawable)
	{
		return ENGINE_NULL_HANDLE;
	}

	void SetDrawablePipelineInternal(OGLDrawable* pDrawable, HPipeline pipeline)
	{
		auto GetAttributeLocationFromSemantics = [](OGLPipeline* pPipeline, PipelinePropertySemantics vboSemantics) -> int
		{
			const PipeLine& p = pPipeline->Pipeline;
			for (const PipeLineStage& stage : p.GetStages())
			{
				for (const PipelinePropertyName& name : stage.GetVertexAttributes())
				{
					if (name.Semantics & vboSemantics)
					{
						return name.location;
					}
				}
			}
		};

		pDrawable->CurrentPipeline = pipeline;
		glGenVertexArrays(1, &pDrawable->VAO);
		glBindVertexArray(pDrawable->VAO);

		OGLPipeline* pPipeline = GetPipeline(pipeline);
		OGLMesh* pMesh = GetMesh(pDrawable->CurrentMesh);
		const EVec<GLuint>& buffers = pMesh->VertexBuffers;
		for (int i = 0; i < buffers.size(); i++)
		{
			PipelinePropertySemantics sem = pMesh->BufferSemantics[i];
			int location = GetAttributeLocationFromSemantics(pPipeline, sem);
			GLuint vbo = buffers[i];
			PipelinePropertyType type = pMesh->Types[i];

			if (sem & psIndices)
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo);

				continue;
			}
			else
			{
				glBindBuffer(GL_ARRAY_BUFFER, vbo);
			}
			

			switch (type)
			{
			break; case PipelinePropertyType::UInt32:
				glVertexAttribPointer(location, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(uint32_t), (void*)0);
			break; case PipelinePropertyType::Float:
				glVertexAttribPointer(location, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
			break; case PipelinePropertyType::Vec2:
				glVertexAttribPointer(location, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
			break; case PipelinePropertyType::Vec3:
				glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
			break; case PipelinePropertyType::Vec4:
				glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
			}

			glEnableVertexAttribArray(location);
		}

		size_t stages = pPipeline->Pipeline.GetStages().size();
		for (int i = 0; i < stages; i++)
		{
			const PipeLineStage& stage = pPipeline->Pipeline.GetStages()[i];
			const EVec<PipelinePropertyName>& names = stage.GetPerInstanceAttributes();
			for (const PipelinePropertyName& name : names)
			{
				switch (name.PropType)
				{
				break; case PipelinePropertyType::UInt32:
					glVertexAttribPointer(name.location, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(uint32_t), (void*)0);
				break; case PipelinePropertyType::Float:
					glVertexAttribPointer(name.location, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
				break; case PipelinePropertyType::Vec2:
					glVertexAttribPointer(name.location, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
				break; case PipelinePropertyType::Vec3:
					glVertexAttribPointer(name.location, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
				break; case PipelinePropertyType::Vec4:
					glVertexAttribPointer(name.location, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)0);
				}
				glEnableVertexAttribArray(name.location);
				glVertexAttribDivisor(name.location, 1);
			}
		}

		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	HDrawable CreateDrawable(HPipeline pipeline, HMesh mesh, void* pUserData)
	{
		
		size_t index;
		OGLDrawable* pDrawable;
		if (!gDrawablePool.AllocateNew(index, pDrawable))
		{
			Error::ErrorMsg({ "[CreateDrawable] Drawable Pool full ", BackendErrorSeverity::Warning });
			return ENGINE_NULL_HANDLE;
		}
		pDrawable->CurrentMesh = mesh;
		pDrawable->pUserData = pUserData;
		SetDrawablePipelineInternal(pDrawable, pipeline);

		return index + 1;
	}

	HPipeline ChangeDrawablePipeline(HDrawable d, HPipeline newPipeline)
	{
		OGLDrawable* drawable = GetDrawable(d);
		HPipeline r = drawable->CurrentPipeline;
		glDeleteVertexArrays(1, &drawable->VAO);
		drawable->VAO = 0;
		SetDrawablePipelineInternal(drawable, newPipeline);
		return r;
	}

	void DrawDrawablesInternal(const EVec<HDrawable>& nonInstanced, const EVec<EVec<HDrawable>>& instances)
	{
		for (const EVec<HDrawable>& instance : instances)
		{
			HDrawable first = instance[0];
			OGLDrawable* d = GetDrawable(first);
			assert(d->bDrawInstanced);
			HPipeline hPipe = d->CurrentPipeline;
			HMesh hMesh = d->CurrentMesh;
		}
	}

	static void SortDrawables(
		HDrawable* drawables,
		size_t numDrawables,
		EMap<HPipeline, EVec<HDrawable>>& byPipelineInstanced,
		EMap<HPipeline, EVec<HDrawable>>& byPipelineNonInstanced,
		EVec<HPipeline>& outPipelines)
	{
		for (int i = 0; i < numDrawables; i++)
		{
			HDrawable hDrawable = drawables[i];
			OGLDrawable* d = GetDrawable(hDrawable);
			if (d->bDrawInstanced)
			{
				byPipelineInstanced[d->CurrentPipeline].push_back(hDrawable);
			}
			else
			{
				byPipelineNonInstanced[d->CurrentPipeline].push_back(hDrawable);
			}
			auto itr = std::find(outPipelines.begin(), outPipelines.end(), d->CurrentPipeline);
			if (itr == outPipelines.end())
			{
				outPipelines.push_back(d->CurrentPipeline);
			}
		}
	}

	//static void SortInstancedDrawablesByMesh(const EMap<HPipeline, EVec<HDrawable>>& byPipelineInstanced, EVec<EVec<HDrawable>>& instancedToDraw)
	//{
	//	for (const std::pair<HPipeline, EVec<HDrawable>>& p : byPipelineInstanced)
	//	{
	//		EMap<HMesh, EVec<HDrawable>> map;
	//		for (HDrawable drawable : p.second)
	//		{
	//			OGLDrawable* pDrawable = GetDrawable(drawable);
	//			map[pDrawable->CurrentMesh].push_back(drawable);
	//		}
	//		for (const std::pair<HMesh, EVec<HDrawable>>& pair : map)
	//		{
	//			instancedToDraw.push_back({});
	//			EVec<HDrawable>& vec = instancedToDraw.back();
	//			for (HDrawable d : pair.second)
	//			{
	//				vec.push_back(d);
	//			}
	//		}
	//	}
	//}

	static void SortPipelinesByNumStages(EVec<HPipeline>& pipeline)
	{
		std::sort(pipeline.begin(), pipeline.end(), [](HPipeline hPipe1, HPipeline hPipe2) -> bool
		{
			OGLPipeline* pPipe1 = GetPipeline(hPipe1);
			OGLPipeline* pPipe2 = GetPipeline(hPipe2);
			return pPipe1->StagesShaders.size() > pPipe2->StagesShaders.size();
		});
	}

	static void DrawDrawablesNonInstanced(const EVec<HDrawable>& drawables, HPipeline pipeline, int shaderIndex)
	{
		OGLPipeline* pPipe = GetPipeline(pipeline);
		int i = 0;
		Shader& s = pPipe->StagesShaders[shaderIndex];
		s.use();
		if (pPipe->PerDrawUnformSetterFn)
		{
			pPipe->PerDrawUnformSetterFn(pipeline, pPipe->pUserData, shaderIndex);
		}
		for (HDrawable drawable : drawables)
		{
			OGLDrawable* pDrawable = GetDrawable(drawable);
			if (pDrawable->UniformSetter)
			{
				pDrawable->UniformSetter(drawable, pipeline, i, pDrawable->pUserData, pPipe->pUserData);
			}
			glBindVertexArray(pDrawable->VAO);
			OGLMesh* pMesh = GetMesh(pDrawable->CurrentMesh);
			if (pMesh->bHasIndices)
			{
				glDrawElements(GL_TRIANGLES, pMesh->NumIndsOrVertsToDraw, GL_UNSIGNED_INT, 0);
			}
			else
			{
				glDrawArrays(GL_TRIANGLES, 0, pMesh->NumIndsOrVertsToDraw);
			}
		}
		i++;
	}

	static void DrawDrawablesInstanced(const EVec<HDrawable>& drawables, HPipeline pipeline, int shaderIndex)
	{
		OGLPipeline* pPipe = GetPipeline(pipeline);
		Shader& s = pPipe->StagesShaders[shaderIndex];
		const size_t perInstanceSize = GetPerInstanceStagingBufferSizeRequired(pipeline, shaderIndex);
		EMap<HMesh, EVec<HDrawable>> byMesh;
		for (HDrawable d : drawables)
		{
			OGLDrawable* pDrawable = GetDrawable(d);
			
			byMesh[pDrawable->CurrentMesh].push_back(d);
		}
		for (const std::pair<HMesh, EVec<HDrawable>>& p : byMesh)
		{
			const EVec<HDrawable>& instance = p.second;
			OGLMesh* pMesh = GetMesh(p.first);
			
			size_t bufferSize = perInstanceSize * instance.size();
			if (pMesh->StagingBufferSize < bufferSize)
			{
				pMesh->pStagingBuffer.reset();
				pMesh->pStagingBuffer = std::make_unique<char[]>(bufferSize);
				pMesh->StagingBufferSize = bufferSize;
				char* pWrite = pMesh->pStagingBuffer.get();
				for (HDrawable di : instance)
				{
					OGLDrawable* instanceD = GetDrawable(di);
					if (instanceD->InstantancedAttributeSetter)
					{
						instanceD->InstantancedAttributeSetter(di, pipeline, shaderIndex, instanceD->pUserData, pPipe->pUserData, pWrite);
					}
					pWrite += perInstanceSize;
				}
				glBindBuffer(GL_ARRAY_BUFFER, pMesh->InstanceBufferHandle);
				glBufferData(GL_ARRAY_BUFFER, perInstanceSize * instance.size(), pMesh->pStagingBuffer.get(), GL_DYNAMIC_DRAW);
			}
			else
			{
				char* pWrite = pMesh->pStagingBuffer.get();
				for (HDrawable di : instance)
				{
					OGLDrawable* instanceD = GetDrawable(di);
					if (instanceD->InstantancedAttributeSetter)
					{
						instanceD->InstantancedAttributeSetter(di, pipeline, shaderIndex, instanceD->pUserData, pPipe->pUserData, pWrite);
					}
					pWrite += perInstanceSize;
				}
				glBindBuffer(GL_ARRAY_BUFFER, pMesh->InstanceBufferHandle);
				glBufferSubData(GL_ARRAY_BUFFER, 0, bufferSize, pMesh->pStagingBuffer.get());
			}
		}
	}

	
	void DrawDrawables(HDrawable* drawables, size_t numDrawables)
	{
		EMap<HPipeline, EVec<HDrawable>> byPipelineInstanced;
		EMap<HPipeline, EVec<HDrawable>> byPipelineNonInstanced;
		EVec<HPipeline> pipelineHandles;
		SortDrawables(drawables, numDrawables, byPipelineInstanced, byPipelineNonInstanced, pipelineHandles);
		SortPipelinesByNumStages(pipelineHandles);

		OGLPipeline* pPipeline = GetPipeline(pipelineHandles[0]);
		int mostStages = pPipeline->StagesShaders.size();
		for (int i = mostStages-1; i >= 0; i--)
		{
			for (HPipeline hPipeline : pipelineHandles)
			{
				OGLPipeline* pPipe = GetPipeline(hPipeline);
				int numStages = pPipe->StagesShaders.size();
				if (numStages < i)
				{
					break;
				}
				if (byPipelineInstanced.find(hPipeline) != byPipelineInstanced.end())
				{
					assert(byPipelineNonInstanced.find(hPipeline) == byPipelineNonInstanced.end());
					const EVec<HDrawable>& drawables = byPipelineInstanced.at(hPipeline);
					DrawDrawablesInstanced(drawables, hPipeline, mostStages - (mostStages - i));

				}
				else if(byPipelineNonInstanced.find(hPipeline) != byPipelineNonInstanced.end())
				{
					assert(byPipelineInstanced.find(hPipeline) == byPipelineInstanced.end());
					const EVec<HDrawable>& drawables = byPipelineNonInstanced.at(hPipeline);
					DrawDrawablesNonInstanced(drawables, hPipeline, mostStages - (mostStages - i));
				}
				else
				{
					assert(false);
				}
			}
		}
	}

	void DestroyDrawable(HDrawable d)
	{
		OGLDrawable* drawable = GetDrawable(d);
		glDeleteVertexArrays(1, &drawable->VAO);
		memset(drawable, 0, sizeof(OGLDrawable));
		gDrawablePool.DeallocateObject(d - 1);
	}

	void DestroyMesh(HMesh m)
	{
		OGLMesh* mesh = GetMesh(m);
		glDeleteBuffers(mesh->VertexBuffers.size(), mesh->VertexBuffers.data());
		glDeleteBuffers(1, &mesh->InstanceBufferHandle);
		mesh->pStagingBuffer.reset();
		memset(mesh, 0, sizeof(OGLMesh));
		gMeshPool.DeallocateObject(m - 1);
	}

	void DestroyPipeline(HPipeline p)
	{
		OGLPipeline* pipe = GetPipeline(p);
		// todo: ensure shaders destroyed
		memset(pipe, 0, sizeof(OGLPipeline));
		gPipelinePool.DeallocateObject(p - 1);
	}

	void InitRenderer()
	{
		printf("initialising opengl renderer");
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		glPixelStorei(GL_PACK_ALIGNMENT, 1);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glViewport(0, 0, Sys::GetW(), Sys::GetH());
	}

	void PreRender()
	{
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void PostRender()
	{
		Sys::SwapBuffers();
	}

	HPipelineUniformProperty GetPipelineUniformPropertyH(HPipeline p, const EString& name, int stage)
	{
		OGLPipeline* pPipe = GetPipeline(p);

		if (stage < 0 || stage >= pPipe->UniformProperties.size())
		{
			Error::ErrorMsg(BackendError{ EString("[GetPipelineUniformPropertyH] stage index out of range: ") + std::to_string(stage), BackendErrorSeverity::Error });
			return ENGINE_NULL_HANDLE;
		}
		if (pPipe->UniformProperties[stage].find(name) == pPipe->UniformProperties[stage].end())
		{
			Error::ErrorMsg(BackendError{ EString("[GetPipelineUniformPropertyH] can't find uniform with name: ") + name, BackendErrorSeverity::Error });
			return ENGINE_NULL_HANDLE;
		}
		return pPipe->UniformProperties[stage].at(name).first;
	}

	void SetPipelineUniform_Vec2(HPipeline pipeline, HPipelineUniformProperty hProp, int pipelineStage, const glm::vec2& v)
	{
		Shader& shader = GetShaderForStage(pipeline, pipelineStage);
		shader.setVec2Direct(hProp, v);
	}

	void SetPipelineUniform_Vec3(HPipeline pipeline, HPipelineUniformProperty hProp, int pipelineStage, const glm::vec3& v)
	{
		Shader& shader = GetShaderForStage(pipeline, pipelineStage);
		shader.setVec2Direct(hProp, v);

	}

	void SetPipelineUniform_Vec4(HPipeline pipeline, HPipelineUniformProperty hProp, int pipelineStage, const glm::vec4& v)
	{
		Shader& shader = GetShaderForStage(pipeline, pipelineStage);
		shader.setVec2Direct(hProp, v);
	}

	void SetPipelineUniform_Mat4(HPipeline pipeline, HPipelineUniformProperty hProp, int pipelineStage, const glm::mat4& v)
	{
		Shader& shader = GetShaderForStage(pipeline, pipelineStage);
		shader.setMat4Direct(hProp, v);
	}

	void SetPipelineUniform_Float(HPipeline pipeline, HPipelineUniformProperty hProp, int pipelineStage, float v)
	{
		Shader& shader = GetShaderForStage(pipeline, pipelineStage);
		shader.setFloatDirect(hProp, v);

	}

	void SetPipelineUniform_UInt32(HPipeline pipeline, HPipelineUniformProperty hProp, int pipelineStage, uint32_t v)
	{
		Shader& shader = GetShaderForStage(pipeline, pipelineStage);
		shader.SetIntDirect(hProp, v);
	}

	void SetPipelineUniform_Texture(HPipeline pipeline, HPipelineUniformProperty hProp, int pipelineStage, HTexture v)
	{

	}
	
	void RegisterPerDrawUniformSetter(HPipeline hPipe, PerDrawUniformSetterFn fn)
	{
		OGLPipeline* pPipe = GetPipeline(hPipe);
		pPipe->PerDrawUnformSetterFn = fn;
	}
	
	void RegisterPerInstanceUniformSetter(HDrawable hDrawable, PerInstanceUniformSetterFn fn)
	{
		OGLDrawable* pDrawable = GetDrawable(hDrawable);
		pDrawable->UniformSetter = fn;
	}
	
	void RegisterPerInstanceAttributeSetter(HDrawable hDrawable, PerInstanceAttributeSetterFn fn)
	{
		OGLDrawable* pDrawable = GetDrawable(hDrawable);
		pDrawable->InstantancedAttributeSetter = fn;
	}

	void SetDrawInstanced(HDrawable hDrawable, bool bInstanced)
	{
		OGLDrawable* pDrawable = GetDrawable(hDrawable);
		pDrawable->bDrawInstanced = bInstanced;
	}
	HTexture UploadTexture(const TextureData& data)
	{
		return HTexture();
	}
}
