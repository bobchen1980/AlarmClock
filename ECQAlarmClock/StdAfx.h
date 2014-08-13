// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#ifndef  STDAFX_H 
#define  STDAFX_H 

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#define _WIN32_WINNT 0x0500

#if _MSC_VER >= 1400
#ifndef WINVER
#define WINVER 0x0501
#endif
#endif

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

///////////////////////// Pragma TODO macro //////////////////////////////////
// When the compiler sees a line like this:
//      #pragma TODO(Fix this later)
// it outputs a line like this:
//      C:\foo.cpp(296) : TODO ==> Fix this later
// You can easily jump directly to this line and examine the surrounding code.
#define chSTR(x)	   #x
#define chSTR2(x)	chSTR(x)
#define TODO(desc) message(__FILE__ "(" chSTR2(__LINE__) ") : TODO ==> " #desc)

#include "XTrace.h"

///////////////////////////////////////////////////////////////////////////////
// for registry settings
#define INI_USER						_T("User")
#define INI_PROGRAM_MAIN_WINDOW			_T("MainWindow")
#define INI_USER_ALWAYS_ON_TOP			_T("OnTop")
#define INI_USER_SHOW_SECOND_HAND		_T("ShowSecondHand")
#define INI_USER_SKIN					_T("Skin")
#define INI_USER_TRANSPARENCY			_T("Transparency")

#define MAXALARM 100

// typedef 	struct  
// {
// 	bool         IsAlarmed;
// 	SYSTEMTIME   AlarmTime;
// 	TCHAR      *AlarmText ;
// 	TCHAR      *AlarmSkin ;
// 	TCHAR      *AlarmSound;
// 	TCHAR      *AlarmPlugin ;
// }__ALARM;

typedef 	struct  
{
	bool         IsAlarmed;
	SYSTEMTIME   AlarmTime;
	TCHAR      AlarmText[100] ;
	TCHAR      AlarmSkin[100] ;
	TCHAR      AlarmSound[100];
	TCHAR      AlarmPlugin[100] ;
}__ALARM;


typedef struct
{
	int nCount;
	int nStart;
	__ALARM Alarm[MAXALARM];
}_ALARM;

typedef struct  
{
	CTime ctStart;
	CTime ctEnd;
	int   nLoopSencond;
	int   nDelay1;
	int   nDelay2;
	int   nDelay3;
	int   nDelay4;
}_ALARMPARAM;

#if _MFC_VER > 0x700
#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // STDAFX_H 
