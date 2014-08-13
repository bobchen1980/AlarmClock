// ECQAlarmClock/PasswordDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ECQAlarmClock.h"
#include "PasswordDlg.h"
#include "PasswordDlg.h"


// CPasswordDlg dialog

IMPLEMENT_DYNAMIC(CPasswordDlg, CDialog)

CPasswordDlg::CPasswordDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPasswordDlg::IDD, pParent)
{

}

CPasswordDlg::~CPasswordDlg()
{
}

void CPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPasswordDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CPasswordDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CPasswordDlg message handlers

void CPasswordDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT1);
	pEdit->GetWindowText(m_Password);

	OnOK();
}
