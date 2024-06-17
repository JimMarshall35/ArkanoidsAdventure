#include "pch.h"
#include "PrismView3DStatic.h"
#include "MiscFunctions.h"
#include <gl/glew.h>
#include <gl/wglew.h>
#include <gtc/matrix_transform.hpp>
#include <vector>
#include "Poly2D.h"
#include "ExtrudeParameters.h"
#include "SerializeObj.h"

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
"uniform vec3 objectColor;\n"
"uniform float ambientStrength;\n"
"uniform float specularStrength;\n"
"uniform float diffuseStrength;\n"
"uniform float shininess;\n"
"uniform sampler2D diffuseAtlas;\n"

"void main()\n"
"{\n"
	"// ambient""\n"
	"vec3 ambient = ambientStrength * lightColor;\n"
	"vec3 pixel_colour = objectColor;\n"

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

	//"vec3 result = ( ambient + diffuse + specular) * pixel_colour;\n"
	//"FragColor = vec4(result, 1.0);\n"
	"vec4 colour = texture(diffuseAtlas, Uv);\n"
	"FragColor = vec4((ambient + diffuse + specular),1.0) * colour;\n"
"}\n";

static bool OpenGlGPULoadTexture(const unsigned char* data, unsigned int width, unsigned int height, unsigned int* id)
{
	glGenTextures(1, id);
	glBindTexture(GL_TEXTURE_2D, *id);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // GL_NEAREST is the better filtering option for this game
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR); // as gives better more "pixelated" (less "smoothed out") textures
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	return true;
}

IMPLEMENT_DYNAMIC(PrismView3DStatic, CStatic)

BEGIN_MESSAGE_MAP(PrismView3DStatic, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

PrismView3DStatic::PrismView3DStatic()
	:m_Cam(glm::vec3{ 5.0f,0.0f,0.0f }, glm::vec3{ 0.0f,0.0f,0.0f }, glm::vec3{ 0.0f,0.0f,1.0f }),
	m_MeshShader()
{
	memset(&m_GLBuffers[0], 0, sizeof(m_GLBuffers));
}

PrismView3DStatic::~PrismView3DStatic()
{
	DeleteOpenGLContext();
}

BOOL PrismView3DStatic::Create(LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	BOOL rval = CStatic::Create(lpszText, dwStyle, rect, pParentWnd, nID);
	return rval;
}

void PrismView3DStatic::SetMesh(const Poly2D& poly, const ExtrudeParameters& params)
{
	m_Mesh.ExtrudeFromPoly2D(poly, params);
	m_bMeshSet = true;
	/* load into opengl */
	if (m_VAO)
	{
		glDeleteVertexArrays(1, &m_VAO);
		glDeleteBuffers(4, m_GLBuffers);
		glDeleteTextures(1, &m_Texture);
	}
	glGenBuffers(4, m_GLBuffers);
	glGenVertexArrays(1, &m_VAO);
	unsigned int postions = m_GLBuffers[POSITION_BUFFER_INDEX];
	unsigned int normals = m_GLBuffers[NORMAL_BUFFER_INDEX];
	unsigned int EBO = m_GLBuffers[EBO_INDEX];
	unsigned int uvs = m_GLBuffers[UV_BUFFER_INDEX];


	const std::vector<glm::vec3>& posData = m_Mesh.GetPositions();
	const std::vector<glm::vec3>& normData = m_Mesh.GetNormals();
	const std::vector<glm::vec2>& uvData = m_Mesh.GetUVs();
	const std::vector<int>& indexData = m_Mesh.GetIndices();

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, postions);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * posData.size(), &posData[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normData.size(), &normData[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, uvs);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * uvData.size(), &uvData[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indexData.size(), &indexData[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	Invalidate(FALSE);

	OpenGlGPULoadTexture(m_Mesh.GetTextureData(), m_Mesh.GetTextureWidth(), m_Mesh.GetTextureHeight(), &m_Texture);
	
	m_Cam.SetLookAt(m_Mesh.GetCentroid());

	SaveAsObj(m_Mesh, "mesh.obj");
}

void PrismView3DStatic::Clear()
{
	m_bMeshSet = false;
	Invalidate(FALSE);
}

void PrismView3DStatic::OnPaint()
{
	if (!m_hOpenGlCtx)
	{
		CreateOpenGLContext();
		InitRenderer();
	}

	CPaintDC dc(this);

	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (m_bMeshSet)
	{
		DrawMesh();
	}

	SwapBuffers(dc);

}

BOOL PrismView3DStatic::OnEraseBkgnd(CDC*)
{
    return 0;
}

void PrismView3DStatic::DeleteOpenGLContext()
{
	wglDeleteContext(m_hOpenGlCtx);
}

void PrismView3DStatic::CreateOpenGLContext()
{
	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
		PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
		32,                   // Colordepth of the framebuffer.
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		24,                   // Number of bits for the depthbuffer
		8,                    // Number of bits for the stencilbuffer
		0,                    // Number of Aux buffers in the framebuffer.
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};
	CDC* cDC = GetDC();

	HDC hdc = *cDC;
	int number = ChoosePixelFormat(hdc, &pfd);
	if (!SetPixelFormat(hdc, number, &pfd))
	{
		CStringW msgBoxText;
		msgBoxText.Format(_T("Could not set pixel format descriptor! %i"), GetLastError());
		AfxMessageBox(msgBoxText, MB_OK | MB_ICONSTOP);
		return;
	}
	m_hOpenGlCtx = wglCreateContext(hdc);
	if (!wglMakeCurrent(hdc, m_hOpenGlCtx))
	{
		AfxMessageBox(_T("wglMakeCurrent failed!"), MB_OK | MB_ICONSTOP);

		return;
	}
	GLenum glewVal = glewInit();
	if (glewVal != GLEW_OK)
	{
		CStringW msgBoxText;
		msgBoxText.Format(_T("glewInit failed! Error code: %i"), glewVal);
		AfxMessageBox(msgBoxText, MB_OK | MB_ICONSTOP);

		return;
	}

	if (WGLEW_EXT_swap_control)
	{
		wglSwapIntervalEXT(0);
	}

}


void PrismView3DStatic::SetProjectionMatrix()
{
	CRect re;
	::GetWindowRect(GetSafeHwnd(), &re);
	float w = re.right - re.left;
	float h = re.bottom - re.top;
	m_ViewportWidth = (float)w;
	m_ViewportHeight = (float)h;

	m_ProjectionMatrix = glm::perspective(RADIANS(90.0f), w / h, 0.1f, 100.0f);
	m_MeshShader.use();
	m_MeshShader.setMat4("projection", m_ProjectionMatrix);
}

void PrismView3DStatic::DrawMesh()
{
	m_MeshShader.use();
	glActiveTexture(GL_TEXTURE0); // activate the texture unit first before binding texture
	glBindTexture(GL_TEXTURE_2D, m_Texture);

	m_MeshShader.setMat4("view", m_Cam.GetViewMatrix());
	glBindVertexArray(m_VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_GLBuffers[EBO_INDEX]);
	glDrawElements(GL_TRIANGLES, m_Mesh.GetIndices().size(), GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void PrismView3DStatic::SetLight(const glm::vec3& position, const glm::vec3& colour)
{
	m_MeshShader.use();
	m_MeshShader.setVec3("lightPos", position);
	m_MeshShader.setVec3("lightColor", colour);
}

void PrismView3DStatic::SetMaterial(const glm::vec3& objectColour, float ambientStrength, float diffuseStrength, float specularStrength, float shininess)
{
	m_MeshShader.use();
	m_MeshShader.setVec3("objectColor", objectColour);
	m_MeshShader.setFloat("ambientStrength", ambientStrength);
	m_MeshShader.setFloat("diffuseStrength", diffuseStrength);
	m_MeshShader.setFloat("specularStrength", specularStrength);
	m_MeshShader.setFloat("shininess", shininess);
}

void PrismView3DStatic::SetMeshModelMatrix(const glm::vec3& position)
{
	m_MeshShader.use();
	glm::mat4 identityMat(1.0f);
	glm::mat4 model = glm::translate(identityMat, position);
	m_MeshShader.setMat4("model", model);

}

void PrismView3DStatic::InitRenderer()
{
	m_MeshShader = Shader(gMeshVert, gMeshFrag);
	SetProjectionMatrix();
	SetMeshModelMatrix({ 0,0,0 });
	SetLight({ 0,100.0f,100.0f }, { 1.0f,1.0f,1.0f });
	SetMaterial({ 1.0f,0.0f,0.0f }, 0.4f, 0.5f, 0.2f, 8.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void PrismView3DStatic::GetWindowRect(LONG& t, LONG& l, LONG& b, LONG& r) const
{
	CRect re;
	::GetWindowRect(GetSafeHwnd(), &re);
	t = re.top;
	l = re.left;
	b = re.bottom;
	r = re.right;
}

void PrismView3DStatic::StartDrag(const glm::vec2& pt, MouseButton btn)
{
	m_LastDragPos = pt;
}

void PrismView3DStatic::StopDrag(const glm::vec2& pt)
{
}


// https://asliceofrendering.com/camera/2019/11/30/ArcballCamera/
void PrismView3DStatic::UpdateDrag(const glm::vec2& lastPt)
{
	float xPos = lastPt.x;
	float yPos = lastPt.y;
	// Get the homogenous position of the camera and pivot point
	glm::vec4 position(m_Cam.GetEye().x, m_Cam.GetEye().y, m_Cam.GetEye().z, 1);
	glm::vec4 pivot(m_Cam.GetLookAt().x, m_Cam.GetLookAt().y, m_Cam.GetLookAt().z, 1);

	// step 1 : Calculate the amount of rotation given the mouse movement.
	float deltaAngleX = (2 * M_PI / m_ViewportWidth); // a movement from left to right = 2*PI = 360 deg
	float deltaAngleY = (M_PI / m_ViewportHeight);  // a movement from top to bottom = PI = 180 deg
	float xAngle = (m_LastDragPos.x - xPos) * deltaAngleX;
	float yAngle = (m_LastDragPos.y - yPos) * deltaAngleY;

	static const glm::vec3 up{ 0.0f,0.0f,1.0f };
	// Extra step to handle the problem when the camera direction is the same as the up vector
	float cosAngle = glm::dot(m_Cam.GetViewDir(), up);
	if (cosAngle * sgn(yAngle) > 0.99f)
		yAngle = 0;

	// step 2: Rotate the camera around the pivot point on the first axis.
	glm::mat4x4 rotationMatrixX(1.0f);
	rotationMatrixX = glm::rotate(rotationMatrixX, xAngle, up);
	position = (rotationMatrixX * (position - pivot)) + pivot;

	// step 3: Rotate the camera around the pivot point on the second axis.
	glm::mat4x4 rotationMatrixY(1.0f);
	rotationMatrixY = glm::rotate(rotationMatrixY, yAngle, m_Cam.GetRightVector());
	glm::vec3 finalPosition = (rotationMatrixY * (position - pivot)) + pivot;

	// Update the camera view (we keep the same lookat and the same up vector)
	m_Cam.SetCameraView(finalPosition, m_Cam.GetLookAt(), up);

	// Update the mouse position for the next rotation
	m_LastDragPos.x = xPos;
	m_LastDragPos.y = yPos;

	Invalidate(FALSE);

}
