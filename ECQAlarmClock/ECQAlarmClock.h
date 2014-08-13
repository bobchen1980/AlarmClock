// ECQAlarmClock.h : main header file for the ECQAlarmClock application
//

#ifndef ECQAlarmClock_H
#define ECQAlarmClock_H

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#define BAD_XY -9999999

/////////////////////////////////////////////////////////////////////////////
// CECQAlarmClockApp:
// See ECQAlarmClock.cpp for the implementation of this class
//

class CECQAlarmClockApp : public CWinApp
{
public:
	CECQAlarmClockApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CECQAlarmClockApp)
	public:
	virtual BOOL InitInstance();
	virtual BOOL InitApplication();
	//}}AFX_VIRTUAL

// Implementation
public:

	// command line args
	CString m_strSkin;
	CString m_strToolTip;
	BYTE m_nTransparency;
	BOOL m_bAlwaysOnTop;
	int m_nHourAdjust;
	int m_nX;
	int m_nY;

protected:
	void ProcessCommandLine(int argc, TCHAR *argv[]);

	//{{AFX_MSG(CECQAlarmClockApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CECQAlarmClockApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //ECQAlarmClock_H
