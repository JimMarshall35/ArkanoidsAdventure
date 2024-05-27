#pragma once
#include "afx.h"

class PrismView3DStatic : public CStatic
{
	DECLARE_DYNAMIC(PrismView3DStatic)
public:
	~PrismView3DStatic();
	virtual BOOL Create(LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID = 0xffff) override;
private:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC*);

	void DeleteOpenGLContext();
	void CreateOpenGLContext();

private:
	HGLRC m_hOpenGlCtx = NULL;
private:
	DECLARE_MESSAGE_MAP()
};

