#pragma once


// CPasswordDlg dialog

class CPasswordDlg : public CDialog
{
	DECLARE_DYNAMIC(CPasswordDlg)

public:
	CPasswordDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPasswordDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_PASS };
public:
	CString m_Password;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
