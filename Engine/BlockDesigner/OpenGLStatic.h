#pragma once
#include <afxwin.h>
class OpenGLStatic :
    public CStatic
{
protected:
    void DeleteOpenGLContext();
    void CreateOpenGLContext();

    virtual void ClearFrameBuffers();
    virtual void InitRenderer() = 0;
    virtual void Draw() = 0;


    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC*);

protected:
    HGLRC m_hOpenGlCtx = NULL;
    DECLARE_MESSAGE_MAP()

};

