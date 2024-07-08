
// BlockDesignerDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "BlockDesigner.h"
#include "BlockDesignerDlg.h"
#include "ExportDlg.h"
#include "afxdialogex.h"
#include "resource.h"
#include <winuser.h>
#include <cwchar>
#include <cerrno>
#include "Exporters.h"

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
public:
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

void CBlockDesignerDlg::Serialize(CArchive& ar)
{
	const WORD v = 1;
	if (ar.IsStoring())
	{
		ar << v;
		ar << m_TexelsPerUnit;
		SerializeExtrudeSettings(ar);
		m_ctlPrismBaseDrawer.Serialize(ar);
		m_ctlPrismView3DStatic.Serialize(ar);
	}
	else
	{
		WORD ver;
		ar >> ver;
		switch (ver)
		{
		case 1:
			ar >> m_TexelsPerUnit;
			SerializeExtrudeSettings(ar);
			m_ctlPrismBaseDrawer.Serialize(ar);
			m_ctlPrismView3DStatic.Serialize(ar);
			break;
		default:
			break;
		}
	}
}

void CBlockDesignerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_PRISM_BASE_DRAWER_STATIC, m_ctlPrismBaseDrawer);
	DDX_Control(pDX, IDC_PRISM_BASE_ZOOM_SLIDER, m_ctlPrismStaticZoomSlider);
	DDX_Control(pDX, IDC_INCREMENT_SPIN, m_ctlIncrementSpinBtn);
	DDX_Control(pDX, IDC_INCREMENT_EDIT, m_ctlIncremetnEdit);
	DDX_Control(pDX, ID_PRISMVIEW3D, m_ctlPrismView3DStatic);

	DDX_Text(pDX, IDC_TOP_CAP_SCALE_X, m_ctlTopCapScaleX);
	DDX_Text(pDX, IDC_TOP_CAP_SCALE_Y, m_ctlTopCapScaleY);
	DDX_Text(pDX, IDC_TOP_CAP_OFFSET_X, m_ctlTopCapOffsetX);
	DDX_Text(pDX, IDC_TOP_CAP_OFFSET_Y, m_ctlTopCapOffsetY);

	DDX_Text(pDX, IDC_BOTTOM_CAP_SCALE_X, m_ctlBottomCapScaleX);
	DDX_Text(pDX, IDC_BOTTOM_CAP_SCALE_Y, m_ctlBottomCapScaleY);
	DDX_Text(pDX, IDC_BOTTOM_CAP_OFFSET_X, m_ctlBottomCapOffsetX);
	DDX_Text(pDX, IDC_BOTTOM_CAP_OFFSET_Y, m_ctlBottomCapOffsetY);

	DDX_Text(pDX, IDC_EXTRUDE_AMOUNT, m_ctlExtrudeAmount);

	DDX_Text(pDX, IDC_TEXTURE_PIXELS_PER_UNIT, m_ctlTexelsPerUnit);

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

	ON_EN_KILLFOCUS(IDC_TOP_CAP_SCALE_X, OnChangeTopCapScaleX)
	ON_EN_KILLFOCUS(IDC_TOP_CAP_SCALE_Y, OnChangeTopCapScaleY)

	ON_EN_KILLFOCUS(IDC_TOP_CAP_OFFSET_X, OnChangeTopCapOffsetX)
	ON_EN_KILLFOCUS(IDC_TOP_CAP_OFFSET_Y, OnChangeTopCapOffsetY)

	ON_EN_KILLFOCUS(IDC_BOTTOM_CAP_SCALE_X, OnChangeBottomCapScaleX)
	ON_EN_KILLFOCUS(IDC_BOTTOM_CAP_SCALE_Y, OnChangeBottomCapScaleY)

	ON_EN_KILLFOCUS(IDC_BOTTOM_CAP_OFFSET_X, OnChangeBottomCapOffsetX)
	ON_EN_KILLFOCUS(IDC_BOTTOM_CAP_OFFSET_Y, OnChangeBottomCapOffsetY)

	ON_EN_KILLFOCUS(IDC_EXTRUDE_AMOUNT, OnChangeExtrudeAmount)

	ON_EN_KILLFOCUS(IDC_TEXTURE_PIXELS_PER_UNIT, OnChangeTexelsPerUnit)

	ON_BN_CLICKED(IDC_CLEAR_BTN, OnClearButtonClick)

	ON_COMMAND(ID_FILE_OPEN32771, &CBlockDesignerDlg::OnFileOpen32771)
	ON_COMMAND(ID_FILE_SAVE32772, &CBlockDesignerDlg::OnFileSave32772)
	ON_COMMAND(ID_FILE_SAVEAS, &CBlockDesignerDlg::OnFileSaveas)
	ON_COMMAND(ID_FILE_EXPORT, &CBlockDesignerDlg::OnFileExport)
END_MESSAGE_MAP()

IMPLEMENT_SERIAL(CBlockDesignerDlg, CDialogEx, 1)
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

	m_ctlBottomCapOffsetX = _T("0.0");
	m_ctlBottomCapOffsetY = _T("0.0");
	m_ctlBottomCapScaleX = _T("1.0");
	m_ctlBottomCapScaleY = _T("1.0");

	m_ctlTopCapOffsetX = _T("0.0");
	m_ctlTopCapOffsetY = _T("0.0");
	m_ctlTopCapScaleX = _T("1.0");
	m_ctlTopCapScaleY = _T("1.0");

	m_ctlExtrudeAmount = _T("1.0");
	m_ctlTexelsPerUnit = _T("100.0");

	UpdateData(FALSE);

	m_ctlPrismBaseDrawer.SetMeshCreatedDelegate([this](const Poly2D& poly)
	{ 
		m_ctlPrismView3DStatic.SetMesh(poly, m_ExtrudeParameters); 

	});
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

bool CBlockDesignerDlg::ValidateAndSetFloat(CString& input, float& output, const CString& fallbackVal)
{
	UpdateData(TRUE);

	wchar_t* c = nullptr;
	float val = std::wcstof(input, &c);
	LPCWSTR cstr = input;
	if (c == &cstr[input.GetLength()])
	{
		output = val;
		return true;
	}
	else
	{
		input = fallbackVal;
		UpdateData(FALSE);
	}
	return false;
}



void CBlockDesignerDlg::SerializeExtrudeSettings(CArchive& ar)
{
	const WORD version = 1;
	if (ar.IsStoring())
	{
		ar << version;
		ar << m_ExtrudeParameters.BottomCapOffsetX;
		ar << m_ExtrudeParameters.BottomCapOffsetY;
		ar << m_ExtrudeParameters.BottomCapScaleX;
		ar << m_ExtrudeParameters.BottomCapScaleY;

		ar << m_ExtrudeParameters.ExtrudeAmount;

		ar << m_ExtrudeParameters.TopCapOffsetX;
		ar << m_ExtrudeParameters.TopCapOffsetY;
		ar << m_ExtrudeParameters.TopCapScaleX;
		ar << m_ExtrudeParameters.TopCapScaleY;

		ar << m_TexelsPerUnit;
	}
	else
	{
		WORD v = 1;
		ar >> v;
		switch (v)
		{
		case 1:
			ar >> m_ExtrudeParameters.BottomCapOffsetX;
			ar >> m_ExtrudeParameters.BottomCapOffsetY;
			ar >> m_ExtrudeParameters.BottomCapScaleX;
			ar >> m_ExtrudeParameters.BottomCapScaleY;
			ar >> m_ExtrudeParameters.ExtrudeAmount;
			ar >> m_ExtrudeParameters.TopCapOffsetX;
			ar >> m_ExtrudeParameters.TopCapOffsetY;
			ar >> m_ExtrudeParameters.TopCapScaleX;
			ar >> m_ExtrudeParameters.TopCapScaleY;
			ar >> m_TexelsPerUnit;

			m_ctlBottomCapOffsetX.Format(_T("%.2f"), m_ExtrudeParameters.BottomCapOffsetX);
			m_ctlBottomCapOffsetY.Format(_T("%.2f"), m_ExtrudeParameters.BottomCapOffsetY);
			m_ctlBottomCapScaleX.Format(_T("%.2f"), m_ExtrudeParameters.BottomCapScaleX);
			m_ctlBottomCapScaleY.Format(_T("%.2f"), m_ExtrudeParameters.BottomCapScaleY);
			m_ctlExtrudeAmount.Format(_T("%.2f"), m_ExtrudeParameters.ExtrudeAmount);
			m_ctlTopCapOffsetX.Format(_T("%.2f"), m_ExtrudeParameters.TopCapOffsetX);
			m_ctlTopCapOffsetY.Format(_T("%.2f"), m_ExtrudeParameters.TopCapOffsetY);
			m_ctlTopCapScaleX.Format(_T("%.2f"), m_ExtrudeParameters.TopCapScaleX);
			m_ctlTopCapScaleY.Format(_T("%.2f"), m_ExtrudeParameters.TopCapScaleY);
			m_ctlTexelsPerUnit.Format(_T("%.2f"), m_TexelsPerUnit);
			UpdateData(FALSE);

			break;
		default:
			break;
		}
	}
}

void CBlockDesignerDlg::OnChangeTopCapScaleX()
{
	if (ValidateAndSetFloat(m_ctlTopCapScaleX, m_ExtrudeParameters.TopCapScaleX, _T("1.0")) && m_ctlPrismView3DStatic.IsMeshSet())
	{
		m_ctlPrismView3DStatic.SetMesh(m_ctlPrismBaseDrawer.GetPoly(), m_ExtrudeParameters);
	}
}

void CBlockDesignerDlg::OnChangeTopCapScaleY()
{
	if (ValidateAndSetFloat(m_ctlTopCapScaleY, m_ExtrudeParameters.TopCapScaleY, _T("1.0")) && m_ctlPrismView3DStatic.IsMeshSet())
	{
		m_ctlPrismView3DStatic.SetMesh(m_ctlPrismBaseDrawer.GetPoly(), m_ExtrudeParameters);
	}
}

void CBlockDesignerDlg::OnChangeTopCapOffsetX()
{
	if (ValidateAndSetFloat(m_ctlTopCapOffsetX, m_ExtrudeParameters.TopCapOffsetX, _T("0.0")) && m_ctlPrismView3DStatic.IsMeshSet())
	{
		m_ctlPrismView3DStatic.SetMesh(m_ctlPrismBaseDrawer.GetPoly(), m_ExtrudeParameters);
	}
}

void CBlockDesignerDlg::OnChangeTopCapOffsetY()
{
	if (ValidateAndSetFloat(m_ctlTopCapOffsetY, m_ExtrudeParameters.TopCapOffsetY, _T("0.0")) && m_ctlPrismView3DStatic.IsMeshSet())
	{
		m_ctlPrismView3DStatic.SetMesh(m_ctlPrismBaseDrawer.GetPoly(), m_ExtrudeParameters);
	}
}

void CBlockDesignerDlg::OnChangeBottomCapScaleX()
{
	if (ValidateAndSetFloat(m_ctlBottomCapScaleX, m_ExtrudeParameters.BottomCapScaleX, _T("1.0")) && m_ctlPrismView3DStatic.IsMeshSet())
	{
		m_ctlPrismView3DStatic.SetMesh(m_ctlPrismBaseDrawer.GetPoly(), m_ExtrudeParameters);
	}
}

void CBlockDesignerDlg::OnChangeBottomCapScaleY()
{
	if (ValidateAndSetFloat(m_ctlBottomCapScaleY, m_ExtrudeParameters.BottomCapScaleY, _T("1.0")) && m_ctlPrismView3DStatic.IsMeshSet())
	{
		m_ctlPrismView3DStatic.SetMesh(m_ctlPrismBaseDrawer.GetPoly(), m_ExtrudeParameters);
	}
}

void CBlockDesignerDlg::OnChangeBottomCapOffsetX()
{
	if (ValidateAndSetFloat(m_ctlBottomCapOffsetX, m_ExtrudeParameters.BottomCapOffsetX, _T("0.0")) && m_ctlPrismView3DStatic.IsMeshSet())
	{
		m_ctlPrismView3DStatic.SetMesh(m_ctlPrismBaseDrawer.GetPoly(), m_ExtrudeParameters);
	}
}

void CBlockDesignerDlg::OnChangeBottomCapOffsetY()
{
	if (ValidateAndSetFloat(m_ctlBottomCapOffsetY, m_ExtrudeParameters.BottomCapOffsetY, _T("0.0")) && m_ctlPrismView3DStatic.IsMeshSet())
	{
		m_ctlPrismView3DStatic.SetMesh(m_ctlPrismBaseDrawer.GetPoly(), m_ExtrudeParameters);
	}
}

void CBlockDesignerDlg::OnChangeExtrudeAmount()
{
	if (ValidateAndSetFloat(m_ctlExtrudeAmount, m_ExtrudeParameters.ExtrudeAmount, _T("1.0")) && m_ctlPrismView3DStatic.IsMeshSet())
	{
		m_ctlPrismView3DStatic.SetMesh(m_ctlPrismBaseDrawer.GetPoly(), m_ExtrudeParameters);
	}
}

void CBlockDesignerDlg::OnChangeTexelsPerUnit()
{
	if (ValidateAndSetFloat(m_ctlTexelsPerUnit, m_TexelsPerUnit, _T("100.0")) && m_ctlPrismView3DStatic.IsMeshSet())
	{
		m_ctlPrismView3DStatic.SetTexelsPerUnit(m_TexelsPerUnit);
	}
}

void CBlockDesignerDlg::OnClearButtonClick()
{
	m_ctlPrismBaseDrawer.Clear();
	m_ctlPrismView3DStatic.Clear();
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
}



void CBlockDesignerDlg::SaveInternal()
{
	CFile file(m_OpenFilePath, CFile::modeCreate | CFile::modeWrite);
	CArchive archive(&file, CArchive::store);
	Serialize(archive);
}

void CBlockDesignerDlg::OnFileOpen32771()
{
	TCHAR szFilters[] = _T("MyType Files (*.bdesigner)|*.bdesigner");

	CFileDialog fileDlg(TRUE, _T("Block Designer"), _T("*.bdesigner"),
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

	if (fileDlg.DoModal() == IDOK)
	{
		CString pathName = fileDlg.GetPathName();
		CFile file(pathName, CFile::modeRead);
		CArchive archive(&file, CArchive::load);
		Serialize(archive);
		m_OpenFilePath = pathName;
		m_bOpenFilePathSet = true;
	}
}


void CBlockDesignerDlg::OnFileSave32772()
{
	if (m_bOpenFilePathSet)
	{
		SaveInternal();
	}
	else
	{
		OnFileSaveas();
	}
}



void CBlockDesignerDlg::OnFileSaveas()
{
	TCHAR szFilters[] = _T("MyType Files (*.bdesigner)|*.bdesigner");

	CFileDialog fileDlg(FALSE, _T("Block Designer"), _T("*.bdesigner"),
		OFN_HIDEREADONLY, szFilters);

	if (fileDlg.DoModal() == IDOK)
	{
		CString pathName = fileDlg.GetPathName();
		m_bOpenFilePathSet = true;
		m_OpenFilePath = pathName;
		SaveInternal();
	}
}


void CBlockDesignerDlg::OnFileExport()
{
	ExportDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		IExporter* exporter = GetExporter(dlg.GetSelectedExporter());
		exporter->SetFolderPath(dlg.GetFolderPath());
		if (exporter->EditSettings())
		{
			exporter->Export(m_ctlPrismView3DStatic.GetMesh());
		}
	}
}
