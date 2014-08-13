/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

#ifndef ABOUT_H
#define ABOUT_H

#include "xhyperlink.h"
#include "resource.h"

#define ABOUT_BASE_CLASS CDialog

//=============================================================================
class CAboutDlg : public CDialog
//=============================================================================
{
public:
	CAboutDlg();
	~CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CXHyperLink m_ctrlWeb;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CBrush m_brush;

	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif //ABOUT_H
