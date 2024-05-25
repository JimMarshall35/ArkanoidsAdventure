#include "pch.h"
#include "PrismBaseDrawerStatic.h"
#include "BasicTypedefs.h"
#include <vector>

IMPLEMENT_DYNAMIC(PrismBaseDrawerStatic, CStatic)

BEGIN_MESSAGE_MAP(PrismBaseDrawerStatic, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

static void Circle(HDC dc, float x, float y, float radius,
	COLORREF outline,
	COLORREF fillC)
{
	CPen NewPen;
	CBrush NewBrush;
	LOGBRUSH b;
	b.lbColor = fillC;
	b.lbStyle = BS_SOLID;

	HBRUSH newBrush = CreateSolidBrush(fillC);

	HGDIOBJ oldBrush = ::SelectObject(dc, newBrush);

	//SelectObject return a NON NULL object
	Ellipse(dc, x - radius, y - radius, x + radius, y + radius);
	//dc.FloodFill(x, y, fillC);
	::SelectObject(dc, oldBrush);
	//::SelectObject(dc, oldPen);
	DeleteObject(newBrush);

}

PrismBaseDrawerStatic::PrismBaseDrawerStatic()
	:m_MouseMoveButton(MouseButton::Middle)
{
}

void PrismBaseDrawerStatic::SetNewZoom(float zoom)
{
	ASSERT(m_pCam);
	m_pCam->Zoom = zoom;
}

void PrismBaseDrawerStatic::SetNewIncrement(int newIncrement)
{
	m_Increment = newIncrement;
}


void PrismBaseDrawerStatic::OnPaint()
{
	CRect rect;
	GetClientRect(&rect);
	float w = rect.Width();
	float h = rect.Height();
	PAINTSTRUCT p, memP;
	HANDLE      hOld;

	if (!m_hbmMem)
	{
		// Create an off-screen DC for double-buffering
		m_hbmMem = CreateCompatibleBitmap(GetDC()->GetSafeHdc(), w, h);
		m_hdcMem = CreateCompatibleDC(GetDC()->GetSafeHdc());


	}
	hOld = ::SelectObject(m_hdcMem, m_hbmMem);

	RECT r = {
		0, 0,
		w, h
	};
	HBRUSH newBrush = CreateSolidBrush(RGB(255, 255, 255));
	HGDIOBJ hbrushOld = ::SelectObject(m_hdcMem, (HGDIOBJ)newBrush);
	FillRect(
		m_hdcMem,
		&r,
		newBrush
	);
	::SelectObject(m_hdcMem, hbrushOld);
	DeleteObject(newBrush);



	if (!m_pCam)
	{
		m_pCam = std::make_unique<Camera2D>(
			w,
			h,
			"PrismBaseDrawerCam",
			glm::vec2{ CAM_INITIAL_X, CAM_INITIAL_Y },
			CAM_INITIAL_ZOOM
			);
	}
	glm::vec4 tlbr = m_pCam->GetTLBR();
	float x = tlbr[1] - fmod((long double)tlbr[1], (long double)(1.0f / (float)m_Increment));
	float y = tlbr[0] - fmod((long double)tlbr[0], (long double)(1.0f / (float)m_Increment));
	
	/* 
		these 2 ifs ensure that the grid is still drawn when the camera top left is in the  
		+ve (bottom right) quadrant
	*/
	if (tlbr[1] > 0.0f)
	{
		x += 1.0f / (float)m_Increment;
	}
	if (tlbr[0] > 0.0f)
	{
		y += 1.0f / (float)m_Increment;
	}

	float worldW = tlbr[3] - tlbr[1];

	float initialX = x;
	float initialY = y;
	float increment = 1.0f / (float)m_Increment;
	const float tolerance = 10e-3;

	int onRow = 0;

	HPEN gridPen = ::CreatePen(
		PS_DASH,
		1,
		RGB(128, 128, 128)
		//	[in] int      iStyle,
		//	[in] int      cWidth,
		//	[in] COLORREF color
	);

	POINT pt;

	while (m_pCam->WorldPosCircleInScreenRect(x, y, CIRCLE_GRID_POINT_RADIUS_WORLDSPACE))
	{
		if (AreSame(round(y) - y, 0.0, tolerance))
		{
			glm::vec2 pt1 = m_pCam->WorldToScreenPos(tlbr[1], y);
			glm::vec2 pt2 = m_pCam->WorldToScreenPos(tlbr[3], y);

			HGDIOBJ oldPen = ::SelectObject(m_hdcMem, gridPen);

			MoveToEx(m_hdcMem, pt1.x, pt1.y, &pt);
			LineTo(m_hdcMem, pt2.x, pt2.y);
			::SelectObject(m_hdcMem, oldPen);
		}

		while (m_pCam->WorldPosCircleInScreenRect(x, y, CIRCLE_GRID_POINT_RADIUS_WORLDSPACE))
		{
			if (onRow == 0 && AreSame(round(x) - x, 0.0, tolerance))
			{
				HGDIOBJ oldPen = ::SelectObject(m_hdcMem, gridPen);
				glm::vec2 pt1 = m_pCam->WorldToScreenPos(x, tlbr[0]);
				glm::vec2 pt2 = m_pCam->WorldToScreenPos(x, tlbr[2]);
				MoveToEx(m_hdcMem, pt1.x, pt1.y, &pt);
				LineTo(m_hdcMem, pt2.x, pt2.y);
				::SelectObject(m_hdcMem, oldPen);
			}

			glm::vec2 ScreenSpace = m_pCam->WorldToScreenPos(x, y);
			if (AreSame(x, 0, tolerance) && AreSame(y, 0, tolerance))
			{
				Circle(m_hdcMem, ScreenSpace.x, ScreenSpace.y,
					m_pCam->WorldLengthToScreenLength(CIRCLE_GRID_POINT_RADIUS_WORLDSPACE),
					RGB(0, 255, 255),
					RGB(0, 255, 0));
			}
			else if (AreSame(round(x) - x, 0.0f, tolerance) && 
				AreSame(round(y) - y, 0.0f, tolerance))
			{
				Circle(m_hdcMem, ScreenSpace.x, ScreenSpace.y, m_pCam->WorldLengthToScreenLength(CIRCLE_GRID_POINT_RADIUS_WORLDSPACE),
					RGB(0, 0, 0),
					RGB(255, 255, 255));
			}
			else
			{
				Circle(m_hdcMem, ScreenSpace.x, ScreenSpace.y, m_pCam->WorldLengthToScreenLength(CIRCLE_GRID_POINT_RADIUS_WORLDSPACE / 2.0f),
					RGB(0, 0, 0),
					RGB(255, 255, 255));
			}


			x += increment;
		}
		x = initialX;
		y += increment;
		onRow++;
	}

	::DeleteObject(gridPen);
	
	// Transfer the off-screen DC to the screen
	CDC* cdc = BeginPaint(&p);

	BitBlt(cdc->GetSafeHdc(), 0, 0, w, h, m_hdcMem, 0, 0, SRCCOPY);


	EndPaint(&p);

}

BOOL PrismBaseDrawerStatic::OnEraseBkgnd(CDC* dc)
{
	return FALSE;
}

void PrismBaseDrawerStatic::GetWindowRectInternal(LONG& t, LONG& l, LONG& b, LONG& r) const
{
	CRect re;
	::GetWindowRect(GetSafeHwnd(), &re);
	t = re.top;
	l = re.left;
	b = re.bottom;
	r = re.right;
}

void PrismBaseDrawerStatic::UpdateMousePos(const glm::vec2& lastPt)
{
}

void PrismBaseDrawerStatic::GetWindowRectForMove(LONG& t, LONG& l, LONG& b, LONG& r) const
{
	GetWindowRectInternal(t, l, b, r);
}

void PrismBaseDrawerStatic::StartDrag(const glm::vec2& pt, MouseButton btn)
{
	if (m_MouseMoveButton == btn)
	{
		ASSERT(!m_bDraggingToMove);
		m_bDraggingToMove = true;
	}
	m_LastDragPos = pt;
}

void PrismBaseDrawerStatic::StopDrag(const glm::vec2& pt)
{
	m_bDraggingToMove = false;
}

void PrismBaseDrawerStatic::UpdateDrag(const glm::vec2& lastPt)
{
	if (m_bDraggingToMove)
	{
		glm::vec2 thisWorld = m_pCam->MouseScreenPosToWorld(lastPt.x, lastPt.y);
		glm::vec2 lastWorld = m_pCam->MouseScreenPosToWorld(m_LastDragPos.x, m_LastDragPos.y);
		glm::vec2 camMoveVector = thisWorld - lastWorld;

		m_pCam->FocusPosition.x += -camMoveVector.x;
		m_pCam->FocusPosition.y += -camMoveVector.y;
		m_LastDragPos = lastPt;
		InvalidateRect(NULL);
	}
	else
	{

	}
}

void PrismBaseDrawerStatic::GetWindowRectForDrag(LONG& t, LONG& l, LONG& b, LONG& r) const
{
	GetWindowRectInternal(t, l, b, r);
}
