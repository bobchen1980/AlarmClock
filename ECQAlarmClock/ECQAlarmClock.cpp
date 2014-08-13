// ECQAlarmClock.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ECQAlarmClock.h"
#include "ECQAlarmClockDlg.h"
#include "XWinVer.h"
#include "XGetOpt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CECQAlarmClockApp

BEGIN_MESSAGE_MAP(CECQAlarmClockApp, CWinApp)
	//{{AFX_MSG_MAP(CECQAlarmClockApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CECQAlarmClockApp construction

CECQAlarmClockApp::CECQAlarmClockApp()
{
	m_strSkin = _T("");
	m_strToolTip = _T("");
	m_nHourAdjust = 0;
	m_nX = BAD_XY;
	m_nY = BAD_XY;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CECQAlarmClockApp object

CECQAlarmClockApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CECQAlarmClockApp initialization

BOOL CECQAlarmClockApp::InitInstance()
{
#if _MFC_VER < 0x700
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

	CXWinVersion ver;
	if (!ver.IsWin2KorLater())
	{
		::MessageBox(NULL, _T("ECQAlarmClock requires Win2000 or later."), 
			_T("ECQAlarmClock"), MB_OK|MB_ICONEXCLAMATION);
		return FALSE;
	}

	HANDLE   hMutex=::CreateMutex(NULL,TRUE,_T("ECQAlarmClock"));//FirstName可以随便取一个唯一的名字   
	if   (hMutex!=NULL)   
	{   
		if   (GetLastError()==ERROR_ALREADY_EXISTS)   
		{   
			return   FALSE;  
		}   
	}   

	SetRegistryKey(_T("ECQAlarmClock"));

	m_bAlwaysOnTop  = GetProfileInt(INI_USER, INI_USER_ALWAYS_ON_TOP, FALSE);
	m_nTransparency  = (BYTE) GetProfileInt(INI_USER, INI_USER_TRANSPARENCY, 255);

	// hide app from taskbar by making dialog's owner window invisible
	CWnd wndInvisible;
	LPCTSTR pstrOwnerClass = AfxRegisterWndClass(0);

	if (!wndInvisible.CreateEx(0, 
							pstrOwnerClass, 
							_T(""),
							WS_POPUP, 
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							NULL, 
							0))
	{
		TRACE(_T("failed to create invisible window"));
		return FALSE;
	}

	ProcessCommandLine(__argc, __targv);

	CECQAlarmClockDlg dlg(&wndInvisible);
  
	m_pMainWnd = &dlg;
	dlg.DoModal();

	// destroy the invisible window
	if (wndInvisible.m_hWnd != NULL)
		wndInvisible.DestroyWindow();

	return FALSE;
}

void CECQAlarmClockApp::ProcessCommandLine(int argc, TCHAR *argv[])
{
	TRACE(_T("in CECQAlarmClockApp::ProcessCommandLine:  argc=%d\n"), argc);

#ifdef _DEBUG
	for (int i = 0; i < argc; i++)
		TRACE(_T("argv[%d]=%s\n"), i, argv[i]);
#endif

	int c;
	while ((c = getopt(argc, argv, _T("h:r:s:t:w:x:y:"))) != EOF)
	{
		switch (c)
		{
			case _T('h'):
				m_nHourAdjust = _ttoi(optarg);
				TRACE(_T("   option h=%d\n"), m_nHourAdjust);
				break;

			case _T('r'):
				m_nTransparency = (BYTE)_ttoi(optarg);
				TRACE(_T("   option r=%d\n"), m_nTransparency);
				break;

			case _T('s'):
				m_strSkin = optarg;
				TRACE(_T("   option s: <%s>\n"), m_strSkin);
				break;

			case _T('t'):
				m_strToolTip = optarg;
				TRACE(_T("   option t: <%s>\n"), m_strToolTip);
				break;

			case _T('w'):
				m_bAlwaysOnTop = _ttoi(optarg);
				TRACE(_T("   option w=%d\n"), m_bAlwaysOnTop);
				break;

			case _T('x'):
				m_nX = _ttoi(optarg);
				TRACE(_T("   option x=%d\n"), m_nX);
				break;

			case _T('y'):
				m_nY = _ttoi(optarg);
				TRACE(_T("   option y=%d\n"), m_nY);
				break;

			default:
				TRACE(_T("ERROR - unknown option %c\n"), c);
				break;
		}
	}
}

BOOL CECQAlarmClockApp::InitApplication() 
{
	WNDCLASS wc;
	
	wc.style = CS_DBLCLKS | CS_SAVEBITS | CS_BYTEALIGNWINDOW;
	wc.lpfnWndProc = DefDlgProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = DLGWINDOWEXTRA;
	wc.hInstance = AfxGetInstanceHandle();
	wc.hIcon = LoadIcon(IDR_MAINFRAME);
	wc.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW + 1;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = _T("ECQAlarmClock");
	::RegisterClass(&wc);
	
	return CWinApp::InitApplication();
}
