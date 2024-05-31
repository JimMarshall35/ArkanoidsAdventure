
// BlockDesignerDlg.h : header file
//

#pragma once
#include "PrismBaseDrawerStatic.h"
#include "PrismView3DStatic.h"
#include "MouseEventDispatcher.h"
#include "ExtrudeParameters.h"

// CBlockDesignerDlg dialog
class CBlockDesignerDlg : public CDialogEx
{
// Construction
public:
	CBlockDesignerDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BLOCKDESIGNER_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

// Implementation
protected:
	HICON m_hIcon;
	PrismBaseDrawerStatic m_ctlPrismBaseDrawer;
	PrismView3DStatic m_ctlPrismView3DStatic;
	CSliderCtrl m_ctlPrismStaticZoomSlider;
	CSpinButtonCtrl m_ctlIncrementSpinBtn;
	CEdit m_ctlIncremetnEdit;

	CString m_ctlTopCapScaleX;
	CString m_ctlTopCapScaleY;
	CString m_ctlTopCapOffsetX;
	CString m_ctlTopCapOffsetY;

	CString m_ctlBottomCapScaleX;
	CString m_ctlBottomCapScaleY;
	CString m_ctlBottomCapOffsetX;
	CString m_ctlBottomCapOffsetY;

	CString m_ctlExtrudeAmount;

	ExtrudeParameters m_ExtrudeParameters;


	MouseEventDispatcher m_DragDispatcher;

protected:
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnMButtonDown(UINT, CPoint);
	afx_msg void OnMButtonUp(UINT, CPoint);

	afx_msg void OnLButtonDown(UINT, CPoint);
	afx_msg void OnLButtonUp(UINT, CPoint);

	afx_msg void OnRButtonDown(UINT, CPoint);
	afx_msg void OnRButtonUp(UINT, CPoint);


	afx_msg void OnMouseMove(UINT, CPoint);

	afx_msg HCURSOR OnQueryDragIcon();

	afx_msg void OnIncrementButtonPress();

	afx_msg void OnChangeTopCapScaleX();
	afx_msg void OnChangeTopCapScaleY();
	afx_msg void OnChangeTopCapOffsetX();
	afx_msg void OnChangeTopCapOffsetY(); 
	
	afx_msg void OnChangeBottomCapScaleX();
	afx_msg void OnChangeBottomCapScaleY();
	afx_msg void OnChangeBottomCapOffsetX();
	afx_msg void OnChangeBottomCapOffsetY();

	afx_msg void OnChangeExtrudeAmount();

	void HandlePrismStaticZoomScroll(UINT nSBCode, UINT nPos);

	bool ValidateAndSetFloat(CString& input, float& output, const CString& fallbackVal);

	DECLARE_MESSAGE_MAP()
};
