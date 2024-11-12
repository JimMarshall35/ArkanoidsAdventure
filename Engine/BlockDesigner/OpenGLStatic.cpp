#include "pch.h"
#include "OpenGLStatic.h"
#include <gl/glew.h>
#include <gl/wglew.h>

BEGIN_MESSAGE_MAP(OpenGLStatic, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

void OpenGLStatic::DeleteOpenGLContext()
{
	wglDeleteContext(m_hOpenGlCtx);
}

void OpenGLStatic::CreateOpenGLContext()
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

void OpenGLStatic::ClearFrameBuffers()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


void OpenGLStatic::OnPaint()
{
	if (!m_hOpenGlCtx)
	{
		CreateOpenGLContext();
		InitRenderer();
	}

	CPaintDC dc(this);

	ClearFrameBuffers();

	Draw();

	SwapBuffers(dc);
}

BOOL OpenGLStatic::OnEraseBkgnd(CDC*)
{
    return 0;
}
