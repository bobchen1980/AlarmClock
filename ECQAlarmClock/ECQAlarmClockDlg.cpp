#include "stdafx.h"
#include "ECQAlarmClock.h"
#include "ECQAlarmClockDlg.h"
#include "about.h"
#include "TitleMenu.h"
#include "WindowPos.h"
#include "About.h"
#include "Mmsystem.h"
#include "PasswordDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// these variables should have been defined in some standard header but are not
#ifndef WS_EX_LAYERED
#define WS_EX_LAYERED 0x00080000 
#endif
#ifndef LWA_COLORKEY
#define LWA_COLORKEY	1	// Use color as the transparency color.
#endif
#ifndef LWA_ALPHA
#define LWA_ALPHA		2	// Use bAlpha to determine the opacity of the layer
#endif

typedef BOOL (WINAPI *lpfnSetLayeredWindowAttributes)
				(HWND hWnd, COLORREF cr, BYTE bAlpha, DWORD dwFlags);

static lpfnSetLayeredWindowAttributes g_pSetLayeredWindowAttributes = NULL;
static CPoint g_pointScreen = CPoint(0, 0);

static UINT WM_ECQAlarmClock_INIT = ::RegisterWindowMessage(_T("WM_ECQAlarmClock_INIT"));

///////////////////////////////////////////////////////////////////////////////
// skin definitions - must be in same order as ID_SKIN_xxxx definitions
//
CECQAlarmClockDlg::SKIN_MENU_DATA CECQAlarmClockDlg::m_SkinsArray[] =
{
	ID_SKIN_DARKBLUE,      IDB_DARKBLUE_CLOCK_FACE,      IDB_DARKBLUE_CLOCK_FACE,    _T("Blue"),
	ID_SKIN_GREEN,         IDB_GREEN_CLOCK_FACE,         IDB_GREEN_CLOCK_FACE,  _T("Green"),
	ID_SKIN_RED,           IDB_RED_CLOCK_FACE,           IDB_RED_CLOCK_FACE,  _T("Red"),
	ID_SKIN_YELLOW,        IDB_YELLOW_CLOCK_FACE,        IDB_YELLOW_CLOCK_FACE,  _T("Yellow"),
	0,                     0,                            0,                0
};

int CECQAlarmClockDlg::m_nSkinsUbound = sizeof(m_SkinsArray)/sizeof(m_SkinsArray[0]);

static CPtrArray g_hWndArray;


///////////////////////////////////////////////////////////////////////////////
// CECQAlarmClockDlg dialog

BEGIN_MESSAGE_MAP(CECQAlarmClockDlg, CDialog)
	//{{AFX_MSG_MAP(CECQAlarmClockDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_DESTROY()
	ON_COMMAND(ID_EXIT, OnClose)
	ON_COMMAND(ID_ALWAYS_ON_TOP, OnAlwaysOnTop)
	ON_COMMAND(ID_SHOW_SECOND_HAND, OnShowSecondHand)
	ON_COMMAND(ID_TRANSPARENCY, OnTransparency)
	ON_COMMAND(ID_SET_ALARM, OnSetAlarm)
	ON_COMMAND(ID_FREEZE, OnFreeze)
	ON_COMMAND(ID_CLOSE_ALL, OnCloseAll)
	ON_COMMAND(ID_ADVANCE_HOUR, OnAdvanceHour)
	ON_COMMAND(ID_INCREASE_TRANSPARENCY, OnIncreaseTransparency)
	ON_COMMAND(ID_DECREASE_TRANSPARENCY, OnDecreaseTransparency)
	ON_COMMAND(ID_APP_ABOUT, OnAbout)
	ON_MESSAGE(UWM_TIMER, OnAlarmClock)
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(ID_SKIN_DARKBLUE, ID_SKIN_YELLOW, OnSkin)
	ON_REGISTERED_MESSAGE(WM_ECQAlarmClock_INIT, OnECQAlarmClockInit)
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
CECQAlarmClockDlg::CECQAlarmClockDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CECQAlarmClockDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CECQAlarmClockDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bMouseCaptured  = FALSE;
	m_bAlwaysOnTop    = FALSE;
	m_bShowSecondHand = TRUE;
	m_ptLButtonDown   = CPoint(0, 0);
	m_eCurrenSkin           = Blue;
	m_nTransparency   = 255;

	HMODULE hUser32 = GetModuleHandle(_T("USER32.DLL"));
	g_pSetLayeredWindowAttributes = (lpfnSetLayeredWindowAttributes)
			GetProcAddress(hUser32, "SetLayeredWindowAttributes");
}

///////////////////////////////////////////////////////////////////////////////
void CECQAlarmClockDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CECQAlarmClockDlg)
	DDX_Control(pDX, IDC_FRAME, m_Clock);
	//}}AFX_DATA_MAP
}

///////////////////////////////////////////////////////////////////////////////
BOOL CECQAlarmClockDlg::OnInitDialog()
{
	ModifyStyleEx(WS_EX_APPWINDOW, 0);	// hide app from taskbar

	CDialog::OnInitDialog();

	m_hAccel = LoadAccelerators(AfxGetInstanceHandle(), 
					MAKEINTRESOURCE(IDR_ACCELERATOR));

	GetConfig();

	m_exePath = GetMainPath();

	///////////////////////////////////////////////////////////////////////////

	SetTransparentColor(RGB(255,0,255), TRUE, m_nTransparency);

	// set resource IDs
	SetSkin();

	// set control size

	CSize size = m_Clock.GetWindowSize();

	CRect rect;
	m_Clock.GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.right = rect.left + size.cx;
	rect.bottom = rect.top + size.cy;
	m_Clock.MoveWindow(&rect);

	CRect rectDlg;
	GetWindowRect(&rectDlg);

	// set position

	CenterRectOnMonitor(rectDlg);

	CString strPos = _T("");
	strPos.Format(_T("0,1,%d,%d,%d,%d"), 
		rectDlg.left, rectDlg.top, rectDlg.right, rectDlg.bottom);

	RestoreWindowPos(m_hWnd, INI_PROGRAM_MAIN_WINDOW, NULL, strPos);

	if (!theApp.m_strToolTip.IsEmpty())
	{
		// tooltip will only display if started from command line
		TRACE(_T(",,,,, tooltip=<%s>\n"), theApp.m_strToolTip);
		m_ToolTip.Create(m_hWnd, AfxGetInstanceHandle());
	}

	///////////////////////////////////////////////////////////////////////////
	// command line args override registry values
	m_Clock.ShowDate(false);
	m_Clock.SetHourAdjust(theApp.m_nHourAdjust);

	if ((theApp.m_nX != BAD_XY) || (theApp.m_nY != BAD_XY))
	{
		CRect rect;
		GetWindowRect(&rect);
		int w = rect.Width();
		int h = rect.Height();
		if (theApp.m_nX != BAD_XY)
		{
			rect.left = theApp.m_nX;
			rect.right = rect.left + w;
		}
		if (theApp.m_nY != BAD_XY)
		{
			rect.top = theApp.m_nY;
			rect.bottom = rect.top + h;
		}
		TRACERECT(rect);
		MoveWindow(&rect);
	}

	if (!theApp.m_strSkin.IsEmpty())
	{
		m_eCurrenSkin = GetSkinFromName(theApp.m_strSkin);
		SetSkin();
	}

	//init program
	m_AlarmRules = new _ALARM();

	ReadMainAlarm();

	// start the clock
	m_Clock.Run();

	m_wndTaskbarNotifier.Create(this);
	m_wndTaskbarNotifier.SetSkin(IDB_POPBG_RESOURCE);
	m_wndTaskbarNotifier.SetTextFont(_T("Arial"),180,TN_TEXT_BOLD,TN_TEXT_UNDERLINE);
	m_wndTaskbarNotifier.SetTextColor(RGB(0,0,0),RGB(0,0,200));
	m_wndTaskbarNotifier.SetTextRect(CRect(20,40,m_wndTaskbarNotifier.m_nSkinWidth-10,m_wndTaskbarNotifier.m_nSkinHeight-25));
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

///////////////////////////////////////////////////////////////////////////////
CECQAlarmClockDlg::SKIN CECQAlarmClockDlg::GetSkinFromName(LPCTSTR lpszSkinName)
{
	if ((m_eCurrenSkin < 0) || (m_eCurrenSkin >= m_nSkinsUbound))
	{
		TRACE(_T("ERROR - invalid skin %d\n"), m_eCurrenSkin);
		ASSERT(FALSE);
		m_eCurrenSkin = Blue;
	}
	
	SKIN skin = m_eCurrenSkin;

	if (lpszSkinName && (lpszSkinName[0] != 0))
	{
		// these are the names that may be entered on command line -
		// must be in same order as m_SkinsArray[] and ID_SKIN_xxxx definitions

		TCHAR * skins[] = 
		{
			_T("Blue"),
			_T("Green"),
			_T("Red"),
			_T("Yellow"),
			0
		};

		for (int i = 0; skins[i] != 0; i++)
		{
			//if (_tcsicmp(skins[i], theApp.m_strSkin) == 0)
			if (_tcsicmp(skins[i], lpszSkinName) == 0)
			{
				skin = (SKIN) i;
				break;
			}
		}
	}

	return skin;
}

///////////////////////////////////////////////////////////////////////////////
void CECQAlarmClockDlg::SetSkin()
{
	if ((m_eCurrenSkin < 0) || (m_eCurrenSkin >= m_nSkinsUbound))
	{
		TRACE(_T("ERROR - invalid skin %d\n"), m_eCurrenSkin);
		ASSERT(FALSE);
		m_eCurrenSkin = Blue;
	}
	
	m_Clock.Stop();

	m_Clock.SetBitmaps(m_SkinsArray[m_eCurrenSkin].nClockFaceBitmapId, 
		m_SkinsArray[m_eCurrenSkin].nDateBitmapId);

	// set custom hand colors, depending on skin

	if (m_eCurrenSkin == Green)
	{
		m_Clock.SetHourHandColor(RGB(0,100,0))
			.SetMinuteHandColor(RGB(0,100,0))
			.SetSecondHandColor(RGB(220,20,60))
			.SetAutomaticHandColor(FALSE);
	}
	else if (m_eCurrenSkin == Red)
	{
		m_Clock.SetHourHandColor(RGB(255,255,95))
			.SetMinuteHandColor(RGB(255,255,95))
			.SetSecondHandColor(RGB(255,255,255))
			.SetAutomaticHandColor(FALSE);
	}
	else if (m_eCurrenSkin == Yellow)
	{
		m_Clock.SetHourHandColor(RGB(80,90,172))
			.SetMinuteHandColor(RGB(80,90,172))
			.SetSecondHandColor(RGB(220,20,60))
			.SetAutomaticHandColor(FALSE);
	}
	else
	{
		m_Clock.SetHourHandColor(RGB(0,0,139))
			.SetMinuteHandColor(RGB(0,0,139))
			.SetSecondHandColor(RGB(254,80,90))
			.SetAutomaticHandColor(FALSE);
	}

	// must repaint background tom ensure bitmap transparency
	Invalidate();

	m_Clock.Run();
}

///////////////////////////////////////////////////////////////////////////////
void CECQAlarmClockDlg::GetConfig()
{
	TRACE(_T("in CECQAlarmClockDlg::GetConfig\n"));

	// following are now read in ECQAlarmClock.cpp
	//m_bAlwaysOnTop   = theApp.GetProfileInt(INI_USER, INI_USER_ALWAYS_ON_TOP, FALSE);
	//m_nTransparency  = (BYTE)theApp.GetProfileInt(INI_USER, INI_USER_TRANSPARENCY, 255);

	m_bAlwaysOnTop = theApp.m_bAlwaysOnTop;
	m_bAlwaysOnTop = !m_bAlwaysOnTop;
	OnAlwaysOnTop();

	m_bShowSecondHand = theApp.GetProfileInt(INI_USER, INI_USER_SHOW_SECOND_HAND, TRUE);
	m_bShowSecondHand = !m_bShowSecondHand;
	OnShowSecondHand();

	m_nTransparency = theApp.m_nTransparency;

	int nSkin        = theApp.GetProfileInt(INI_USER, INI_USER_SKIN, 0);
	if ((nSkin < 0) || (nSkin >= m_nSkinsUbound))
		nSkin = 0;
	m_eCurrenSkin = (SKIN) nSkin;
}

///////////////////////////////////////////////////////////////////////////////
void CECQAlarmClockDlg::SaveConfig()
{
	theApp.WriteProfileInt(INI_USER, INI_USER_ALWAYS_ON_TOP, m_bAlwaysOnTop);
	theApp.WriteProfileInt(INI_USER, INI_USER_SHOW_SECOND_HAND, m_bShowSecondHand);
	theApp.WriteProfileInt(INI_USER, INI_USER_SKIN,          m_eCurrenSkin);
	theApp.WriteProfileInt(INI_USER, INI_USER_TRANSPARENCY,  m_nTransparency);
}

///////////////////////////////////////////////////////////////////////////////
BOOL CECQAlarmClockDlg::SetTransparentColor(COLORREF rgb, BOOL bTrans, BYTE transparency)
{
	if (!g_pSetLayeredWindowAttributes)
		return FALSE;
	
	if (bTrans)
	{
		// set layered style for the dialog
		SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);

		// call it with 0 alpha for the given color
		g_pSetLayeredWindowAttributes(m_hWnd, rgb, transparency, LWA_COLORKEY|LWA_ALPHA);
	}
	else
	{
		SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);
		
		// Ask the window and its children to repaint
		::RedrawWindow(m_hWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
	}
	
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
void CECQAlarmClockDlg::CenterRectOnMonitor(CRect& rect)
{
	int dlgw = rect.Width();
	int dlgh = rect.Height();

	int monw = GetSystemMetrics(SM_CXSCREEN);
	int monh = GetSystemMetrics(SM_CYSCREEN);

	CPoint centerpoint(monw/2, monh/2);

	HMONITOR hMon = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);

	if (hMon)
	{
		MONITORINFO mi;

		mi.cbSize = sizeof(mi);
		GetMonitorInfo(hMon, &mi);
		CRect r = mi.rcMonitor;

		centerpoint.x = r.left + r.Width()/2;
		centerpoint.y = r.top + r.Height()/2;
	}

	rect.left   = centerpoint.x - dlgw/2;
	rect.right  = rect.left + dlgw;
	rect.top    = centerpoint.y - dlgh/2;
	rect.bottom = rect.top + dlgh;
}

///////////////////////////////////////////////////////////////////////////////
void CECQAlarmClockDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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
		CDialog::OnPaint();
	}
}

///////////////////////////////////////////////////////////////////////////////
HCURSOR CECQAlarmClockDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

///////////////////////////////////////////////////////////////////////////////
void CECQAlarmClockDlg::OnRButtonUp(UINT nFlags, CPoint point) 
{
	ShowPopup(point);
	
	CDialog::OnRButtonUp(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
//
// The purpse of this function is to send a left mouse click to the
// underlying window.
//
BOOL CALLBACK EnumWindowsProc(HWND hWnd,		// handle to window
							  LPARAM lParam)	// HWND of caller
{
	HWND me = (HWND) lParam;

	if (hWnd == me)
	{
		// this is my window - ignore
		return TRUE;
	}

	CRect rect;
	::GetWindowRect(hWnd, &rect);

	if (rect.PtInRect(g_pointScreen))
	{
		// found window with the point that was clicked
		TCHAR szBuf[100];
		::SendMessage(hWnd, WM_GETTEXT, 50, (LPARAM)szBuf);
		szBuf[50] = 0;
		TRACE(_T("over window '%s'\n"), szBuf);

		HWND hParent = ::GetParent(hWnd);

		// loop to find top-most parent
		while (hParent)
		{
			HWND h = ::GetParent(hParent);
			if (h)
				hParent = h;
			else
				break;
		}

		if (hParent == 0)
			hParent = hWnd;

		TRACE(_T("hParent=%X\n"), hParent);

		CPoint pointClient(g_pointScreen);
		::ScreenToClient(hParent, &pointClient);

		// temporarily hide the window, so mouse click will go to correct window
		::ShowWindow(me, SW_HIDE);

		INPUT input[2] = 
		{
			INPUT_MOUSE, { pointClient.x, pointClient.y, 0, MOUSEEVENTF_LEFTDOWN, 0, 0},
			INPUT_MOUSE, { pointClient.x, pointClient.y, 0, MOUSEEVENTF_LEFTUP, 0, 0},
		};

		// send 2 left-button messages
		::SendInput(2, input, sizeof(INPUT));

		// must wait ...
		Sleep(100);

		// re-show window
		::ShowWindow(me, SW_SHOWNOACTIVATE);

		// stop enumeration
		return FALSE;
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
LRESULT CECQAlarmClockDlg::OnECQAlarmClockInit(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	TRACE(_T("in CECQAlarmClockDlg::OnECQAlarmClockInit: 0x%X\n"), m_hWnd);

	if (__argc > 1)
		return 15759;	// respond to broadcast message with a unique code,
						// if started with command line options
	else
		return 0;
}

///////////////////////////////////////////////////////////////////////////////
void CALLBACK SendAsyncProc(HWND hWnd,			// handle to destination window
							UINT uMsg,			// message
							ULONG_PTR dwData,	// application-defined value
							LRESULT lResult)	// result of message processing
{
	//TRACE(_T("in SendAsyncProc:  hWnd=0x%X  lResult=%d\n"), hWnd, lResult);

	HWND me = (HWND) dwData;

	if ((uMsg == WM_ECQAlarmClock_INIT) && (hWnd != me) && (lResult == 15759))
	{
		// this callback is from a ECQAlarmClock app
		TRACE(_T("adding 0x%X\n"), hWnd);
		g_hWndArray.Add(hWnd);
	}
}

///////////////////////////////////////////////////////////////////////////////
void CECQAlarmClockDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	TRACE(_T("in CECQAlarmClockDlg::OnLButtonDown\n"));
	m_bMouseCaptured = TRUE;

	SetCapture();
	
	m_ptLButtonDown = point;

	GetCursorPos(&g_pointScreen);

	static BOOL bFirstTime = TRUE;
	if (bFirstTime)
	{
		// collect HWNDs of other ECQAlarmClock apps
		TRACE(_T("calling SendMessageCallback\n"));
		::SendMessageCallback(HWND_BROADCAST, WM_ECQAlarmClock_INIT, 0, 0, 
			SendAsyncProc, (ULONG_PTR) m_hWnd);
	}
	bFirstTime = FALSE;

	CDialog::OnLButtonDown(nFlags, point);
}


///////////////////////////////////////////////////////////////////////////////
void CECQAlarmClockDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	TRACE(_T("in CECQAlarmClockDlg::OnLButtonUp\n"));

	EndCapture();

	// if not fully opaque, allow click-thru
	if (m_nTransparency != 255)
	{
		CPoint pt;
		GetCursorPos(&pt);

		if (pt == g_pointScreen)
		{
			TRACE(_T("same point\n"));
			::EnumWindows(EnumWindowsProc, (LPARAM)m_hWnd);
		}
	}

	CDialog::OnLButtonUp(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
void CECQAlarmClockDlg::EndCapture()
{
	if (m_bMouseCaptured)
	{
		TRACE(_T("releasing mouse\n"));
		::ReleaseCapture();
	}
	m_bMouseCaptured = FALSE;
}

///////////////////////////////////////////////////////////////////////////////
void CECQAlarmClockDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bMouseCaptured)
	{
		//TRACE(_T("in CECQAlarmClockDlg::OnMouseMove - mouse captured\n"));
		if (!theApp.m_strToolTip.IsEmpty())
			m_ToolTip.Hide();

		CRect rectDlg;
		GetWindowRect(&rectDlg);
		int w = rectDlg.Width();
		int h = rectDlg.Height();
		rectDlg.left  += point.x - m_ptLButtonDown.x;
		rectDlg.right  = rectDlg.left + w;
		rectDlg.top   += point.y - m_ptLButtonDown.y;
		rectDlg.bottom = rectDlg.top + h;
		MoveWindow(&rectDlg);

		// don't move other apps if Ctrl key is down
		if ((__argc > 1) && (GetAsyncKeyState(VK_CONTROL) >= 0))
		{
			// we were started from command line, so move other ECQAlarmClock apps too

			TRACE(_T("moving other apps\n"));

			for (int i = 0; i < g_hWndArray.GetSize(); i++)
			{
				HWND hWnd = (HWND) g_hWndArray.GetAt(i);

				if (hWnd && IsWindow(hWnd))
				{
					CRect rect;
					::GetWindowRect(hWnd, &rect);
					int w = rect.Width();
					int h = rect.Height();
					rect.left  += point.x - m_ptLButtonDown.x;
					rect.right  = rectDlg.left + w;
					rect.top   += point.y - m_ptLButtonDown.y;
					rect.bottom = rectDlg.top + h;
					::MoveWindow(hWnd, rect.left, rect.top, w, h, FALSE);
				}
			}
		}
	}
	else if (!theApp.m_strToolTip.IsEmpty())
	{
		CPoint pt;
		::GetCursorPos(&pt);
		m_ToolTip.Show(theApp.m_strToolTip, pt.x, pt.y);
	}

	CDialog::OnMouseMove(nFlags, point);
}

///////////////////////////////////////////////////////////////////////////////
BOOL CECQAlarmClockDlg::OnEraseBkgnd(CDC* pDC) 
{
	// fill background with transparent color
	CRect rect;
	GetWindowRect(&rect);
	ScreenToClient(&rect);
	CBrush brush(RGB(255,0,255));
	pDC->FillRect(&rect, &brush);
	return TRUE;//CDialog::OnEraseBkgnd(pDC);
}

///////////////////////////////////////////////////////////////////////////////
void CECQAlarmClockDlg::OnClose() 
{
	CPasswordDlg dlg;
	if(dlg.DoModal() == IDOK)
	{
		if(dlg.m_Password != _T("2008"))
			return;
	}else return;

	CDialog::OnCancel();	
}

///////////////////////////////////////////////////////////////////////////////
void CECQAlarmClockDlg::OnCancel() 
{
	// don't let ESC key close the dialog
}

///////////////////////////////////////////////////////////////////////////////
void CECQAlarmClockDlg::OnDestroy() 
{
	// don't save options if started with command line args

	if (__argc <= 1)
	{
		SaveWindowPos(m_hWnd, INI_PROGRAM_MAIN_WINDOW);
		SaveConfig();
	}

	delete m_AlarmRules;

	CDialog::OnDestroy();
}

///////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK EnumWindowsProcCloseAll(HWND hWnd,	// handle to window
									 LPARAM lParam)	// HWND of caller
{
	HWND me = (HWND) lParam;

	if (hWnd == me)
	{
		// this is my window - ignore
		return TRUE;
	}

	TCHAR szClassName[100];
	::GetClassName(hWnd, szClassName, sizeof(szClassName)/sizeof(TCHAR)-1);
	szClassName[sizeof(szClassName)/sizeof(TCHAR)-1] = 0;

	if (_tcsicmp(szClassName, _T("ECQAlarmClock")) == 0)
	{
		TRACE(_T("closing window 0x%X\n"), hWnd);
		::SendMessage(::GetParent(hWnd), WM_CLOSE, 0, 0);
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
void CECQAlarmClockDlg::OnCloseAll() 
{
	TRACE(_T("in CECQAlarmClockDlg::OnCloseAll\n"));
	::EnumWindows(EnumWindowsProcCloseAll, (LPARAM)m_hWnd);
	GetParent()->SendMessage(WM_CLOSE);		// close this instance
}

///////////////////////////////////////////////////////////////////////////////
BOOL CECQAlarmClockDlg::PreTranslateMessage(MSG* pMsg) 
{
	//TRACE(_T("in CECQAlarmClockDlg::PreTranslateMessage\n"));
	TranslateAccelerator(m_hWnd, m_hAccel, pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

///////////////////////////////////////////////////////////////////////////////
BOOL CECQAlarmClockDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	TRACE(_T("in CECQAlarmClockDlg::OnMouseWheel\n"));
	if (nFlags & MK_CONTROL)
	{
		// Ctrl key is down
		TRACE(_T("ctrl key down\n"));
		if (zDelta < 0)
			OnIncreaseTransparency();
		else
			OnDecreaseTransparency();
	}
	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}


void CECQAlarmClockDlg::ReadMainAlarm()
{
	SYSTEMTIME curst,st;
	COleDateTime dt;
	CString szTime;

	GetLocalTime(&curst);
	st.wYear = 2000;

	m_AlarmRules->nCount = 0;
	m_AlarmRules->nStart = 0;
	for(int i=0;i<MAXALARM;i++)
	{
		//must less 60 max is 59
// 		st.wSecond++;
// 		if(st.wSecond >= 60)
// 		{
// 			st.wSecond = 0;
// 			st.wMinute +=1 ;
// 		}
		
		m_AlarmRules->Alarm[i].IsAlarmed = true;
		m_AlarmRules->Alarm[i].AlarmTime = curst;
		_tcscpy(m_AlarmRules->Alarm[i].AlarmText, _T(""));
		_tcscpy(m_AlarmRules->Alarm[i].AlarmSound, _T(""));
		_tcscpy(m_AlarmRules->Alarm[i].AlarmPlugin, _T(""));
		_tcscpy(m_AlarmRules->Alarm[i].AlarmSkin, _T(""));

	}

	CIni IniFile(m_exePath+_T("Alarm.ini"));
	m_AlarmRules->nCount = IniFile.GetInt(_T("Section1"), _T("nCount"), 0);   
	m_AlarmRules->nStart = IniFile.GetInt(_T("Section1"), _T("nStart"), 0);   
	//IniFile.GetDataBlock(_T("Section2"), _T("Alarm"), &m_AlarmRules->Alarm, sizeof(m_AlarmRules->Alarm));
	for(int i=0;i<m_AlarmRules->nCount;i++)
	{
		CString szNum;
		szNum.Format(_T("Alarm_%d"),i);
		m_AlarmRules->Alarm[i].IsAlarmed = IniFile.GetBool(szNum, _T("IsAlarmed"),false);
		//IniFile.GetDataBlock(szNum, _T("AlarmTime"),&m_AlarmRules->Alarm[i].AlarmTime,sizeof(SYSTEMTIME));
		szTime = IniFile.GetString(szNum, _T("AlarmTime"),_T("2008-08-08 0:0:0"));
		dt.ParseDateTime(szTime,0);
		dt.GetAsSystemTime(st);
		m_AlarmRules->Alarm[i].AlarmTime = st;
		_tcscpy(m_AlarmRules->Alarm[i].AlarmSound , (TCHAR *)(LPCTSTR)IniFile.GetString(szNum, _T("AlarmSound"),_T("")));
		_tcscpy(m_AlarmRules->Alarm[i].AlarmSkin , (TCHAR *)(LPCTSTR)IniFile.GetString(szNum, _T("AlarmSkin"),_T("")));
		_tcscpy(m_AlarmRules->Alarm[i].AlarmText , (TCHAR *)(LPCTSTR)IniFile.GetString(szNum, _T("AlarmText"),_T("")));
		_tcscpy(m_AlarmRules->Alarm[i].AlarmPlugin , (TCHAR *)(LPCTSTR)IniFile.GetString(szNum, _T("AlarmPlugin"),_T("")));
	}

	//start alarm
	SetAlarm();

}

void CECQAlarmClockDlg::SetAlarm()
{
	SYSTEMTIME	CurrentSt;
	FILETIME	CurrentFt;
	FILETIME	AlarmFt;

	GetLocalTime(&CurrentSt);
	VERIFY(SystemTimeToFileTime(&CurrentSt, &CurrentFt));

	for(int i=m_AlarmRules->nStart;i< m_AlarmRules->nCount;i++)
	{
		if(!m_AlarmRules->Alarm[i].IsAlarmed) 
		{
			VERIFY(SystemTimeToFileTime(&m_AlarmRules->Alarm[i].AlarmTime, &AlarmFt));
			if (CompareFileTime(&AlarmFt, &CurrentFt) >= 0)
				break;
			else
			{
				m_AlarmRules->Alarm[i].IsAlarmed = true;
				m_AlarmRules->nStart++;
			}
		}
	}
		
	if(m_AlarmRules->nStart < m_AlarmRules->nCount)
	{
		m_Alarm.SetAlarm(m_AlarmRules->Alarm[m_AlarmRules->nStart].AlarmTime, GetSafeHwnd(), 0);
	}

}

LRESULT CECQAlarmClockDlg::OnAlarmClock( WPARAM, LPARAM )
{

	SYSTEMTIME	st;

	ExecuteAlarm(m_AlarmRules->Alarm[m_AlarmRules->nStart]);
	m_AlarmRules->Alarm[m_AlarmRules->nStart].IsAlarmed = true;

	GetLocalTime(&st);
	WriteLog(_T("%02d/%02d/%04d %02d:%02d:%02d: \t*** Alarm ***\r\n"),
		 st.wMonth, st.wDay, st.wYear, st.wHour, st.wMinute, st.wSecond);
	
	m_AlarmRules->nStart++;
	//next alarm
	SetAlarm();

	return 0;
}

void CECQAlarmClockDlg::ExecuteAlarm(__ALARM Alarm)
{

	//set skin
	if( lstrlen(Alarm.AlarmSkin) > 0)
		m_eCurrenSkin =  GetSkinFromName(Alarm.AlarmSkin);
	else
		m_eCurrenSkin = GetSkinFromName(_T("Blue"));
	SetSkin();
	//paly sound
	if( lstrlen(Alarm.AlarmSound) >0 )
		sndPlaySound(Alarm.AlarmSound,SND_ASYNC);
	else
		Beep(8000,100);

	//call exe file
	if(lstrlen(Alarm.AlarmPlugin) >0 )
	{
		
		ShellExecute(this->m_hWnd,_T("open"),m_exePath + Alarm.AlarmPlugin,_T(""),_T(""),SW_SHOW );
	}

	//pop text
	if(lstrlen(Alarm.AlarmText) > 0)
		m_wndTaskbarNotifier.Show(Alarm.AlarmText,500,10000,500);

// 	if(!m_bAlwaysOnTop)
// 		OnAlwaysOnTop();

}

#define MAX_STRING_LENGTH 1024
void CECQAlarmClockDlg::WriteLog(const LPCTSTR lpFormat, ...)
{
	TCHAR	buffer[MAX_STRING_LENGTH];
	va_list va;
	HANDLE	hFile;
	BOOL	rc;
	DWORD	dwActual;
	//TCHAR	filePath[] = _T(".\\AlarmClock.log");
	CString filePath;
	filePath.Format(_T("%s\\Alarm.log"),m_exePath);

	ZeroMemory(&buffer, MAX_STRING_LENGTH * sizeof(TCHAR));
	va_start(va, lpFormat);
	_vstprintf(buffer, lpFormat, va);
	va_end(va);

	hFile = CreateFile(filePath, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		SetFilePointer(hFile, 0, 0, FILE_END);	// Seek to end of file.
		rc = WriteFile(hFile, buffer, (DWORD) (_tcslen(buffer) * sizeof(TCHAR)), &dwActual, NULL);
		ASSERT(rc);

		CloseHandle(hFile);
	}
}

CString CECQAlarmClockDlg::GetMainPath()
{
	TCHAR cPath[MAX_PATH];
	CString rtnSting;
	
	GetModuleFileName( NULL, cPath, MAX_PATH ); //获取程序路径（包括程序名）
	int i = 0, j;
	while( cPath[i]!=0 )
	{
		if( cPath[i]=='\\' )
			j = i;
		i++;
	}
	cPath[j+1] = '\0';

	rtnSting.Format(_T("%s"),cPath);
	
	return rtnSting;   //分离路径名（去掉程序名）
}