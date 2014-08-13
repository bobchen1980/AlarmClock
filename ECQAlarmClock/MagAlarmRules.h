#pragma once
#include "afxcmn.h"


// CMagAlarmRules dialog

class CMagAlarmRules : public CDialog
{
	DECLARE_DYNAMIC(CMagAlarmRules)

public:
	CMagAlarmRules(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMagAlarmRules();

// Dialog Data
	enum { IDD = IDD_MagAlarmRules_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
public:
	afx_msg void OnBnClickedCancel();
public:
	afx_msg void OnBnClickedAddAlarm();
public:
	afx_msg void OnBnClickedDeleteAlarm();
public:
	CListCtrl *m_pListCtrl;
	_ALARM  *m_initAlarms;
	void UploadList(_ALARM *upAlarms);
	void SortAlarm(_ALARM *alarms);
	void EmptyAlarms(_ALARM *upAlarms);
	void ReadAlarms(_ALARM *upAlarms);
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnBnClickedDeleteAll();
public:
	afx_msg void OnBnClickedAddBulk();
};
