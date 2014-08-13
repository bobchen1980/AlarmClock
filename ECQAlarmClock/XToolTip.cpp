#include "stdafx.h"
#include "XToolTip.h"

#pragma warning(disable : 4996)	// disable bogus deprecation warning

#ifndef __noop
#if _MSC_VER < 1300
#define __noop ((void)0)
#endif
#endif

#undef TRACE
#define TRACE __noop
#undef TRACERECT
#define TRACERECT __noop

//=============================================================================
// if you want to see the TRACE output, uncomment this line:
#include "XTrace.h"


//=============================================================================
//
// CXToolTip()
//
// Purpose:     Construct CXToolTip object.
//
// Parameters:  None
//
// Returns:     None
//
// Notes:       Construction is a two-step process.  First, construct the
//              CXToolTip object.  Second, call CXToolTip::Create() to 
//              create the CXToolTip window.
//
CXToolTip::CXToolTip() 
{
	TRACE(_T("in CXToolTip::CXToolTip\n"));

	_tcscpy(m_szClassName, _T("XToolTip"));
	m_pszText = NULL;
	m_X = m_Y = -1;
	m_hInstance = 0;
	m_hParent = 0;

	NONCLIENTMETRICS ncm = { 0 };
	ncm.cbSize = sizeof(ncm);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &ncm, FALSE);
	m_pFont = CreateFontIndirect(&ncm.lfStatusFont);
}

//=============================================================================
//
// ~CXToolTip()
//
// Purpose:     Destroy CXToolTip object.
//
// Parameters:  None
//
// Returns:     None
//
CXToolTip::~CXToolTip() 
{
	TRACE(_T("in CXToolTip::~CXToolTip\n"));

	::DestroyWindow(m_hWnd);
	::UnregisterClass(m_szClassName, m_hInstance);
	if (m_pFont)
		::DeleteObject(m_pFont);
	m_pFont = NULL;
	if (m_pszText)
		free(m_pszText);
	m_pszText = NULL;
}

//=============================================================================
//
// DefWindowProcX()
//
// Purpose:     Initial window proc to dispatch messages to 
//              CXToolTip::WindowProc().  This allows us to set up
//              the 'this' pointer to CXToolTip instance.
//
// Parameters:  Standard windows message parameters.
//
// Returns:     LRESULT - The return value is the result of the message 
//                        processing and depends on the message.
//
static 
LRESULT __stdcall DefWindowProcX(HWND hWnd,		// handle to window
								 UINT message,	// message identifier
								 WPARAM wParam,	// first message parameter
								 LPARAM lParam)	// second message parameter
{
	switch (message)
	{
		case WM_CREATE:
		{
			// save 'this' pointer in windows extra memory - the lParam
			// is set when ::CreateWindowEx() is called
			CREATESTRUCT* pcs = (CREATESTRUCT *) lParam;
			if (!pcs)
			{
				TRACE(_T("ERROR - CREATESTRUCT lParam is zero\n"));
				ASSERT(pcs);
				return -1;		// abort creation
			}
			::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pcs->lpCreateParams);
			return 0;
		}
		break;

		default:
		{
			// dispatch via saved 'this' pointer
			LONG_PTR lData = ::GetWindowLongPtr(hWnd, GWLP_USERDATA);
			if (lData)
			{
				CXToolTip *pWnd = (CXToolTip *) lData;
				return pWnd->WindowProc(message, wParam, lParam);
			}
			else
			{
				// probably some WM_NCxxxx message
				TRACE(_T("GWLP_USERDATA = 0 for message = 0x%04X\n"), message);
			}
		}
		break;
	}

	return ::DefWindowProc(hWnd, message, wParam, lParam);
}

//=============================================================================
//
// Create()
//
// Purpose:     This virtual function creates the CXToolTip window.
//
// Parameters:  hParent   - the parent window HWND
//              hInstance - handle to the instance that contains 
//                          the window procedure 
//
// Returns:     BOOL - TRUE = window created successfully
//
BOOL CXToolTip::Create(HWND hParent, HINSTANCE hInstance)
{
	TRACE(_T("in CXToolTip::Create\n"));
	ASSERT(hParent);
	m_hParent = hParent;
	m_hInstance = hInstance;

	WNDCLASS wc =
	{
		CS_SAVEBITS,				// class style - saves, as a bitmap, the
									// portion of the screen image obscured 
									// by a window of this class
		DefWindowProcX,				// window proc
		0,							// class extra bytes
		0,							// window extra bytes
		hInstance,					// instance handle
		0,							// icon
		::LoadCursor(0, IDC_ARROW),	// cursor
		0,							// background brush
		0,							// menu name
		m_szClassName				// class name
	};

	if (!::RegisterClass(&wc))
	{
		DWORD dwLastError = GetLastError();
		if (dwLastError != ERROR_CLASS_ALREADY_EXISTS)
		{
			TRACE(_T("ERROR - RegisterClass failed, GetLastError() returned %u\n"), dwLastError);
			ASSERT(FALSE);
			return FALSE;
		}
	}

	// we pass 'this' pointer as lpParam, so DefWindowProcX will see it 
	// in WM_CREATE message
	m_hWnd = ::CreateWindowEx(0, m_szClassName, _T(""), WS_POPUP | WS_VISIBLE, 
		30000, 30000, 10, 10,
		hParent, (HMENU)0, hInstance, this);

	if (m_hWnd == 0)
	{
		TRACE(_T("ERROR - CreateWindowEx failed\n"));
		ASSERT(m_hWnd);
		return FALSE;
	}

	return TRUE;
}

//=============================================================================
//
// GetCursorOffset()
//
// Purpose:     GetCursorOffset() returns the location of the non-transparent 
//              pixel in the cursor closest to the bottom right corner of 
//              the cursor
//
// Parameters:  hWnd - HWND of window containing cursor
//
// Returns:     SIZE - coordinates of last pixel location
//
SIZE CXToolTip::GetCursorOffset(HWND hWnd)
{
	SIZE sz = { 32, 32 };

	// get info for current cursor
	ICONINFO info;
	::GetIconInfo((HICON)GetCursor(), &info);

	if (info.hbmMask)
	{
		BITMAP bmInfo;
		::GetObject(info.hbmMask, sizeof(BITMAP), &bmInfo);
		sz.cx = bmInfo.bmWidth;
		sz.cy = bmInfo.bmHeight;

		CDC dcWindow;
		HDC hDC = ::GetDC(hWnd);
		dcWindow.Attach(hDC);
		CDC dc;
		dc.CreateCompatibleDC(&dcWindow);
		HBITMAP oldbmp = (HBITMAP) dc.SelectObject(info.hbmMask);
		int row = 0, col = 0;
		BOOL bFinished = FALSE;

		// start at bottom and search up
		for (row = bmInfo.bmHeight - 1; row >= 0; row--)
		{
			COLORREF first = dc.GetPixel(bmInfo.bmWidth - 1, row);

			// start at right and search to left
			for (col = bmInfo.bmWidth - 1; col >= 0; col--)
			{
				COLORREF cr = dc.GetPixel(col, row);
				if (cr != first)
				{
					// color has changed
					TRACE(_T("row=%d  col=%d  cr=%X  first=%X\n"), row, col, cr, first);
					bFinished = TRUE;
					break;
				}
			}
			if (bFinished)
				break;
		}
		if (bFinished)
		{
			sz.cx = col;
			sz.cy = row;
		}

		dc.SelectObject(oldbmp);

		// must delete bitmaps returned by GetIconInfo
		::DeleteObject(info.hbmMask);
		if (info.hbmColor)
			::DeleteObject(info.hbmColor);

		::ReleaseDC(hWnd, hDC);
	}

	TRACE(_T("sz.cx=%d  sz.cy=%d\n"), sz.cx, sz.cy);
	return sz;
}

//=============================================================================
//
// Hide()
//
// Purpose:     Hides the CXToolTip window.
//
// Parameters:  None
//
// Returns:     None
//
void CXToolTip::Hide() 
{
	if (::IsWindow(m_hWnd))
	{
		::KillTimer(m_hWnd, 1);
		::ShowWindow(m_hWnd, SW_HIDE);
	}
	if (m_pszText)
		free(m_pszText);
	m_pszText = NULL;
}

//=============================================================================
//
// Show()
//
// Purpose:     Shows the tooltip window at x,y location
//
// Parameters:  lpszText - Pointer to text to display
//              x, y     - Top left corner coordinates of tooltip
//              bConfine - TRUE = confine tooltip to parent window
//
// Returns:     None
//
void CXToolTip::Show(LPCTSTR lpszText, int x, int y, BOOL bConfine /*= FALSE*/) 
{
	TRACE(_T("CXToolTip::Show: x=%d  y=%d =========================\n"), x, y);

	if (x == m_X && y == m_Y)
		return;

	m_X = x;
	m_Y = y;

	if (!lpszText || 
		(lpszText[0] == _T('\0')) ||
		!IsWindow(m_hWnd))
	{
		Hide();
		return;
	}

	static BOOL bFirstTime = TRUE;
	if (bFirstTime)
	{
		// m_sizeOffset is the location of the non-transparent pixel closest 
		// to the bottom right corner of the cursor
		m_sizeOffset = GetCursorOffset(m_hWnd);
	}
	bFirstTime = FALSE;

	::GetWindowRect(m_hParent, &m_rectParent);

	BOOL bTextChanged = TRUE;

	if (m_pszText)
	{
		if (_tcscmp(lpszText, m_pszText) == 0)
		{
			bTextChanged = FALSE;
		}
		else
		{
			free(m_pszText);
			m_pszText = NULL;
		}
	}
	if (!m_pszText)
		m_pszText = _tcsdup(lpszText);

	if (bTextChanged)
	{
		// text has changed - we need to calculate size of text
		CDC dc;
		HDC hDC = ::GetDC(m_hWnd);
		dc.Attach(hDC);
		int nSavedDC = dc.SaveDC();
		dc.SelectObject(m_pFont);
		CRect rect(0, 0, 300, 0);
		dc.DrawText(lpszText, -1, rect, 
			DT_CALCRECT | DT_WORDBREAK | DT_NOPREFIX);
		TRACERECT(rect);
		dc.RestoreDC(nSavedDC);

		// add a bit of padding on the sides
		int width  = rect.Width()  + 7;
		int height = rect.Height() + 3;

		if (bConfine)
		{
			// keep the tooltip from displaying outside the parent window
			if (x + width > m_rectParent.right)
				x = m_rectParent.right - width;
			if (y + height > m_rectParent.bottom)
				y = m_rectParent.bottom - height;
		}

		m_rect = CRect(x, y, x+width, y+height);
		TRACERECT(m_rect);

		if (::IsWindow(m_hWnd))
		{
			::SetWindowPos(m_hWnd, HWND_TOP, 
				x+m_sizeOffset.cx, y+m_sizeOffset.cy, 
				width, height, 
				SWP_NOACTIVATE);
			::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
		}

		::InvalidateRect(m_hWnd, NULL, FALSE);

		::ReleaseDC(m_hWnd, hDC);
	}
	else
	{
		// text has not changed, just move the window
		::MoveWindow(m_hWnd, x+m_sizeOffset.cx, y+m_sizeOffset.cy, 
			m_rect.Width(), m_rect.Height(), TRUE);
	}

	::SetTimer(m_hWnd, 1, 80, NULL);
}

//=============================================================================
//
// WindowProc()
//
// Purpose:     This function is the window proc for CXColorHexagonCtrl object.  
//              Messages are forwarded to this function from DefWindowProcX().
//
// Parameters:  message - message identifier
//              wParam  - first message parameter
//              lParam  - second message parameter
//
// Returns:     LRESULT - The return value is the result of the message 
//                        processing and depends on the message.
//
LRESULT CXToolTip::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	switch (message)
	{
		case WM_CREATE:
			// will never see this message - it's handled by DefWindowProcX
			break;

		case WM_ERASEBKGND:
			// nothing to erase, since we draw the entire client area
			TRACE(_T("WM_ERASEBKGND\n"));
			return TRUE;

		case WM_PAINT:
		{
			TRACE(_T("WM_PAINT\n"));
			PAINTSTRUCT ps; 
			HDC hdc = (wParam != NULL) ? 
						(HDC) wParam : ::BeginPaint(m_hWnd, &ps);
			if (hdc == 0)
				return 0;
			CDC dc;
			dc.Attach(hdc);
			int nSavedDC = dc.SaveDC();

			// Draw background
			COLORREF clrBackground = ::GetSysColor(COLOR_INFOBK);
			CRect rectClient;
			::GetClientRect(m_hWnd, &rectClient);
			TRACERECT(rectClient);
			dc.Rectangle(rectClient);
			rectClient.DeflateRect(1,1,1,1);
			dc.FillSolidRect(rectClient, clrBackground);

			// Draw text
			dc.SelectObject(m_pFont);
			dc.SetTextColor(::GetSysColor(COLOR_INFOTEXT));
			dc.SetBkMode(TRANSPARENT);
			rectClient.left += 3;
			dc.DrawText(m_pszText, -1, rectClient, DT_WORDBREAK | DT_NOPREFIX);
			dc.RestoreDC(nSavedDC);
			if (wParam == NULL)
				::EndPaint(m_hWnd, &ps);
			return 0;
		}

		case WM_TIMER:
		{
			POINT point;
			::GetCursorPos(&point);
			if (!m_rectParent.PtInRect(point))
			{
				// outside parent window, hide tooltip
				Hide();
			}
			break;
		}

		default:
			break;
	}

	return ::DefWindowProc(m_hWnd, message, wParam, lParam);
}
