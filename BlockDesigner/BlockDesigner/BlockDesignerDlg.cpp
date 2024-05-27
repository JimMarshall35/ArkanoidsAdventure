
// BlockDesignerDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "BlockDesigner.h"
#include "BlockDesignerDlg.h"
#include "afxdialogex.h"
#include "resource.h"
#include <winuser.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CBlockDesignerDlg dialog



CBlockDesignerDlg::CBlockDesignerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_BLOCKDESIGNER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBlockDesignerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_PRISM_BASE_DRAWER_STATIC, m_ctlPrismBaseDrawer);
	DDX_Control(pDX, IDC_PRISM_BASE_ZOOM_SLIDER, m_ctlPrismStaticZoomSlider);
	DDX_Control(pDX, IDC_INCREMENT_SPIN, m_ctlIncrementSpinBtn);
	DDX_Control(pDX, IDC_INCREMENT_EDIT, m_ctlIncremetnEdit);
	DDX_Control(pDX, ID_PRISMVIEW3D, m_ctlPrismView3DStatic);
}

void CBlockDesignerDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CSliderCtrl* pSlider = reinterpret_cast<CSliderCtrl*>(pScrollBar);
	if (pSlider = &m_ctlPrismStaticZoomSlider)
	{
		HandlePrismStaticZoomScroll(nSBCode, nPos);
	}
	CSpinButtonCtrl* pSpin = reinterpret_cast<CSpinButtonCtrl*>(pScrollBar);
	if (pSpin == &m_ctlIncrementSpinBtn)
	{
		int i = m_ctlIncrementSpinBtn.GetPos();
		m_ctlPrismBaseDrawer.SetNewIncrement(i);
	}
}

BEGIN_MESSAGE_MAP(CBlockDesignerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_VSCROLL()

	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()

	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()

	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()

	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_INCREMENT_SPIN, &CBlockDesignerDlg::OnIncrementButtonPress)


END_MESSAGE_MAP()


// CBlockDesignerDlg message handlers

afx_msg void CBlockDesignerDlg::OnMButtonDown(UINT v, CPoint pt)
{
	m_DragDispatcher.OnMiddleMouseDown({ pt.x, pt.y }, [this](LONG& t, LONG& l, LONG& b, LONG& r) -> void
	{
		CRect rect;
		rect.top = t;
		rect.left = l;
		rect.bottom = b;
		rect.right = r;
		ScreenToClient(&rect);
		t = rect.top;
		l = rect.left;
		b = rect.bottom;
		r = rect.right;

	});
}

void CBlockDesignerDlg::OnMButtonUp(UINT, CPoint pt)
{
	m_DragDispatcher.OnMiddleMouseUp({ pt.x, pt.y }, [this](LONG& t, LONG& l, LONG& b, LONG& r) -> void
	{
		CRect rect;
		rect.top = t;
		rect.left = l;
		rect.bottom = b;
		rect.right = r;
		ScreenToClient(&rect);
		t = rect.top;
		l = rect.left;
		b = rect.bottom;
		r = rect.right;
	});
}

void CBlockDesignerDlg::OnLButtonDown(UINT, CPoint pt)
{
	m_DragDispatcher.OnLeftMouseDown({ pt.x, pt.y }, [this](LONG& t, LONG& l, LONG& b, LONG& r) -> void
	{
		CRect rect;
		rect.top = t;
		rect.left = l;
		rect.bottom = b;
		rect.right = r;
		ScreenToClient(&rect);
		t = rect.top;
		l = rect.left;
		b = rect.bottom;
		r = rect.right;
	});
}

void CBlockDesignerDlg::OnLButtonUp(UINT, CPoint pt)
{
	m_DragDispatcher.OnLeftMouseUp({ pt.x, pt.y }, [this](LONG& t, LONG& l, LONG& b, LONG& r) -> void
	{
		CRect rect;
		rect.top = t;
		rect.left = l;
		rect.bottom = b;
		rect.right = r;
		ScreenToClient(&rect);
		t = rect.top;
		l = rect.left;
		b = rect.bottom;
		r = rect.right;
	});
}

void CBlockDesignerDlg::OnRButtonDown(UINT, CPoint pt)
{
	m_DragDispatcher.OnRightMouseDown({ pt.x, pt.y }, [this](LONG& t, LONG& l, LONG& b, LONG& r) -> void
	{
		CRect rect;
		rect.top = t;
		rect.left = l;
		rect.bottom = b;
		rect.right = r;
		ScreenToClient(&rect);
		t = rect.top;
		l = rect.left;
		b = rect.bottom;
		r = rect.right;
	});
}

void CBlockDesignerDlg::OnRButtonUp(UINT, CPoint pt)
{
	m_DragDispatcher.OnRightMouseUp({ pt.x, pt.y }, [this](LONG& t, LONG& l, LONG& b, LONG& r) -> void
	{
		CRect rect;
		rect.top = t;
		rect.left = l;
		rect.bottom = b;
		rect.right = r;
		ScreenToClient(&rect);
		t = rect.top;
		l = rect.left;
		b = rect.bottom;
		r = rect.right;
	});
}




BOOL CBlockDesignerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_ctlIncrementSpinBtn.SetRange(1, 5);
	m_ctlIncrementSpinBtn.SetPos(1);

	m_DragDispatcher.RegisterDragAcceptor(&m_ctlPrismBaseDrawer, MouseButton::Middle);
	m_DragDispatcher.RegisterMouseMoveAcceptor(&m_ctlPrismBaseDrawer);
	m_DragDispatcher.RegisterMouseDownAcceptor(&m_ctlPrismBaseDrawer, MouseButton::Left);
	m_DragDispatcher.RegisterMouseDownAcceptor(&m_ctlPrismBaseDrawer, MouseButton::Right);

	m_DragDispatcher.RegisterDragAcceptor(&m_ctlPrismView3DStatic, MouseButton::Right);


	m_ctlPrismBaseDrawer.SetMeshCreatedDelegate([this](const Poly2D& poly) { m_ctlPrismView3DStatic.SetMesh(poly); });
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CBlockDesignerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CBlockDesignerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

void CBlockDesignerDlg::OnMouseMove(UINT, CPoint pt)
{
	m_DragDispatcher.OnMouseMove({ pt.x, pt.y }, [this](LONG& t, LONG& l, LONG& b, LONG& r) -> void
	{
		CRect rect;
		rect.top = t;
		rect.left = l;
		rect.bottom = b;
		rect.right = r;
		ScreenToClient(&rect);
		t = rect.top;
		l = rect.left;
		b = rect.bottom;
		r = rect.right;
	});
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CBlockDesignerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CBlockDesignerDlg::OnIncrementButtonPress()
{
	printf("fdo");
}

void CBlockDesignerDlg::HandlePrismStaticZoomScroll(UINT nSBCode, UINT nPos)
{
	int p = m_ctlPrismStaticZoomSlider.GetPos();
	float normalized = (100.f - (float)p) / 100.0f;
	float newZoom = CAM_INITIAL_ZOOM * normalized;
	if (newZoom < 1.0f)
	{
		newZoom = 1.0f;
	}
	m_ctlPrismBaseDrawer.SetNewZoom(newZoom);
	m_ctlPrismBaseDrawer.InvalidateRect(NULL, TRUE);
	/*switch (nSBCode)
	{
	case TB_LINEUP:
	case TB_LINEDOWN:
	case TB_PAGEUP:
	case TB_PAGEDOWN:
	case TB_THUMBPOSITION:
	case TB_TOP:
	case TB_BOTTOM:
	case TB_THUMBTRACK:
	case TB_ENDTRACK:
	default:
		break;
	}*/
}

