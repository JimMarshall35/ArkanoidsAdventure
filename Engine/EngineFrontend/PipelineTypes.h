#pragma once
#include <cstdint>
#include "EngineLib.h"

typedef Handle_t HFrameBuffer;
typedef Handle_t HDrawable;
typedef Handle_t HMesh;

typedef Handle_t HPipeline;
typedef Handle_t HPipelineUniformProperty;
typedef Handle_t HPipelinePerInstanceProperty;
typedef Handle_t HTexture;

enum RenderTargetBufType
{
	RTBUF_COLOUR = (1 << 0),
	RTBUF_DEPTH = (1 << 1),
	RTBUF_STENCIL = (1 << 2)
};


enum class PipelinePropertyType : std::uint32_t
{
	Unknwon,
	Vec2,
	Vec3,
	Vec4,
	Mat4,
	Float,
	UInt32,
	Texture,
	NUM_TYPE
};


enum PipelinePropertySemantics : std::uint16_t
{
	psUnknown = 0,
	psPerVertexPos = 1,
	psPerVertexNorm = (1 << 1),
	psPerVertexColour = (1 << 2),
	psPerVertexUV = (1 << 3),

	psIndices = (1 << 4),

	psPerInstanceModelMatrix = (1 << 5),

	psUniformModelMatrix = (1 << 6),
	psUnifomViewMatrix = (1 << 7),
	psUniformProjectionMatrix = (1 << 8),
	psUniformLightsArray = (1 << 9),
	psUniformColourTexture = (1 << 10),

	psCustomAttributesBegin,
	psUniformCustom = (1 << 11),
	psPerInstanceCustomVal = (1 << 12),
	psPerVertexCustomVal = (1 << 13),
};


enum class PipelineErrorType
{
	DuplicateSemantic,
	DuplicateName,
	NotExactlyOneSemanticBitSet,
	BufferSetTwice,
	InvalidSemanticsForBufferType,
	UnknownPropertyType
};


struct PipelineError
{
	PipelineErrorType Type;
	EString Msg;
};

int NumSemanticsSet(PipelinePropertySemantics semantics);

EString GetPipelineSemName(PipelinePropertySemantics semantics);
