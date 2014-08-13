#include "stdafx.h"
#include "About.h"


//=============================================================================
BEGIN_MESSAGE_MAP(CAboutDlg, ABOUT_BASE_CLASS)
//=============================================================================
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//=============================================================================
CAboutDlg::CAboutDlg() : ABOUT_BASE_CLASS(CAboutDlg::IDD)
//=============================================================================
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

//=============================================================================
CAboutDlg::~CAboutDlg()
//=============================================================================
{
}

//=============================================================================
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
//=============================================================================
{
	ABOUT_BASE_CLASS::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

//=============================================================================
BOOL CAboutDlg::OnInitDialog()
//=============================================================================
{
	ABOUT_BASE_CLASS::OnInitDialog();
		
	return TRUE;
}


//=============================================================================
BOOL CAboutDlg::OnEraseBkgnd(CDC* pDC) 
//=============================================================================
{
	CRect rectClient;
	GetClientRect(&rectClient);
/*
#pragma warning(disable : 4310)

	TRIVERTEX vert[2];
	vert[0].x      = 0;
	vert[0].y      = 0;
	vert[0].Red    = (COLOR16) (GetRValue(BACKGROUND_COLOR1) << 8);
	vert[0].Green  = (COLOR16) (GetGValue(BACKGROUND_COLOR1) << 8);
	vert[0].Blue   = (COLOR16) (GetBValue(BACKGROUND_COLOR1) << 8);
	vert[0].Alpha  = 0x0000;

	vert[1].x      = rectClient.right;
	vert[1].y      = rectClient.bottom; 
	vert[1].Red    = (COLOR16) (GetRValue(BACKGROUND_COLOR2) << 8);
	vert[1].Green  = (COLOR16) (GetGValue(BACKGROUND_COLOR2) << 8);
	vert[1].Blue   = (COLOR16) (GetBValue(BACKGROUND_COLOR2) << 8);
	vert[1].Alpha  = 0x0000;

	GRADIENT_RECT rect;
	rect.UpperLeft  = 0;
	rect.LowerRight = 1;

	::GradientFill(pDC->m_hDC, vert, 2, &rect, 1, GRADIENT_FILL_RECT_V);

	return TRUE;*/
	return ABOUT_BASE_CLASS::OnEraseBkgnd(pDC);
}
