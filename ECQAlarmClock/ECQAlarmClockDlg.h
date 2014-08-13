// ECQAlarmClockDlg.h : header file
//

#ifndef ECQAlarmClockDLG_H
#define ECQAlarmClockDLG_H

#include "XAnalogClock.h"
#include "XToolTip.h"
#include "resource.h"
#include "alarmclock.h"
#include "IniFile.h"
#include "TaskbarNotifier.h"
/////////////////////////////////////////////////////////////////////////////
// CECQAlarmClockDlg dialog

class CECQAlarmClockDlg : public CDialog
{
// Construction
public:
	CECQAlarmClockDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CECQAlarmClockDlg)
	enum { IDD = IDD_ECQAlarmClock_DIALOG };
	CXAnalogClock	m_Clock;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CECQAlarmClockDlg)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual void OnCancel();
	//}}AFX_VIRTUAL

// Implementation
protected:
	_ALARM *m_AlarmRules;  //max is 200
	CAlarmClock m_Alarm;
	CString m_exePath;

	// must be in same order as m_SkinsArray[] and ID_SKIN_xxxx definitions
	enum SKIN
	{
		Blue = 0,
		Green,
		Red,
		Yellow
	};			

	void CenterRectOnMonitor(CRect& rect);
	void EndCapture();
	void GetConfig();
	SKIN GetSkinFromName(LPCTSTR lpszSkinName);
	void SaveConfig();
	void SetSkin();
	BOOL SetTransparentColor(COLORREF rgb, BOOL bTrans, BYTE transparency);
	void ShowPopup(CPoint point);
	void ReadMainAlarm();
	void SetAlarm();
	void WriteLog(LPCTSTR lpFormat, ...);
	void ExecuteAlarm(__ALARM Alarm);
	CString GetMainPath();

	struct SKIN_MENU_DATA
	{
		UINT nID;
		UINT nClockFaceBitmapId;
		UINT nDateBitmapId;
		TCHAR *pszName;
	};

	static SKIN_MENU_DATA	m_SkinsArray[];
	static int				m_nSkinsUbound;
	SKIN					m_eCurrenSkin;
	HICON					m_hIcon;
	HACCEL					m_hAccel;
	CXToolTip				m_ToolTip;
	BOOL					m_bMouseCaptured;
	BOOL					m_bAlwaysOnTop;
	BOOL					m_bShowSecondHand;
	CPoint					m_ptLButtonDown;
	BYTE					m_nTransparency;

	CTaskbarNotifier m_wndTaskbarNotifier;

	// Generated message map functions
	//{{AFX_MSG(CECQAlarmClockDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnClose();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnAlwaysOnTop();
	afx_msg void OnShowSecondHand();
	afx_msg void OnTransparency();
	afx_msg void OnDestroy();
	afx_msg void OnSetAlarm();
	afx_msg void OnFreeze();
	afx_msg void OnCloseAll();
	afx_msg void OnAdvanceHour();
	afx_msg void OnIncreaseTransparency();
	afx_msg void OnDecreaseTransparency();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg LRESULT OnAlarmClock( WPARAM, LPARAM );
	//}}AFX_MSG
	afx_msg void OnAbout();
	afx_msg void OnSkin(UINT nID);
	afx_msg LRESULT OnECQAlarmClockInit(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //ECQAlarmClockDLG_H
