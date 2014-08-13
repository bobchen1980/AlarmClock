// MagAlarmRules.cpp : implementation file
//

#include "stdafx.h"
#include "ECQAlarmClock.h"
#include "MagAlarmRules.h"
#include "AddAlarmDlg.h"
#include "AddBulkAlarmDlg.h"

// CMagAlarmRules dialog

IMPLEMENT_DYNAMIC(CMagAlarmRules, CDialog)

CMagAlarmRules::CMagAlarmRules(CWnd* pParent /*=NULL*/)
	: CDialog(CMagAlarmRules::IDD, pParent)
{

}

CMagAlarmRules::~CMagAlarmRules()
{
}

void CMagAlarmRules::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMagAlarmRules, CDialog)
	ON_BN_CLICKED(IDOK, &CMagAlarmRules::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CLOSE, &CMagAlarmRules::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON1, &CMagAlarmRules::OnBnClickedAddAlarm)
	ON_BN_CLICKED(IDC_BUTTON2, &CMagAlarmRules::OnBnClickedDeleteAlarm)
	ON_BN_CLICKED(IDC_BUTTON3, &CMagAlarmRules::OnBnClickedDeleteAll)
	ON_BN_CLICKED(IDC_BUTTON4, &CMagAlarmRules::OnBnClickedAddBulk)
END_MESSAGE_MAP()


// CMagAlarmRules message handlers

void CMagAlarmRules::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
// 	CString szTime;
// 	SYSTEMTIME st;
// 	COleDateTime dt;
// 
// 	m_initAlarms->nCount = m_pListCtrl->GetItemCount();
// 	m_initAlarms->nStart = 0;
// 
// 	for(int i=0;i<m_pListCtrl->GetItemCount();i++)
// 	{

// 		szTime = m_pListCtrl->GetItemText(i,0);
// 		dt.ParseDateTime(szTime,0);
// 		dt.GetAsSystemTime(st);
// 		m_initAlarms->Alarm[i].AlarmTime = st;
// 		m_initAlarms->Alarm[i].AlarmSound = m_pListCtrl->GetItemText(i,2);
// 		m_initAlarms->Alarm[i].AlarmSkin = m_pListCtrl->GetItemText(i,3);
// 		m_initAlarms->Alarm[i].AlarmText = m_pListCtrl->GetItemText(i,4);
// 		m_initAlarms->Alarm[i].AlarmPlugin = m_pListCtrl->GetItemText(i,5);
// 		m_initAlarms->Alarm[i].IsAlarmed = false;
// 	}

	ReadAlarms(m_initAlarms);

	OnOK();
}

void CMagAlarmRules::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CMagAlarmRules::OnBnClickedAddAlarm()
{
	// TODO: Add your control notification handler code here
	CAddAlarmDlg dlg;
	__ALARM addAlarm;
	_ALARM *upAlarms;
	CString szTime; 
	SYSTEMTIME st;
	COleDateTime dt;
	int iAfter;
	upAlarms = m_initAlarms;

	if(dlg.DoModal() == IDOK )
	{
		addAlarm = dlg.m_newAlarm;
// 		iAfter = m_pListCtrl->GetItemCount()+1;
// 		st =addAlarm.AlarmTime;
// 		szTime.Format(_T("%d-%d-%d %d:%d:%d"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);
// 		m_pListCtrl->InsertItem(iAfter,_T(""));
// 		m_pListCtrl->SetItemText(iAfter,1,szTime);
// 		m_pListCtrl->SetItemText(iAfter,2,addAlarm.AlarmSound);
// 		m_pListCtrl->SetItemText(iAfter,3,addAlarm.AlarmSkin);
// 		m_pListCtrl->SetItemText(iAfter,4,addAlarm.AlarmText);
// 		m_pListCtrl->SetItemText(iAfter,5,addAlarm.AlarmPlugin);

// 		if(  m_pListCtrl->GetItemCount() > MAXALARM )
// 		{
// 			AfxMessageBox(_T("The max rule is 100!"));
// 			return;
// 		}
// 
// 		for(int i=0;i<m_pListCtrl->GetItemCount();i++)
// 		{
// 			szTime = m_pListCtrl->GetItemText(i,0);
// 			dt.ParseDateTime(szTime,0);
// 			dt.GetAsSystemTime(st);
// 			upAlarms->Alarm[i].AlarmTime = st;
// 			upAlarms->Alarm[i].AlarmSound = m_pListCtrl->GetItemText(i,2);
// 			upAlarms->Alarm[i].AlarmSkin = m_pListCtrl->GetItemText(i,3);
// 			upAlarms->Alarm[i].AlarmText = m_pListCtrl->GetItemText(i,4);
// 			upAlarms->Alarm[i].AlarmPlugin = m_pListCtrl->GetItemText(i,5);
// 			//upAlarms->Alarm[i].IsAlarmed = false;
// 		}
// 		iAfter = m_pListCtrl->GetItemCount();

		ReadAlarms(upAlarms);
		iAfter = upAlarms->nCount;
		upAlarms->nCount = iAfter + 1;
		upAlarms->Alarm[iAfter] = addAlarm;

		SortAlarm(upAlarms);

		UploadList(upAlarms);
	}
}

void CMagAlarmRules::SortAlarm(_ALARM *alarms)
{
	__ALARM sortAlarm;
	bool    isFlag;
	int     n = alarms->nCount;

	FILETIME	firstFT;
	FILETIME	secondFT;

	for(int i=0;i<n;i++)
	{
		isFlag = false;
		for( int j=0;j<n-1-i;j++) 
		{
			VERIFY(SystemTimeToFileTime(&alarms->Alarm[j+1].AlarmTime, &secondFT));
			VERIFY(SystemTimeToFileTime(&alarms->Alarm[j].AlarmTime, &firstFT));
			if (CompareFileTime(&firstFT, &secondFT) >= 0)
			{
				sortAlarm = alarms->Alarm[j+1];
				alarms->Alarm[j+1] = alarms->Alarm[j];
				alarms->Alarm[j] = sortAlarm;
			}
			isFlag=true;
		}
		if(!isFlag)
			return;
	}
}

void CMagAlarmRules::OnBnClickedDeleteAlarm()
{
	// TODO: Add your control notification handler code here
	for(int i=0;i<m_pListCtrl->GetItemCount();i++)
	{	
		if(m_pListCtrl->GetCheck(i))
			m_pListCtrl->DeleteItem(i--);
	}
	ReadAlarms(m_initAlarms);
}

BOOL CMagAlarmRules::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_pListCtrl = (CListCtrl *)GetDlgItem(IDC_LIST1);

	m_pListCtrl->SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_CHECKBOXES|LVS_EX_FULLROWSELECT);
	m_pListCtrl->InsertColumn(0,_T("Alarm Time"),LVCFMT_LEFT,130);
	m_pListCtrl->InsertColumn(1,_T("Status"),LVCFMT_LEFT,60);
	m_pListCtrl->InsertColumn(2,_T("Sound"),LVCFMT_LEFT,60);
	m_pListCtrl->InsertColumn(3,_T("Skin"),LVCFMT_LEFT,60);
	m_pListCtrl->InsertColumn(4,_T("PopText"),LVCFMT_LEFT,60);
	m_pListCtrl->InsertColumn(5,_T("Plugin"),LVCFMT_LEFT,100);


	UploadList(m_initAlarms);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
 void CMagAlarmRules::UploadList(_ALARM *upAlarms)
 {
	CString szTime; 
	SYSTEMTIME st;
	CString szNum;
	
	m_pListCtrl->DeleteAllItems();
	for(int i=0;i<upAlarms->nCount;i++)
	{
		st = upAlarms->Alarm[i].AlarmTime;
		szTime.Format(_T("%d-%.2d-%.2d %.2d:%.2d:%.2d"),st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond);

		if(upAlarms->Alarm[i].IsAlarmed)
			szNum = _T("Alarmed");
		else
			szNum = _T("Wait");

		m_pListCtrl->InsertItem(i,szTime);
		//m_pListCtrl->SetItemText(i,0,szNum);
		m_pListCtrl->SetItemText(i,1,szNum);
		m_pListCtrl->SetItemText(i,2,upAlarms->Alarm[i].AlarmSound);
		m_pListCtrl->SetItemText(i,3,upAlarms->Alarm[i].AlarmSkin);
		m_pListCtrl->SetItemText(i,4,upAlarms->Alarm[i].AlarmText);
		m_pListCtrl->SetItemText(i,5,upAlarms->Alarm[i].AlarmPlugin);

	}

 }
void CMagAlarmRules::EmptyAlarms(_ALARM *upAlarms)
{
	SYSTEMTIME curst;
	GetLocalTime(&curst);
	curst.wYear = 2000;
	upAlarms->nCount = 0;
	upAlarms->nStart = 0;
	for(int i=0;i<MAXALARM;i++)
	{
		if(i >= MAXALARM-2)
			break;

		upAlarms->Alarm[i].IsAlarmed = true;
		upAlarms->Alarm[i].AlarmTime = curst;
		_tcscpy(upAlarms->Alarm[i].AlarmText,_T(""));
		_tcscpy(upAlarms->Alarm[i].AlarmSound,_T(""));
		_tcscpy(upAlarms->Alarm[i].AlarmPlugin,_T(""));
		_tcscpy(upAlarms->Alarm[i].AlarmSkin,_T(""));
	}
}

void CMagAlarmRules::ReadAlarms(_ALARM *upAlarms)
{
	CString szTime;
	SYSTEMTIME st;
	COleDateTime dt;

	EmptyAlarms(upAlarms);

	upAlarms->nCount = m_pListCtrl->GetItemCount();
	upAlarms->nStart = 0;

	for(int i=0;i<m_pListCtrl->GetItemCount();i++)
	{

		szTime = m_pListCtrl->GetItemText(i,0);
		dt.ParseDateTime(szTime,0);
		dt.GetAsSystemTime(st);
		upAlarms->Alarm[i].AlarmTime = st;
		_tcscpy(upAlarms->Alarm[i].AlarmSound , (TCHAR *)(LPCTSTR )m_pListCtrl->GetItemText(i,2));
		_tcscpy(upAlarms->Alarm[i].AlarmSkin , (TCHAR *)(LPCTSTR )m_pListCtrl->GetItemText(i,3));
		_tcscpy(upAlarms->Alarm[i].AlarmText , (TCHAR *)(LPCTSTR )m_pListCtrl->GetItemText(i,4));
		_tcscpy(upAlarms->Alarm[i].AlarmPlugin , (TCHAR *)(LPCTSTR )m_pListCtrl->GetItemText(i,5));
		upAlarms->Alarm[i].IsAlarmed = false;
	}
}

 void CMagAlarmRules::OnBnClickedDeleteAll()
 {
	 // TODO: Add your control notification handler code here
	 m_pListCtrl->DeleteAllItems(); 
	 EmptyAlarms(m_initAlarms);
	 //OnBnClickedOk();
 }

 void CMagAlarmRules::OnBnClickedAddBulk()
 {
	 // TODO: Add your control notification handler code here
	 CAddBulkAlarmDlg dlg;
	 __ALARM arrAlarms[4];
	 _ALARM  *upBulkAlarms;
	 _ALARMPARAM newParam;
	 CString szTime; 
	 SYSTEMTIME st,st1,st2,st3,st4;
	 COleDateTime dt;
	 CTime offsetTime,ct1,ct2,ct3,ct4;
	 CTimeSpan subSpan,addSpan;
	 int iAfter,iMaxItem=0;

	 upBulkAlarms = m_initAlarms;

	 if(dlg.DoModal() == IDOK)
	 {
		memcpy(arrAlarms,dlg.m_AlarmRule,sizeof(dlg.m_AlarmRule));
		newParam = dlg.m_AlarmParam;

		//AfxMessageBox(newParam.ctStart.Format(_T("%Y-%m-%d %H:%M:%S")));
		if(  m_pListCtrl->GetItemCount() >= MAXALARM )
		{
			AfxMessageBox(_T("The max rule is 100!"));
			return;
		}
//		iAfter = m_pListCtrl->GetItemCount();
// 		for(int i=0;i<m_pListCtrl->GetItemCount();i++)
// 		{
// 			szTime = m_pListCtrl->GetItemText(i,0);
// 			dt.ParseDateTime(szTime,0);
// 			dt.GetAsSystemTime(st);
// 			upBulkAlarms->Alarm[i].AlarmTime = st;
// 			upBulkAlarms->Alarm[i].AlarmSound = m_pListCtrl->GetItemText(i,2);
// 			upBulkAlarms->Alarm[i].AlarmSkin = m_pListCtrl->GetItemText(i,3);
// 			upBulkAlarms->Alarm[i].AlarmText = m_pListCtrl->GetItemText(i,4);
// 			upBulkAlarms->Alarm[i].AlarmPlugin = m_pListCtrl->GetItemText(i,5);
// 		}

		ReadAlarms(upBulkAlarms);
		iAfter = upBulkAlarms->nCount;
		subSpan = newParam.ctEnd - newParam.ctStart;
		iMaxItem = subSpan.GetTotalSeconds() / newParam.nLoopSencond;
		offsetTime =  newParam.ctStart;
		if( int nTotal = (iAfter + iMaxItem)>= MAXALARM)
			iMaxItem = MAXALARM - iAfter;

		for(int i=0;i<iMaxItem;i++)
		{
			ct1 = offsetTime + CTimeSpan(newParam.nDelay1);
			ct2= offsetTime + CTimeSpan(newParam.nDelay2);
			ct3 = offsetTime + CTimeSpan(newParam.nDelay3);
			ct4 = offsetTime + CTimeSpan(newParam.nDelay4);

			ct1.GetAsSystemTime(st1);
			ct2.GetAsSystemTime(st2);
			ct3.GetAsSystemTime(st3);
			ct4.GetAsSystemTime(st4);

			arrAlarms[0].AlarmTime = st1;
			arrAlarms[1].AlarmTime = st2;
			arrAlarms[2].AlarmTime = st3;
			arrAlarms[3].AlarmTime = st4;
			
			if(iAfter >= MAXALARM)
				break;
			upBulkAlarms->Alarm[iAfter++] = arrAlarms[0];
			upBulkAlarms->Alarm[iAfter++] = arrAlarms[1];
			upBulkAlarms->Alarm[iAfter++] = arrAlarms[2];
			upBulkAlarms->Alarm[iAfter++] = arrAlarms[3];
			upBulkAlarms->nCount = iAfter ;
			upBulkAlarms->nStart = 0;	

			offsetTime += CTimeSpan(newParam.nLoopSencond);

		}

		SortAlarm(upBulkAlarms);

		UploadList(upBulkAlarms);
	 }
 }
