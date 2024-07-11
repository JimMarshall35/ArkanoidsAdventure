#include "TestPipeline.h"
#include "Pipeline.h"
#include "IBackendApp.h"
#include "EngineDLLInterface.h"

static const char* gMeshVert =
"#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n"
"layout(location = 1) in vec3 aNormal;\n"
"layout(location = 2) in vec2 aUv;\n"

"out vec3 FragPos;\n"
"out vec3 FragWorldPos;\n"
"out vec3 Normal;\n"
"out vec3 LightPos;\n"
"out vec3 WorldSpaceNormal;\n"
"out vec2 Uv;\n"

"uniform vec3 lightPos;\n" // we now define the uniform in the vertex shader and pass the 'view space' lightpos to the fragment shader. lightPos is currently in world space.

"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"

"void main()\n"
"{""\n"
	"gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
	"FragPos = vec3(view * model * vec4(aPos, 1.0));\n"
	"FragWorldPos = vec3(model * vec4(aPos, 1.0));\n"
	"Normal = mat3(transpose(inverse(view * model))) * aNormal;\n"
	"WorldSpaceNormal = vec3(model * vec4(aNormal, 0.0));\n"

	"LightPos = vec3(view * vec4(lightPos, 1.0));\n" // Transform world-space light position to view-space light position
	"Uv = aUv;\n"
"}\n";

static const char* gMeshFrag =
"#version 330 core\n"
"out vec4 FragColor;\n"

"in vec3 FragPos;\n"
"in vec3 FragWorldPos;\n"
"in vec3 Normal;\n"
"in vec3 LightPos;\n" // extra in variable, since we need the light position in view space we calculate this in the vertex shader
"in vec3 WorldSpaceNormal;\n"
"in vec2 Uv;\n"

"uniform vec3 lightColor;\n"
"uniform float ambientStrength;\n"
"uniform float specularStrength;\n"
"uniform float diffuseStrength;\n"
"uniform float shininess;\n"
"uniform sampler2D diffuseAtlas;\n"

"void main()\n"
"{\n"
	"// ambient""\n"
	"vec3 ambient = ambientStrength * lightColor;\n"

	"// diffuse ""\n"
	"vec3 norm = normalize(Normal);\n"
	"vec3 lightDir = normalize(LightPos - FragPos);\n"
	"float diff = max(dot(norm, lightDir), 0.0);\n"
	"vec3 diffuse = diffuseStrength * diff * lightColor;\n"

	"// specular""\n"
	"//float specularStrength = 0.2;\n"
	"vec3 viewDir = normalize(-FragPos);\n" // the viewer is always at (0,0,0) in view-space, so viewDir is (0,0,0) - Position => -Position
	"vec3 reflectDir = reflect(-lightDir, norm);\n"
	"float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);\n"
	"vec3 specular = specularStrength * spec * lightColor;\n"

	"vec4 colour = texture(diffuseAtlas, Uv);\n"
	"FragColor = vec4((ambient + diffuse + specular),1.0) * colour;\n"
"}\n";



PerDrawUniforms gPerDrawUniforms;


static HPipeline hPipeline = ENGINE_NULL_HANDLE;
static HPipelineUniformProperty hLightPos, hLightColour, hModel, hView, hProjection, hAmbientStrength, hSpecularStrength, hDiffuseStrength, hShininess, hDiffuseAtlas;

EVec<char> ToVector(const char* cString)
{
	EVec<char> data;
	size_t l = strnlen_s(cString, INT_MAX);
	data.resize(l + 1);
	for (int i = 0; i < l; i++)
	{
		data[i] = cString[i];
	}
	data[l] = '\0';
	return data;
}

void PerDrawUnifomSetter(HPipeline pipeline, void* pUserData, int pipelineStage)
{
	PerDrawUniforms* pUni = (PerDrawUniforms*)pUserData;
	const BackendAPI& api = Engine::GetAPI();
	api.SetPipelineUniform_Vec3(hPipeline, hLightPos, 0, pUni->lightPos);
	api.SetPipelineUniform_Vec3(hPipeline, hLightColour, 0, pUni->lightColour);
	api.SetPipelineUniform_Mat4(hPipeline, hView, 0, pUni->v);
	api.SetPipelineUniform_Mat4(hPipeline, hProjection, 0, pUni->p);
}

void PerInstanceUniformSetter(HDrawable drawable, HPipeline pipeline, int pipelineStage, void* pDrawableUserData, void* PipelineUserData)
{
	PerInstanceUniforms* pUni = (PerInstanceUniforms*)pDrawableUserData;
	const BackendAPI& api = Engine::GetAPI();
	api.SetPipelineUniform_Mat4(hPipeline, hModel, 0, pUni->m);
	api.SetPipelineUniform_Float(hPipeline, hAmbientStrength, 0, pUni->ambientStrength);
	api.SetPipelineUniform_Float(hPipeline, hSpecularStrength, 0, pUni->speculatStrength);
	api.SetPipelineUniform_Float(hPipeline, hDiffuseStrength, 0, pUni->diffuseStrength);
	api.SetPipelineUniform_Float(hPipeline, hShininess, 0, pUni->shininess);
	api.SetPipelineUniform_Texture(hPipeline, hDiffuseAtlas, 0, pUni->hTexture);

}

HPipeline BuildTestPipeline(PipeLine& outPipe, const BackendAPI& api)
{
	if (hPipeline != ENGINE_NULL_HANDLE)
	{
		assert(false);
		return hPipeline;
	}
	PipelineStageInitArgs initArgs;
	initArgs.fragShaderCode = ToVector(gMeshFrag);
	initArgs.vertShaderCode = ToVector(gMeshVert);
	initArgs.requiredRenderTargetBuffers = (RenderTargetBufType)(RTBUF_COLOUR | RTBUF_DEPTH);
	initArgs.hFrameBuffer = H_MAINWINDOW_FRAMEBUFFER;
	initArgs.Semantics = psIndices | psPerVertexPos | psPerVertexNorm | psPerVertexUV;
	PipeLineStage stage(initArgs);
	PipelinePropertyName pvPos, pvUV, pvNorm;
	pvPos.location = 0;
	pvPos.Name = "aPos";
	pvPos.PropType = PipelinePropertyType::Vec3;
	pvPos.Semantics = psPerVertexPos;

	pvNorm.location = 1;
	pvNorm.Name = "aNormal";
	pvNorm.PropType = PipelinePropertyType::Vec3;
	pvNorm.Semantics = psPerVertexNorm;

	pvUV.location = 2;
	pvUV.Name = "aUV";
	pvUV.PropType = PipelinePropertyType::Vec2;
	pvUV.Semantics = psPerVertexUV;

	stage.PushPerVertexAttribute(pvPos);
	stage.PushPerVertexAttribute(pvNorm);
	stage.PushPerVertexAttribute(pvUV);

	PipelinePropertyName uLightPos, uLightColour, uModel, uView, uProjection, uAmbientStrength, uSpecularStrength, uDiffuseStrength, uShininess, uDiffuseAtlas;
	uLightPos.location = 0;
	uLightPos.Name = "lightPos";
	uLightPos.PropType = PipelinePropertyType::Vec3;
	uLightPos.Semantics = psUniformCustom;
	stage.PushUniformAttribute(uLightPos);

	uLightColour.location = 0;
	uLightColour.Name = "lightColor";
	uLightColour.PropType = PipelinePropertyType::Vec3;
	uLightColour.Semantics = psUniformCustom;
	stage.PushUniformAttribute(uLightColour);

	uModel.location = 0;
	uModel.Name = "model";
	uModel.PropType = PipelinePropertyType::Mat4;
	uModel.Semantics = psUniformModelMatrix;
	stage.PushUniformAttribute(uModel);

	uView.location = 0;
	uView.Name = "view";
	uView.PropType = PipelinePropertyType::Mat4;
	uView.Semantics = psUnifomViewMatrix;
	stage.PushUniformAttribute(uView);

	uProjection.location = 0;
	uProjection.Name = "projection";
	uProjection.PropType = PipelinePropertyType::Mat4;
	uProjection.Semantics = psUniformProjectionMatrix;
	stage.PushUniformAttribute(uProjection);

	uAmbientStrength.location = 0;
	uAmbientStrength.Name = "ambientStrength";
	uAmbientStrength.PropType = PipelinePropertyType::Float;
	uAmbientStrength.Semantics = psUniformCustom;
	stage.PushUniformAttribute(uAmbientStrength);

	uSpecularStrength.location = 0;
	uSpecularStrength.Name = "specularStrength";
	uSpecularStrength.PropType = PipelinePropertyType::Float;
	uSpecularStrength.Semantics = psUniformCustom;
	stage.PushUniformAttribute(uSpecularStrength);

	uDiffuseStrength.location = 0;
	uDiffuseStrength.Name = "diffuseStrength";
	uDiffuseStrength.PropType = PipelinePropertyType::Float;
	uDiffuseStrength.Semantics = psUniformCustom;
	stage.PushUniformAttribute(uDiffuseStrength);

	uShininess.location = 0;
	uShininess.Name = "shininess";
	uShininess.PropType = PipelinePropertyType::Float;
	uShininess.Semantics = psUniformCustom;
	stage.PushUniformAttribute(uShininess);

	uDiffuseAtlas.location = 0;
	uDiffuseAtlas.Name = "diffuseAtlas";
	uDiffuseAtlas.PropType = PipelinePropertyType::Texture;
	uDiffuseAtlas.Semantics = psUniformColourTexture;
	stage.PushUniformAttribute(uDiffuseAtlas);
	
	outPipe.PushStage(stage);

	HPipeline hPipe = api.UploadPipeline(outPipe, &gPerDrawUniforms);

	hPipeline = hPipe;
	// hLightPos, hLightColour, hModel, hView, hProjection, hAmbientStrength, hSpecularStrength, hDiffuseStrength, hShininess, hDiffuseAtlas;
	hLightPos = api.GetPipelineUniformPropertyH(hPipe, uLightPos.Name, 0);
	hLightColour = api.GetPipelineUniformPropertyH(hPipe, uLightColour.Name, 0);
	hModel = api.GetPipelineUniformPropertyH(hPipe, uModel.Name, 0);
	hView = api.GetPipelineUniformPropertyH(hPipe, uView.Name, 0);
	hProjection = api.GetPipelineUniformPropertyH(hPipe, uProjection.Name, 0);
	hAmbientStrength = api.GetPipelineUniformPropertyH(hPipe, uAmbientStrength.Name, 0);
	hSpecularStrength = api.GetPipelineUniformPropertyH(hPipe, uSpecularStrength.Name, 0);
	hDiffuseStrength = api.GetPipelineUniformPropertyH(hPipe, uDiffuseStrength.Name, 0);
	hShininess = api.GetPipelineUniformPropertyH(hPipe, uShininess.Name, 0);
	hDiffuseAtlas = api.GetPipelineUniformPropertyH(hPipe, uDiffuseAtlas.Name, 0);
	api.RegisterPerDrawUniformSetter(hPipe, &PerDrawUnifomSetter);
	return hPipeline;
}

HDrawable GetTestPipelineDrawable(HMesh mesh, PerInstanceUniforms* pUniformPtr)
{
	const BackendAPI& api = Engine::GetAPI();
	HDrawable hDrawable = api.CreateDrawable(hPipeline, mesh, pUniformPtr);
	api.RegisterPerInstanceUniformSetter(hDrawable, &PerInstanceUniformSetter);
	return hDrawable;
}

PerDrawUniforms& GetPipelineUniforms()
{
	return gPerDrawUniforms;
}
