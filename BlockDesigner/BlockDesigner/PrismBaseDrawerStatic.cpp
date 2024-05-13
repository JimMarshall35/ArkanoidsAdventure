#include "pch.h"
#include "PrismBaseDrawerStatic.h"
#include "BasicTypedefs.h"

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

	NewBrush.CreateBrushIndirect(&b);
	NewPen.CreatePen(PS_SOLID, 1, outline);
	//HGDIOBJ oldPen = ::SelectObject(dc, &NewPen);
	HGDIOBJ oldBrush = ::SelectObject(dc, &NewBrush);

	//SelectObject return a NON NULL object
	Ellipse(dc, x - radius, y - radius, x + radius, y + radius);
	//dc.FloodFill(x, y, fillC);
	::SelectObject(dc, oldBrush);
	//::SelectObject(dc, oldPen);


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

	//CPaintDC dc(this);
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

	//dc.Gr
	//dc.FillSolidRect(0, 0, w, h, 0x00FFFFFF);
	//dc.FillRect()
	// Draw into m_hdcMem here
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

		/*[in] HDC        hDC,
		[in] const RECT * lprc,
		[in] HBRUSH     hbr*/
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
	float x = (float)((int)(tlbr[1] / (1.0f / (float)m_Increment))) * (1.0f / (float)m_Increment);
	//float x = ceilf(tlbr[1]);
	float y = (float)((int)(tlbr[0] / (1.0f / (float)m_Increment))) * (1.0f / (float)m_Increment);
	// ceilf(tlbr[0]);
	float initialX = x;
	float initialY = y;
	float increment = 1.0f / (float)m_Increment;
	const float tolerance = 10e-4;

	while (m_pCam->WorldPosCircleInScreenRect(x, y, CIRCLE_GRID_POINT_RADIUS_WORLDSPACE))
	{
		while (m_pCam->WorldPosCircleInScreenRect(x, y, CIRCLE_GRID_POINT_RADIUS_WORLDSPACE))
		{
			glm::vec2 ScreenSpace = m_pCam->WorldToScreenPos(x, y);
			if (AreSame(x, 0, tolerance) && AreSame(y, 0, tolerance))
			{
				Circle(m_hdcMem, ScreenSpace.x, ScreenSpace.y,
					m_pCam->WorldLengthToScreenLength(CIRCLE_GRID_POINT_RADIUS_WORLDSPACE),
					RGB(0, 255, 255),
					RGB(0, 255, 0));
			}
			else if (AreSame(fmod(x, 1.0f), 0.0f, tolerance) && AreSame(fmod(y, 1.0f), 0.0f, tolerance))
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
	}

	// draw thick border to hide the fact that circles aren't clipped to actual size of WND
	//cdc->FillSolidRect(-BORDER_THICKNESS, -BORDER_THICKNESS, w + 2*BORDER_THICKNESS, BORDER_THICKNESS, 0x00000000);
	//cdc->FillSolidRect(-BORDER_THICKNESS, -BORDER_THICKNESS, BORDER_THICKNESS, h * 2 * BORDER_THICKNESS, 0x00000000);
	//cdc->FillSolidRect(w, -BORDER_THICKNESS, BORDER_THICKNESS, h * 2 * BORDER_THICKNESS, 0x00000000);
	//cdc->FillSolidRect(-BORDER_THICKNESS, h, w + 2 * BORDER_THICKNESS, h + 2 * BORDER_THICKNESS, 0x00000000);

	// Transfer the off-screen DC to the screen
	CDC* cdc = BeginPaint(&p);

	BitBlt(cdc->GetSafeHdc(), 0, 0, w, h, m_hdcMem, 0, 0, SRCCOPY);


	EndPaint(&p);

}

BOOL PrismBaseDrawerStatic::OnEraseBkgnd(CDC* dc)
{
	return FALSE;
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
}

void PrismBaseDrawerStatic::GetWindowRect(LONG& t, LONG& l, LONG& b, LONG& r)
{
	CRect re;
	::GetWindowRect(GetSafeHwnd(), &re);
	t = re.top;
	l = re.left;
	b = re.bottom;
	r = re.right;

}
