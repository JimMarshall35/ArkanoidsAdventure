#include "pch.h"
#include "PrismBaseDrawerStatic.h"
#include "BasicTypedefs.h"
#include "MiscFunctions.h"
#include "PrismMesh.h"
#include "SerializeObj.h"
#include <vector>

IMPLEMENT_SERIAL(PrismBaseDrawerStatic, CStatic, 1)

BEGIN_MESSAGE_MAP(PrismBaseDrawerStatic, CStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

static void Circle(HDC dc, float x, float y, float radius,
	HPEN outline,
	HBRUSH fillC)
{
	HGDIOBJ oldBrush = ::SelectObject(dc, fillC);
	HGDIOBJ oldPen = ::SelectObject(dc, outline);
	Ellipse(dc, x - radius, y - radius, x + radius, y + radius);
	::SelectObject(dc, oldPen);
	::SelectObject(dc, oldBrush);
}

PrismBaseDrawerStatic::PrismBaseDrawerStatic()
	:m_MouseMoveButton(MouseButton::Middle),
	m_AddPolyPointButton(MouseButton::Left),
	m_CompletePolyButton(MouseButton::Right)
{
	CreateBrushesAndPens();
}

PrismBaseDrawerStatic::~PrismBaseDrawerStatic()
{
	DeleteBrushesAndPens();
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

void PrismBaseDrawerStatic::SetMeshCreatedDelegate(PolyCreatedDelegate onMeshCreated)
{
	m_OnPolyCreasted = onMeshCreated;
}

void PrismBaseDrawerStatic::Clear()
{
	m_Poly2D.Clear();
	InvalidateRect(NULL);
}

void PrismBaseDrawerStatic::Serialize(CArchive& ar)
{
	static const WORD sVersion = 1;
	if (ar.IsStoring())
	{
		ar << sVersion;
		BYTE b = m_bConstructingPoly;
		ar << b;
		m_Poly2D.Serialize(ar);
	}
	else
	{
		WORD ver;
		ar >> ver;
		switch (ver)
		{
		case 1:
		{
			BYTE b;
			ar >> b;
			m_bConstructingPoly = b;
			m_Poly2D.Serialize(ar);
			InvalidateRect(NULL);
		}
			break;
		default:
			break;
		}
	}
}


void PrismBaseDrawerStatic::OnPaint()
{
	CRect rect;
	GetClientRect(&rect);
	float w = rect.Width();
	float h = rect.Height();

	if (!m_hbmMem)
	{
		// Create an off-screen DC for double-buffering
		m_hbmMem = CreateCompatibleBitmap(GetDC()->GetSafeHdc(), w, h);
		m_hdcMem = CreateCompatibleDC(GetDC()->GetSafeHdc());
	}

	HANDLE hOld = ::SelectObject(m_hdcMem, m_hbmMem);

	RECT r = {
		0, 0,
		w, h
	};
	HGDIOBJ hbrushOld = ::SelectObject(m_hdcMem, (HGDIOBJ)m_hBackgroundBrush);
	FillRect(
		m_hdcMem,
		&r,
		m_hBackgroundBrush
	);
	::SelectObject(m_hdcMem, hbrushOld);

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
	POINT pt;

	while (m_pCam->WorldPosCircleInScreenRect(x, y, CIRCLE_GRID_POINT_RADIUS_WORLDSPACE))
	{
		/* Draw horizontal grid lines */
		if (AreSame(round(y) - y, 0.0, tolerance))
		{
			glm::vec2 pt1 = m_pCam->WorldToScreenPos(tlbr[1], y);
			glm::vec2 pt2 = m_pCam->WorldToScreenPos(tlbr[3], y);

			HGDIOBJ oldPen = ::SelectObject(m_hdcMem, m_hGridPen);

			MoveToEx(m_hdcMem, pt1.x, pt1.y, &pt);
			LineTo(m_hdcMem, pt2.x, pt2.y);
			::SelectObject(m_hdcMem, oldPen);
		}

		while (m_pCam->WorldPosCircleInScreenRect(x, y, CIRCLE_GRID_POINT_RADIUS_WORLDSPACE))
		{
			/* Draw vertical grid lines*/
			if (onRow == 0 && AreSame(round(x) - x, 0.0, tolerance))
			{
				HGDIOBJ oldPen = ::SelectObject(m_hdcMem, m_hGridPen);
				glm::vec2 pt1 = m_pCam->WorldToScreenPos(x, tlbr[0]);
				glm::vec2 pt2 = m_pCam->WorldToScreenPos(x, tlbr[2]);
				MoveToEx(m_hdcMem, pt1.x, pt1.y, &pt);
				LineTo(m_hdcMem, pt2.x, pt2.y);
				::SelectObject(m_hdcMem, oldPen);
			}

			/* Draw points */
			HPEN pointOutlinePen = (m_bAPointIsSelected && AreSame(x, m_SelectedPoint.x, tolerance) && AreSame(y, m_SelectedPoint.y, tolerance)) ? 
				m_hSelectedGridPointPen :
				m_hUnselectedPointPen;

			glm::vec2 ScreenSpace = m_pCam->WorldToScreenPos(x, y);
			if (AreSame(x, 0, tolerance) && AreSame(y, 0, tolerance))
			{
				Circle(m_hdcMem, ScreenSpace.x, ScreenSpace.y,
					m_pCam->WorldLengthToScreenLength(CIRCLE_GRID_POINT_RADIUS_WORLDSPACE),
					pointOutlinePen,
					m_hOriginPointBrush);
			}
			else if (AreSame(round(x) - x, 0.0f, tolerance) && 
				AreSame(round(y) - y, 0.0f, tolerance))
			{
				Circle(m_hdcMem, ScreenSpace.x, ScreenSpace.y, m_pCam->WorldLengthToScreenLength(CIRCLE_GRID_POINT_RADIUS_WORLDSPACE),
					pointOutlinePen,
					m_hOtherPointBrush);
			}
			else
			{
				Circle(m_hdcMem, ScreenSpace.x, ScreenSpace.y, m_pCam->WorldLengthToScreenLength(CIRCLE_GRID_POINT_RADIUS_WORLDSPACE / 2.0f),
					pointOutlinePen,
					m_hOtherPointBrush);
			}
			x += increment;
		}
		x = initialX;
		y += increment;
		onRow++;
	}

	if (m_bConstructingPoly)
	{
		DrawPolyUnderConstruction();
	}
	else if (m_Poly2D.HasPoints())
	{
		DrawConstructedPoly();
	}
	
	// Transfer the off-screen DC to the screen
	PAINTSTRUCT p;
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

void PrismBaseDrawerStatic::CreateBrushesAndPens() 
{
	m_hOriginPointBrush = CreateSolidBrush(RGB(0, 255, 0));
	m_hOtherPointBrush = CreateSolidBrush(RGB(255, 255, 255));
	m_hBackgroundBrush = CreateSolidBrush(RGB(255, 255, 255));

	m_hGridPen = ::CreatePen(
		PS_DASH,
		1,
		RGB(128, 128, 128)
	);

	m_hUnselectedPointPen = ::CreatePen(
		PS_SOLID,
		1,
		RGB(0,0,0)
	);

	m_hSelectedGridPointPen = ::CreatePen(
		PS_SOLID,
		2,
		RGB(255, 0, 0)
	);

	m_hPolyUnderConstructionPen = ::CreatePen(
		PS_SOLID,
		2,
		RGB(0, 0, 255)
	);

	m_hConstructedPolyPen = ::CreatePen(
		PS_SOLID,
		2,
		RGB(0, 255, 255)
	);
}

void PrismBaseDrawerStatic::DeleteBrushesAndPens()
{
	::DeleteObject(m_hOriginPointBrush);
	::DeleteObject(m_hOtherPointBrush);
	::DeleteObject(m_hBackgroundBrush);
	::DeleteObject(m_hGridPen);
	::DeleteObject(m_hUnselectedPointPen);
	::DeleteObject(m_hUnselectedPointPen);
	::DeleteObject(m_hSelectedGridPointPen);
	::DeleteObject(m_hPolyUnderConstructionPen);
	::DeleteObject(m_hConstructedPolyPen);
}

void PrismBaseDrawerStatic::DrawPolyUnderConstruction() const
{
	const std::vector<glm::vec2>& polyPoints = m_Poly2D.GetPoints();
	ASSERT(polyPoints.size() > 0);
	glm::vec2 lastPt;
	HGDIOBJ oldPen = ::SelectObject(m_hdcMem, m_hPolyUnderConstructionPen);

	glm::vec2 firstPtScreen = m_pCam->WorldToScreenPos(polyPoints[0].x, polyPoints[0].y);
	POINT _;
	MoveToEx(m_hdcMem, firstPtScreen.x, firstPtScreen.y, &_);

	for (int i = 1; i < polyPoints.size(); i++)
	{
		glm::vec2 ptScreen = m_pCam->WorldToScreenPos(polyPoints[i].x, polyPoints[i].y);
		LineTo(m_hdcMem, ptScreen.x, ptScreen.y);
	}
	// draw line from lastPt to mousePosition
	LineTo(m_hdcMem, m_LastMouseScreenPos.x, m_LastMouseScreenPos.y);

	::SelectObject(m_hdcMem, oldPen);
}

void PrismBaseDrawerStatic::DrawConstructedPoly() const
{
	const std::vector<glm::vec2>& polyPoints = m_Poly2D.GetPoints();
	const std::vector<int>& polyIndices = m_Poly2D.GetIndices();

	ASSERT(polyPoints.size() > 0);
	ASSERT(polyIndices.size() > 0);
	ASSERT(polyIndices.size() % 3 == 0);

	HGDIOBJ oldPen = ::SelectObject(m_hdcMem, m_hConstructedPolyPen);
	POINT _;

	for (int i = 0; i < polyIndices.size(); i += 3)
	{
		const glm::vec2& t1 = polyPoints[polyIndices[i]];
		const glm::vec2& t2 = polyPoints[polyIndices[i + 1]];
		const glm::vec2& t3 = polyPoints[polyIndices[i + 2]];
		glm::vec2 s1 = m_pCam->WorldToScreenPos(t1.x, t1.y);
		glm::vec2 s2 = m_pCam->WorldToScreenPos(t2.x, t2.y);
		glm::vec2 s3 = m_pCam->WorldToScreenPos(t3.x, t3.y);
		MoveToEx(m_hdcMem, s1.x, s1.y, &_);
		LineTo(m_hdcMem, s2.x, s2.y);
		LineTo(m_hdcMem, s3.x, s3.y);
		LineTo(m_hdcMem, s1.x, s1.y);
	}

	::SelectObject(m_hdcMem, oldPen);
}

void PrismBaseDrawerStatic::UpdateMousePos(const glm::vec2& lastPt)
{
	/*
		THIS IS BROKEN FOR POINTS IN THE TOPLEFT QUADRANT (-x,-y)
		doesn't detect the point in the topleft.
		Usable but annoying
	*/
	glm::vec2 world = m_pCam->MouseScreenPosToWorld(lastPt.x, lastPt.y);
	float incr = 1.0f / (float)m_Increment;
	/* top left point */
	float lx = world.x - fmod(world.x, incr);
	float ly = world.y - fmod(world.y, incr);
	glm::vec2 tl = { lx,ly };
	/* four points surrounding mouse pos */
	glm::vec2 points[4] = {
		tl,
		tl + glm::vec2{incr, 0},
		tl + glm::vec2{0, incr},
		tl + glm::vec2{incr, incr},
	};
	float circleRadius = CIRCLE_GRID_POINT_RADIUS_WORLDSPACE;
	bool bPointPreviouslySelected = m_bAPointIsSelected;
	m_bAPointIsSelected = false;
	/* check if any of the 4 points are close enough to be considered hoverred */
	for (int i = 0; i < 4; i++)
	{
		const glm::vec2& pt = points[i];
		float dist = glm::distance(pt, world);
		if (dist <= circleRadius)
		{
			m_bAPointIsSelected = true;
			m_SelectedPoint = pt;
			break;
		}
	}
	/* if change from selected or deselected (or vice versa), trigger redraw */
	if (m_bConstructingPoly || (bPointPreviouslySelected != m_bAPointIsSelected))
	{
		InvalidateRect(NULL);
	}
	m_LastMouseScreenPos = lastPt;
}

static bool PointIsAllowed(const glm::vec2& point2Add, const std::vector<glm::vec2>& existingPoints)
{
	size_t size = existingPoints.size();
	//ASSERT(size);
	for (const glm::vec2& pt : existingPoints)
	{
		if (PointIsSame(point2Add, pt))
		{
			return false;
		}
	}
	if (size > 1)
	{
		/*glm::vec2 newSeg1 = existingPoints[size - 1];
		const glm::vec2& newSeg2 = point2Add;
		for (int i = 0; i < size - 1; i++)
		{
			if (DoIntersect(newSeg1, newSeg2, existingPoints[i], existingPoints[i + 1]))
			{
				return false;
			}
		}*/
	}
	
	return true;
}

void PrismBaseDrawerStatic::MouseDown(const glm::vec2& pt, MouseButton btn)
{
	glm::vec2 worldPos = m_pCam->MouseScreenPosToWorld(pt.x, pt.y);
	if (btn == m_AddPolyPointButton)
	{
		m_bConstructingPoly = true;
		glm::vec2 point2Add = m_bAPointIsSelected ? m_SelectedPoint : worldPos;
		if (PointIsAllowed(point2Add, m_Poly2D.GetPoints()))
		{
			m_Poly2D.PushPoint(point2Add);
			if (m_bAPointIsSelected)
			{
				// point has snapped to one of the circles so redraw
				InvalidateRect(NULL);
			}
		}
		else
		{
			/* make polygon flash to show that you cn't place a point here */
		}
	}
	else if (btn == m_CompletePolyButton)
	{
		if (m_bConstructingPoly)
		{
			m_bConstructingPoly = false;
			m_Poly2D.Triangulate();

			/*PrismMesh pm;
			pm.ExtrudeFromPoly2D(m_Poly2D, 1.0f);
			SaveAsObj(pm, "mesh.obj");*/
			if (m_OnPolyCreasted)
			{
				m_OnPolyCreasted(m_Poly2D);
			}

			InvalidateRect(NULL);
		}
	}
}

void PrismBaseDrawerStatic::GetWindowRect(LONG& t, LONG& l, LONG& b, LONG& r) const
{
	GetWindowRectInternal(t, l, b, r);
}

void PrismBaseDrawerStatic::StartDrag(const glm::vec2& pt, MouseButton btn)
{
	if (m_MouseMoveButton == btn)
	{
		ASSERT(!m_bDraggingToMove);
		m_bDraggingToMove = true;
		m_LastDragPos = pt;
	}
	
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