#include "PipelineTypes.h"
#include <intrin.h>
#include <cassert>

int NumSemanticsSet(PipelinePropertySemantics semantics)
{
	return __popcnt16(semantics);
}

EString GetPipelineSemName(PipelinePropertySemantics semantics)
{
#define SEMCASE(sem) case sem: return EString(#sem);
	switch (semantics)
	{
		SEMCASE(psUnknown)
		SEMCASE(psPerVertexPos)
		SEMCASE(psPerVertexNorm)
		SEMCASE(psPerVertexColour)
		SEMCASE(psPerVertexUV)
		SEMCASE(psPerInstanceModelMatrix)
		SEMCASE(psUniformModelMatrix)
		SEMCASE(psUnifomViewMatrix)
		SEMCASE(psUniformProjectionMatrix)
		SEMCASE(psUniformLightsArray)
		SEMCASE(psUniformColourTexture) 
		SEMCASE(psUniformCustom)
		SEMCASE(psPerInstanceCustomVal)
		SEMCASE(psPerVertexCustomVal)
	default:
		assert(false);

	}
	return EString();
#undef SEMCASE
}
