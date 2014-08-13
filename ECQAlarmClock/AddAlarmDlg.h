#pragma once
#include "afxdtctl.h"


// CAddAlarmDlg dialog

class CAddAlarmDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddAlarmDlg)

public:
	CAddAlarmDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAddAlarmDlg();

// Dialog Data
	enum { IDD = IDD_AddRule_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	__ALARM m_newAlarm;

protected:
	CString m_exePath;
	
	CString GetMainPath();
	void    OnInitControl();
	void    BrowseDir(CString strDir,CStringArray &arrFiles); 

public:
	afx_msg void OnBnClickedCheckSound();
public:
	afx_msg void OnBnClickedCheckSkin();
public:
	afx_msg void OnBnClickedCheckPoptext();
public:
	afx_msg void OnBnClickedCheckPlugin();
public:
	afx_msg void OnBnClickedOk();
public:
	afx_msg void OnBnClickedButtonToday();
public:
	CDateTimeCtrl m_Date;
public:
	CDateTimeCtrl m_Time;
};
