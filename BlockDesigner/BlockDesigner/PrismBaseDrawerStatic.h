#pragma once
#include "afx.h"
#include "Camera2D.h"
#include "IDragAcceptor.h"
#include "IMouseMoveAcceptor.h"
#include "IMouseDownAcceptor.h"
#include "Poly2D.h"
#include <memory>
#include <glm.hpp>
#include <functional>

typedef std::function<void(const Poly2D&)> PolyCreatedDelegate;

/* -----------* D E F I N E S *----------- */

#define HOME_ZOOM_NUM_COLUMNS 3
#define CIRCLE_GRID_POINT_RADIUS_PX 5
#define CIRCLE_GRID_POINT_RADIUS_WORLDSPACE 0.04f

#define CAM_INITIAL_X 0.5f
#define CAM_INITIAL_Y 0.5f
#define CAM_INITIAL_ZOOM 200.0f
#define BORDER_THICKNESS 10

/* -----------*     E N D     *----------- */

class PrismBaseDrawerStatic : public CStatic,
	public IDragAcceptor,
	public IMouseMoveAcceptor,
	public IMouseDownAcceptor
{ 
	DECLARE_DYNAMIC(PrismBaseDrawerStatic)
public:
	PrismBaseDrawerStatic();
	~PrismBaseDrawerStatic();
	void SetNewZoom(float zoom);
	void SetNewIncrement(int newIncrement);
	void SetMeshCreatedDelegate(PolyCreatedDelegate onMeshCreated);

	// Inherited via IDragAcceptor
	virtual void StartDrag(const glm::vec2& pt, MouseButton btn) override;
	virtual void StopDrag(const glm::vec2& pt) override;
	virtual void UpdateDrag(const glm::vec2& lastPt) override;

	// Inherited via IMouseMoveAcceptor
	virtual void UpdateMousePos(const glm::vec2& lastPt) override;

	// Inherited via IMouseDownAcceptor
	virtual void MouseDown(const glm::vec2& pt, MouseButton btn) override;

	// Inherited via IGetWindowRect
	virtual void GetWindowRect(LONG& t, LONG& l, LONG& b, LONG& r) const override;

	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC*);
private:
	void GetWindowRectInternal(LONG& t, LONG& l, LONG& b, LONG& r) const;
	void CreateBrushesAndPens();
	void DeleteBrushesAndPens();


	void DrawPolyUnderConstruction() const;
	void DrawConstructedPoly() const;

private:
	std::unique_ptr<Camera2D> m_pCam;
	glm::vec2 m_LastDragPos;
	MouseButton m_MouseMoveButton;
	MouseButton m_AddPolyPointButton;
	MouseButton m_CompletePolyButton;
	bool m_bDraggingToMove = false;
	bool m_bConstructingPoly = false;
	int m_Increment = 1;
	HDC m_hdcMem = NULL;
	HBITMAP m_hbmMem = NULL;

	bool m_bAPointIsSelected = false;
	glm::vec2 m_SelectedPoint;

	glm::vec2 m_LastMouseScreenPos;
	
	/* The prism base polygon that this class serves to create */
	Poly2D m_Poly2D;

	/* brushes and pens */
	HBRUSH m_hOriginPointBrush = NULL;
	HBRUSH m_hOtherPointBrush = NULL;
	HBRUSH m_hBackgroundBrush = NULL;
	HPEN m_hGridPen = NULL;
	HPEN m_hUnselectedPointPen = NULL;
	HPEN m_hSelectedGridPointPen = NULL;
	HPEN m_hPolyUnderConstructionPen = NULL;
	HPEN m_hConstructedPolyPen = NULL;

	PolyCreatedDelegate m_OnPolyCreasted = nullptr;
private:
	DECLARE_MESSAGE_MAP()
};

