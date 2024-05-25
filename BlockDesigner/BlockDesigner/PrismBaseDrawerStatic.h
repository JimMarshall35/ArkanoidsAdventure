#pragma once
#include "afx.h"
#include "Camera2D.h"
#include "IDragAcceptor.h"
#include "IMouseMoveAcceptor.h"
#include <memory>
#include <glm.hpp>

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
	public IMouseMoveAcceptor
{ 
	DECLARE_DYNAMIC(PrismBaseDrawerStatic)
public:
	PrismBaseDrawerStatic();
	~PrismBaseDrawerStatic();
	void SetNewZoom(float zoom);
	void SetNewIncrement(int newIncrement);

	// Inherited via IDragAcceptor
	virtual void StartDrag(const glm::vec2& pt, MouseButton btn) override;
	virtual void StopDrag(const glm::vec2& pt) override;
	virtual void UpdateDrag(const glm::vec2& lastPt) override;
	virtual void GetWindowRectForDrag(LONG& t, LONG& l, LONG& b, LONG& r) const override;

	// Inherited via IMouseMoveAcceptor
	virtual void UpdateMousePos(const glm::vec2& lastPt) override;
	virtual void GetWindowRectForMove(LONG& t, LONG& l, LONG& b, LONG& r) const override;

	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC*);
private:
	void GetWindowRectInternal(LONG& t, LONG& l, LONG& b, LONG& r) const;
	void CreateBrushesAndPens();
	void DeleteBrushesAndPens();
private:
	std::unique_ptr<Camera2D> m_pCam;
	glm::vec2 m_LastDragPos;
	MouseButton m_MouseMoveButton;
	bool m_bDraggingToMove = false;
	int m_Increment = 1;
	HDC m_hdcMem = NULL;
	HBITMAP m_hbmMem = NULL;

	bool m_bAPointIsSelected = false;
	glm::vec2 m_SelectedPoint;

	HBRUSH m_hOriginPointBrush;
	HBRUSH m_hOtherPointBrush;
	HBRUSH m_hBackgroundBrush;
	HPEN m_hGridPen;
	HPEN m_hUnselectedPointPen;
	HPEN m_hSelectedGridPointPen;

private:
	DECLARE_MESSAGE_MAP()

	


	

};

