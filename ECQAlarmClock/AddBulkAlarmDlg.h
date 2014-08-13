#pragma once
#include "afxdtctl.h"


// CAddBulkAlarmDlg dialog
#define MAXRULE 4

class CAddBulkAlarmDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddBulkAlarmDlg)

public:
	CAddBulkAlarmDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CAddBulkAlarmDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_ADDBULK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
public:
	afx_msg void OnBnClickedCancel();
public:
	virtual BOOL OnInitDialog();
public:
	__ALARM m_AlarmRule[MAXRULE];
	_ALARMPARAM m_AlarmParam;

protected:
	CString m_exePath;
	void OnInitControl();
	CString GetMainPath();

	void  BrowseDir(CString strDir,CStringArray &arrFiles); 

public:
	afx_msg void OnBnClickedCheckSound();
public:
	afx_msg void OnBnClickedCheckSkin();
public:
	afx_msg void OnBnClickedCheckPoptext();
public:
	afx_msg void OnBnClickedCheckPlugin();
public:
	CDateTimeCtrl m_StartTimeCtrl;
public:
	CDateTimeCtrl m_EndTimeCtrl;
};
