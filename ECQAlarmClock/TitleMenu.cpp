// TitleMenu.cpp: implementation of the CTitleMenu class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TitleMenu.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CTitleMenu::CTitleMenu()
{
	CreatePopupMenuTitleFont();

	clLeft  = ::GetSysColor(COLOR_ACTIVECAPTION);
	clRight = ::GetSysColor(27);  //COLOR_GRADIENTACTIVECAPTION
	clText  = ::GetSysColor(COLOR_CAPTIONTEXT);
	
	m_bCanDoGradientFill = FALSE;
	hinst_msimg32 = LoadLibrary(_T("msimg32.dll"));
	
	if (hinst_msimg32)
	{
		m_bCanDoGradientFill = TRUE;		
		dllfunc_GradientFill = 
			(LPFNDLLFUNC1) GetProcAddress(hinst_msimg32, "GradientFill");
	}
	bDrawEdge = false;
	flag_edge = BDR_SUNKENINNER;
}

CTitleMenu::~CTitleMenu()
{
	if (m_Font.GetSafeHandle())
		m_Font.DeleteObject();
	FreeLibrary(hinst_msimg32);
}


/////////////////////////////////////////////////////////////////////////////
// CTitleMenu message handlers

void CTitleMenu::CreatePopupMenuTitleFont()
{
	// start by getting the stock menu font
	NONCLIENTMETRICS ncm;
	memset(&ncm, 0, sizeof(ncm));
	ncm.cbSize = sizeof(ncm);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, 0);
	LOGFONT lf;
	memcpy(&lf, &ncm.lfMenuFont, sizeof(LOGFONT));
	lf.lfWeight = FW_BOLD;
	if (lf.lfHeight < 0)
		lf.lfHeight -= 4;
	else
		lf.lfHeight += 4;
	m_Font.CreateFontIndirect(&lf);

#if 0  // -----------------------------------------------------------
	//HFONT hfont = (HFONT)GetStockObject(ANSI_VAR_FONT);
	if (hfont) 
	{ 
	    LOGFONT lf; //get the complete LOGFONT describing this font
	    if (GetObject(hfont, sizeof(LOGFONT), &lf)) 
		{
			lf.lfWeight = FW_BOLD;	// set the weight to bold
			// recreate this font with just the weight changed
			return ::CreateFontIndirect(&lf);
		}
	}
	return NULL;
#endif // -----------------------------------------------------------
}

//
// This function adds a title entry to a popup menu
//
void CTitleMenu::AddMenuTitle(LPCTSTR lpszTitle)
{
	// insert an empty owner-draw item at top to serve as the title
	// note: item is not selectable (disabled) but not grayed
	m_strTitle = lpszTitle;
	InsertMenu(0, MF_BYPOSITION | MF_OWNERDRAW | MF_STRING | MF_DISABLED, 0);
}

void CTitleMenu::MeasureItem(LPMEASUREITEMSTRUCT mi)
{
	// get the screen dc to use for retrieving size information
	HDC hScreenDC = ::GetDC(NULL);

	// select the title font
	ASSERT(m_Font.GetSafeHandle());
	HFONT hfontOld = (HFONT)SelectObject(hScreenDC, (HFONT)m_Font);

	// compute the size of the title
	CSize size;
	VERIFY(::GetTextExtentPoint32(hScreenDC, m_strTitle, m_strTitle.GetLength(), 
		&size));

	// deselect the title font
	::SelectObject(hScreenDC, hfontOld);

	// add in the left margin for the menu item
	size.cx += GetSystemMetrics(SM_CXMENUCHECK) + 8;

	// Return the width and height
	// + include space for border
	const int nBorderSize = 2;
	mi->itemWidth  = size.cx + nBorderSize;
	mi->itemHeight = size.cy + nBorderSize;

	::ReleaseDC(NULL, hScreenDC);
}

void CTitleMenu::DrawItem(LPDRAWITEMSTRUCT di)
{
	int nSavedDC = ::SaveDC(di->hDC);

	::SetBkColor(di->hDC, clLeft);
	
	if (m_bCanDoGradientFill && (clLeft != clRight))
	{
 		TRIVERTEX rcVertex[2];
		di->rcItem.right--; // exclude this point, like FillRect does 
		di->rcItem.bottom--;
		rcVertex[0].x     = di->rcItem.left;
		rcVertex[0].y     = di->rcItem.top;
		rcVertex[0].Red   = (COLOR16)(GetRValue(clLeft)<<8);	// color values from 0x0000 to 0xff00 !!!!
		rcVertex[0].Green = (COLOR16)(GetGValue(clLeft)<<8);
		rcVertex[0].Blue  = (COLOR16)(GetBValue(clLeft)<<8);
		rcVertex[0].Alpha = 0x0000;
		rcVertex[1].x     = di->rcItem.right; 
		rcVertex[1].y     = di->rcItem.bottom;
		rcVertex[1].Red   = (COLOR16)(GetRValue(clRight)<<8);
		rcVertex[1].Green = (COLOR16)(GetGValue(clRight)<<8);
		rcVertex[1].Blue  = (COLOR16)(GetBValue(clRight)<<8);
		rcVertex[1].Alpha = 0;
		GRADIENT_RECT rect;
		rect.UpperLeft = 0;
		rect.LowerRight = 1;
		
		// fill the area 
		GradientFill(di->hDC, rcVertex, 2, &rect, 1, GRADIENT_FILL_RECT_H);
	}
	else
	{
		::ExtTextOut(di->hDC, 0, 0, ETO_OPAQUE, &di->rcItem, NULL, 0, NULL);
	}

	if (bDrawEdge)
		::DrawEdge(di->hDC, &di->rcItem, flag_edge, BF_RECT);
 
	::SetBkMode(di->hDC, TRANSPARENT);
	::SetTextColor(di->hDC, clText);

	// select font into the dc
	::SelectObject(di->hDC, (HFONT)m_Font);

	// add the menu margin offset
	di->rcItem.left += GetSystemMetrics(SM_CXMENUCHECK)+8;

	// draw the text left aligned and vertically centered
	::DrawText(di->hDC, m_strTitle, -1, &di->rcItem, 
		DT_SINGLELINE | DT_VCENTER | DT_LEFT);

	//Restore font and colors
	if (nSavedDC)
		::RestoreDC(di->hDC, nSavedDC);
}

BOOL CTitleMenu::GradientFill(HDC hdc, 
							  PTRIVERTEX pVertex, 
							  DWORD dwNumVertex, 
							  PVOID pMesh, 
							  DWORD dwNumMesh, 
							  DWORD dwMode)
{
	ASSERT(m_bCanDoGradientFill); 
	return dllfunc_GradientFill(hdc,pVertex,dwNumVertex,pMesh,dwNumMesh,dwMode); 
}

COLORREF CTitleMenu::LightenColor(COLORREF col, double factor)
{
	COLORREF rgb = 0;
	if (factor > 0.0 && factor <= 1.0)
	{
		BYTE Red,Green,blue,lightred,lightgreen,lightblue;
		Red        = GetRValue(col);
		Green      = GetGValue(col);
		blue       = GetBValue(col);
		lightred   = (BYTE)((factor*(255-Red)) + Red);
		lightgreen = (BYTE)((factor*(255-Green)) + Green);
		lightblue  = (BYTE)((factor*(255-blue)) + blue);
		rgb = RGB(lightred, lightgreen, lightblue);
	}
	return rgb;
}
