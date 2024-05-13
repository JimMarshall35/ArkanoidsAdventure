
// BlockDesignerDlg.h : header file
//

#pragma once
#include "PrismBaseDrawerStatic.h"
#include "DragDispatcher.h"


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
	CSliderCtrl m_ctlPrismStaticZoomSlider;
	CSpinButtonCtrl m_ctlIncrementSpinBtn;
	CEdit m_ctlIncremetnEdit;

	DragDispatcher m_DragDispatcher;

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


	void HandlePrismStaticZoomScroll(UINT nSBCode, UINT nPos);

	DECLARE_MESSAGE_MAP()
};
