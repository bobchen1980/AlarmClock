#include "stdafx.h"
#include "ECQAlarmClockDlg.h"
#include "TitleMenu.h"
#include "about.h"
#include "MagAlarmRules.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
void CECQAlarmClockDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if ((nID & 0xFFF0) == SC_CLOSE)
	{
		OnClose();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

///////////////////////////////////////////////////////////////////////////////
void CECQAlarmClockDlg::OnAbout()
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

///////////////////////////////////////////////////////////////////////////////
void CECQAlarmClockDlg::ShowPopup(CPoint point)
{
	int i = 0;

	UINT nFlags = MF_STRING;

	CTitleMenu popup;
	VERIFY(popup.CreatePopupMenu());
	popup.AddMenuTitle(_T("INTER-NOISE 2008"));
	popup.SetColor(RGB(0,0,255));
	popup.SetGradientColor(CTitleMenu::LightenColor(RGB(255,0,0), .7));
	popup.SetTextColor(RGB(255,255,255));

	///////////////////////////////////////////////////////////////////////////
	// skins
	VERIFY(popup.AppendMenu(MF_SEPARATOR)); //----------------------------------------------------

	VERIFY(popup.AppendMenu(MF_STRING, ID_SET_ALARM, _T("Set Alarm Rules")));
	nFlags = MF_STRING;
	if (m_bAlwaysOnTop)
		nFlags |= MF_CHECKED;
	VERIFY(popup.AppendMenu(nFlags, ID_ALWAYS_ON_TOP, _T("Always On Top")));

	CMenu skins_popup;
	VERIFY(skins_popup.CreatePopupMenu());

	for (i = 0; m_SkinsArray[i].nID != 0; i++)
	{
		nFlags = MF_STRING;
		if (i == m_eCurrenSkin)
			nFlags |= MF_CHECKED;
		VERIFY(skins_popup.AppendMenu(nFlags, m_SkinsArray[i].nID, m_SkinsArray[i].pszName));
	}

	CString s = _T("");

	s.Format(_T("Set Backgound (%s)"), m_SkinsArray[m_eCurrenSkin].pszName);
	VERIFY(popup.AppendMenu(MF_STRING|MF_POPUP, (UINT)(HMENU)skins_popup, s));

	/*s.Format(_T("Transparency (%u) ..."), m_nTransparency);
	VERIFY(popup.AppendMenu(MF_STRING, ID_TRANSPARENCY, s));


	nFlags = MF_STRING;
	if (m_bShowSecondHand)
		nFlags |= MF_CHECKED;
	VERIFY(popup.AppendMenu(nFlags, ID_SHOW_SECOND_HAND, _T("Show Second Hand")));*/

	VERIFY(popup.AppendMenu(MF_SEPARATOR)); //----------------------------------------------------

	VERIFY(popup.AppendMenu(MF_STRING, ID_APP_ABOUT, _T("About Alarm Clock")));

	VERIFY(popup.AppendMenu(MF_SEPARATOR)); //----------------------------------------------------

	//VERIFY(popup.AppendMenu(MF_STRING, ID_CLOSE_ALL, _T("Close All")));
	VERIFY(popup.AppendMenu(MF_STRING, ID_EXIT, _T("Exit With Password")));

	// display the menu

	CPoint pt = point;
	ClientToScreen(&pt);
	VERIFY(popup.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x+1, pt.y+1, this));
}

///////////////////////////////////////////////////////////////////////////////
void CECQAlarmClockDlg::OnAlwaysOnTop()
{
	m_bAlwaysOnTop = !m_bAlwaysOnTop;
	
	if (m_bAlwaysOnTop)
		SetWindowPos(&wndTopMost, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE);
	else
		SetWindowPos(&wndNoTopMost, 0,0,0,0, SWP_NOMOVE|SWP_NOSIZE);
}

///////////////////////////////////////////////////////////////////////////////
void CECQAlarmClockDlg::OnShowSecondHand()
{
	m_bShowSecondHand = !m_bShowSecondHand;
	m_Clock.ShowSecondHand(m_bShowSecondHand);
}

///////////////////////////////////////////////////////////////////////////////
void CECQAlarmClockDlg::OnSkin(UINT nID)
{
	m_eCurrenSkin = (SKIN) (nID - ID_SKIN_DARKBLUE);
	SetSkin();
}

///////////////////////////////////////////////////////////////////////////////
void CECQAlarmClockDlg::OnTransparency()
{
	//SetTransparentColor(RGB(255,0,255), TRUE, m_nTransparency);
}

///////////////////////////////////////////////////////////////////////////////
void CECQAlarmClockDlg::OnSetAlarm() 
{
	CString szNum,szTime,szSound,szSkin,szText,szPlugin;
	CMagAlarmRules dlg;
	SYSTEMTIME st;
	dlg.m_initAlarms = m_AlarmRules;
	if(dlg.DoModal() == IDOK )
	{
		m_AlarmRules = dlg.m_initAlarms;
		
		CIni IniFile(m_exePath+_T("Alarm.ini"));
		IniFile.WriteInt(_T("Section1"), _T("nCount"), m_AlarmRules->nCount);   
		IniFile.WriteInt(_T("Section1"), _T("nStart"), m_AlarmRules->nStart);   
		//IniFile.WriteDataBlock(_T("Section2"), _T("Alarm"), &m_AlarmRules->Alarm, sizeof(m_AlarmRules->Alarm));
		for(int k=0;k<MAXALARM;k++)
		{
			szNum.Format(_T("Alarm_%d"),k);
			IniFile.DeleteSection(szNum);
		}
		for(int i=0;i<m_AlarmRules->nCount;i++)
		{
			szNum.Format(_T("Alarm_%d"),i);
			IniFile.WriteBool(szNum, _T("IsAlarmed"),m_AlarmRules->Alarm[i].IsAlarmed);
			st =  m_AlarmRules->Alarm[i].AlarmTime;
			szTime.Format(_T("%d-%.2d-%.2d %.2d:%.2d:%.2d"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
			//IniFile.WriteDataBlock(szNum, _T("AlarmTime"),&m_AlarmRules->Alarm[i].AlarmTime,sizeof(SYSTEMTIME));
			szSound.Format(_T("%s"),m_AlarmRules->Alarm[i].AlarmSound);
			szSkin.Format(_T("%s"),m_AlarmRules->Alarm[i].AlarmSkin);
			szText.Format(_T("%s"),m_AlarmRules->Alarm[i].AlarmText);
			szPlugin.Format(_T("%s"),m_AlarmRules->Alarm[i].AlarmPlugin);
			IniFile.WriteString(szNum, _T("AlarmTime"),szTime);
			IniFile.WriteString(szNum, _T("AlarmSound"),szSound);
			IniFile.WriteString(szNum, _T("AlarmSkin"),szSkin);
			IniFile.WriteString(szNum, _T("AlarmText"),szText);
			IniFile.WriteString(szNum, _T("AlarmPlugin"),szPlugin);
		}

	}
	
	SetAlarm();
}

///////////////////////////////////////////////////////////////////////////////
void CECQAlarmClockDlg::OnIncreaseTransparency() 
{
	TRACE(_T("in CECQAlarmClockDlg::OnIncreaseTransparency\n"));
	if (m_nTransparency <= 245)
	{
		m_nTransparency += 10;
		SetTransparentColor(RGB(255,0,255), TRUE, m_nTransparency);
	}
}

///////////////////////////////////////////////////////////////////////////////
void CECQAlarmClockDlg::OnDecreaseTransparency() 
{
	TRACE(_T("in CECQAlarmClockDlg::OnDecreaseTransparency\n"));
	if (m_nTransparency >= 40)
	{
		m_nTransparency -= 10;
		SetTransparentColor(RGB(255,0,255), TRUE, m_nTransparency);
	}
}

///////////////////////////////////////////////////////////////////////////////
void CECQAlarmClockDlg::OnAdvanceHour() 
{
#ifdef _DEBUG
	CTime t = m_Clock.GetTime();
	int nHour = t.GetHour();
	nHour += 1;
	if (nHour > 23)
		nHour = 0;
	t = CTime(t.GetYear(), t.GetMonth(), t.GetDay(), nHour, t.GetMinute(), 59);
	m_Clock.UseSystemTime(FALSE);
	m_Clock.SetTime(t);
	m_Clock.RedrawWindow();
#endif
}

///////////////////////////////////////////////////////////////////////////////
void CECQAlarmClockDlg::OnFreeze() 
{
#ifdef _DEBUG
	// The purpose of this is to facilitate screenshots
	m_Clock.ToggleDebug();
#endif
}
