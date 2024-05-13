#pragma once
#include "afx.h"
#include "Camera2D.h"
#include "IDragAcceptor.h"
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

class PrismBaseDrawerStatic : public CStatic, public IDragAcceptor
{ 
	DECLARE_DYNAMIC(PrismBaseDrawerStatic)
public:
	PrismBaseDrawerStatic();
	void SetNewZoom(float zoom);
	void SetNewIncrement(int newIncrement);

	// Inherited via IDragAcceptor
	virtual void StartDrag(const glm::vec2& pt, MouseButton btn) override;
	virtual void StopDrag(const glm::vec2& pt) override;
	virtual void UpdateDrag(const glm::vec2& lastPt) override;
	virtual void GetWindowRect(LONG& t, LONG& l, LONG& b, LONG& r) override;

	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC*);
protected:
	std::unique_ptr<Camera2D> m_pCam;
	glm::vec2 m_LastDragPos;
	MouseButton m_MouseMoveButton;
	bool m_bDraggingToMove = false;
	int m_Increment = 1;
	HDC m_hdcMem = NULL;
	HBITMAP m_hbmMem = NULL;

protected:
	DECLARE_MESSAGE_MAP()

	

};

