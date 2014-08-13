#include "stdafx.h"
#include "WindowPos.h"

#ifndef INI_PROGRAM
#define INI_PROGRAM _T("Program")
#endif

#pragma warning(disable : 4996)			// disable bogus deprecation warning

#ifndef _countof
#define _countof(array) (sizeof(array)/sizeof(array[0]))
#endif

void SaveWindowPos(HWND hWnd, LPCTSTR lpszEntry)
{
	TRACE(_T("in SaveWindowPos\n"));

	ASSERT(hWnd != 0);
	if (hWnd == 0)
		return;
	ASSERT(::IsWindow(hWnd));
	if (!::IsWindow(hWnd))
		return;
	ASSERT(lpszEntry != NULL);
	if (lpszEntry == NULL)
		return;

	WINDOWPLACEMENT wp = { 0 };
	wp.length = sizeof(WINDOWPLACEMENT);

	// get window position and iconized/maximized status
	::GetWindowPlacement(hWnd, &wp);
	//TRACERECT(wp.rcNormalPosition);

	wp.flags   = 0;
	wp.showCmd = SW_SHOWNORMAL;

	TCHAR tmp[200];
	_stprintf(tmp, _T("%u,%u,%d,%d,%d,%d"),
		wp.flags,
		wp.showCmd,
		wp.rcNormalPosition.left,
		wp.rcNormalPosition.top,
		wp.rcNormalPosition.right,
		wp.rcNormalPosition.bottom);

	// write position to INI file
	AfxGetApp()->WriteProfileString(INI_PROGRAM, lpszEntry, tmp);
	TRACE(_T("writing <%s> to <%s>\n"), tmp, lpszEntry);
}

BOOL RestoreWindowPos(HWND hWnd,
					  LPCTSTR lpszEntry,
					  WINDOWPLACEMENT *pWP /*= NULL*/,
					  LPCTSTR lpszDefaultPos /*= NULL*/)
{
	TRACE(_T("in RestoreWindowPos - %s\n"), lpszEntry);
	if (pWP == NULL)
	{
		ASSERT(hWnd != 0);
		if (hWnd == 0)
			return FALSE;
		ASSERT(::IsWindow(hWnd));
		if (!::IsWindow(hWnd))
			return FALSE;
	}
	ASSERT(lpszEntry != NULL);
	if (lpszEntry == NULL)
		return FALSE;

	// read window position from INI file
	CString s = _T("");

	CString strDefault = _T("");
	if (lpszDefaultPos && lpszDefaultPos[0] != 0)
		strDefault = lpszDefaultPos;

	// MainWindow=0,1,395,198,1012,642

	s = AfxGetApp()->GetProfileString(INI_PROGRAM, lpszEntry, strDefault);

	if (s.IsEmpty())
	{
		TRACE(_T("window pos empty\n"));
		return FALSE;
	}

	TRACE(_T("windowpos=<%s>\n"), s);

	TCHAR tmp[200];
	_tcsncpy(tmp, s, _countof(tmp)-2);

	// get WINDOWPLACEMENT info
	WINDOWPLACEMENT wp = { 0 };
	wp.length = sizeof(WINDOWPLACEMENT);
	wp.ptMaxPosition = CPoint(-::GetSystemMetrics(SM_CXBORDER), -::GetSystemMetrics(SM_CYBORDER));
	TCHAR *cp = 0;
	wp.rcNormalPosition = CRect(200, 100, 850, 550);
	if ((cp = _tcstok(tmp, _T(",\r\n"))) != NULL)
		wp.flags = _ttoi(cp);
	if ((cp = _tcstok(NULL,  _T(",\r\n"))) != NULL)
		wp.showCmd = _ttoi(cp);
	if ((cp = _tcstok(NULL,  _T(",\r\n"))) != NULL)
		wp.rcNormalPosition.left = _ttoi(cp);
	if ((cp = _tcstok(NULL,  _T(",\r\n"))) != NULL)
		wp.rcNormalPosition.top = _ttoi(cp);
	if ((cp = _tcstok(NULL,  _T(",\r\n"))) != NULL)
		wp.rcNormalPosition.right = _ttoi(cp);
	if ((cp = _tcstok(NULL,  _T(",\r\n"))) != NULL)
		wp.rcNormalPosition.bottom = _ttoi(cp);

	TRACERECT(wp.rcNormalPosition);

	if (pWP)
	{
		*pWP = wp;
	}
	else
	{
		// sets window's position and iconized/maximized status
		TRACE(_T("setting window placement\n"));
		::SetWindowPlacement(hWnd, &wp);
	}
	return TRUE;
}
