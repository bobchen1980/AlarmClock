// AddBulkAlarmDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ECQAlarmClock.h"
#include "AddBulkAlarmDlg.h"


// CAddBulkAlarmDlg dialog

IMPLEMENT_DYNAMIC(CAddBulkAlarmDlg, CDialog)

CAddBulkAlarmDlg::CAddBulkAlarmDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddBulkAlarmDlg::IDD, pParent)
{

}

CAddBulkAlarmDlg::~CAddBulkAlarmDlg()
{
}

void CAddBulkAlarmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_StartTimeCtrl);
	DDX_Control(pDX, IDC_DATETIMEPICKER2, m_EndTimeCtrl);
}


BEGIN_MESSAGE_MAP(CAddBulkAlarmDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CAddBulkAlarmDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CAddBulkAlarmDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CHECK_SOUND, &CAddBulkAlarmDlg::OnBnClickedCheckSound)
	ON_BN_CLICKED(IDC_CHECK_SKIN, &CAddBulkAlarmDlg::OnBnClickedCheckSkin)
	ON_BN_CLICKED(IDC_CHECK_POPTEXT, &CAddBulkAlarmDlg::OnBnClickedCheckPoptext)
	ON_BN_CLICKED(IDC_CHECK_PLUGIN, &CAddBulkAlarmDlg::OnBnClickedCheckPlugin)
	ON_BN_CLICKED(IDC_CHECK_SOUND1, &CAddBulkAlarmDlg::OnBnClickedCheckSound)
	ON_BN_CLICKED(IDC_CHECK_SKIN1, &CAddBulkAlarmDlg::OnBnClickedCheckSkin)
	ON_BN_CLICKED(IDC_CHECK_POPTEXT1, &CAddBulkAlarmDlg::OnBnClickedCheckPoptext)
	ON_BN_CLICKED(IDC_CHECK_PLUGIN1, &CAddBulkAlarmDlg::OnBnClickedCheckPlugin)
	ON_BN_CLICKED(IDC_CHECK_SOUND2, &CAddBulkAlarmDlg::OnBnClickedCheckSound)
	ON_BN_CLICKED(IDC_CHECK_SKIN2, &CAddBulkAlarmDlg::OnBnClickedCheckSkin)
	ON_BN_CLICKED(IDC_CHECK_POPTEXT2, &CAddBulkAlarmDlg::OnBnClickedCheckPoptext)
	ON_BN_CLICKED(IDC_CHECK_PLUGIN2, &CAddBulkAlarmDlg::OnBnClickedCheckPlugin)
	ON_BN_CLICKED(IDC_CHECK_SOUND3, &CAddBulkAlarmDlg::OnBnClickedCheckSound)
	ON_BN_CLICKED(IDC_CHECK_SKIN3, &CAddBulkAlarmDlg::OnBnClickedCheckSkin)
	ON_BN_CLICKED(IDC_CHECK_POPTEXT3, &CAddBulkAlarmDlg::OnBnClickedCheckPoptext)
	ON_BN_CLICKED(IDC_CHECK_PLUGIN3, &CAddBulkAlarmDlg::OnBnClickedCheckPlugin)
END_MESSAGE_MAP()


// CAddBulkAlarmDlg message handlers

void CAddBulkAlarmDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	SYSTEMTIME st;
	CEdit *pEdit;
	CString szTmp;

	m_StartTimeCtrl.GetTime(m_AlarmParam.ctStart);
	m_EndTimeCtrl.GetTime(m_AlarmParam.ctEnd);
	
	pEdit= (CEdit *)GetDlgItem(IDC_EDIT1);
	pEdit->GetWindowText(szTmp);
	m_AlarmParam.nLoopSencond = _wtoi(szTmp);

	pEdit= (CEdit *)GetDlgItem(IDC_EDIT2);
	pEdit->GetWindowText(szTmp);
	m_AlarmParam.nDelay1 = _wtoi(szTmp);

	pEdit= (CEdit *)GetDlgItem(IDC_EDIT3);
	pEdit->GetWindowText(szTmp);
	m_AlarmParam.nDelay2 = _wtoi(szTmp);

	pEdit= (CEdit *)GetDlgItem(IDC_EDIT4);
	pEdit->GetWindowText(szTmp);
	m_AlarmParam.nDelay3 = _wtoi(szTmp);

	pEdit= (CEdit *)GetDlgItem(IDC_EDIT5);
	pEdit->GetWindowText(szTmp);
	m_AlarmParam.nDelay4 = _wtoi(szTmp);

	for(int k=0;k<MAXRULE;k++)
	{

		CString szSoundfile="",szSkinfile="",szPoptext="",szPlugin="";

		CComboBox *pcbo1 = (CComboBox *)GetDlgItem(IDC_COMBO_SOUND+k);
		if( ((CButton *)GetDlgItem(IDC_CHECK_SOUND+k))->GetCheck() )
		{
			if(pcbo1->GetCurSel()>=0)	
				pcbo1->GetLBText(pcbo1->GetCurSel(),szSoundfile);
			else
				szSoundfile = "";
		}

		CComboBox *pcbo2 = (CComboBox *)GetDlgItem(IDC_COMBO_SKIN+k);
		if( ((CButton *)GetDlgItem(IDC_CHECK_SKIN+k))->GetCheck() )
		{
			if(pcbo2->GetCurSel()>=0)
				pcbo2->GetLBText(pcbo2->GetCurSel(),szSkinfile);
			else
				szSkinfile = "";
		}

		CEdit *pEdit3 = (CEdit *)GetDlgItem(IDC_EDIT_TEXT+k);
		if( ((CButton *)GetDlgItem(IDC_CHECK_POPTEXT+k))->GetCheck())
			pEdit3->GetWindowText(szPoptext);

		CComboBox *pcbo4 = (CComboBox *)GetDlgItem(IDC_COMBO_PLUGIN+k);
		if( ((CButton *)GetDlgItem(IDC_CHECK_PLUGIN+k))->GetCheck())
		{
			if(pcbo4->GetCurSel()>=0)
				pcbo4->GetLBText(pcbo4->GetCurSel(),szPlugin); 
			else
				szPlugin = "";
		}

		m_AlarmRule[k].IsAlarmed = false;
		_tcscpy(m_AlarmRule[k].AlarmSound , (TCHAR *)(LPCTSTR)szSoundfile);
		_tcscpy(m_AlarmRule[k].AlarmSkin ,  (TCHAR *)(LPCTSTR)szSkinfile);
		_tcscpy(m_AlarmRule[k].AlarmText , (TCHAR *)(LPCTSTR)szPoptext);
		_tcscpy(m_AlarmRule[k].AlarmPlugin , (TCHAR *)(LPCTSTR)szPlugin);

	}

	OnOK();
}

void CAddBulkAlarmDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

BOOL CAddBulkAlarmDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_exePath = GetMainPath();

	OnInitControl();

	CTime tm; 
	tm=CTime::GetCurrentTime(); 
	m_StartTimeCtrl.SetTime(&tm); 
	m_StartTimeCtrl.SetFormat(_T("yyyy-MM-dd HH:mm:ss")); 

	m_EndTimeCtrl.SetTime(&tm); 
	m_EndTimeCtrl.SetFormat(_T("yyyy-MM-dd HH:mm:ss")); 

	CEdit *pEdit1 = (CEdit *)GetDlgItem(IDC_EDIT1);
	CEdit *pEdit2 = (CEdit *)GetDlgItem(IDC_EDIT2);
	CEdit *pEdit3 = (CEdit *)GetDlgItem(IDC_EDIT3);
	CEdit *pEdit4 = (CEdit *)GetDlgItem(IDC_EDIT4);
	CEdit *pEdit5 = (CEdit *)GetDlgItem(IDC_EDIT5);

	pEdit1->SetWindowText(_T("1200"));
	pEdit2->SetWindowText(_T("0"));
	pEdit3->SetWindowText(_T("780"));
	pEdit4->SetWindowText(_T("900"));
	pEdit5->SetWindowText(_T("1080"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAddBulkAlarmDlg::OnInitControl()
{

	CStringArray arrSoundfile;
	CStringArray arrSkins;
	CStringArray arrPlugins;
	CComboBox *pcboSoundfile,*pcboSkin,*pcboPlugin;
	CEdit *pEditText;

	BrowseDir(m_exePath+"sound",arrSoundfile);
	BrowseDir(m_exePath+"plugin",arrPlugins);

	for(int k=0;k<MAXRULE;k++)
	{
		//init sound
		pcboSoundfile = (CComboBox *)GetDlgItem(IDC_COMBO_SOUND+k);
		for(int i=0;i<arrSoundfile.GetSize();i++)
		{
			//AfxMessageBox(arrSoundfile[i]);
			pcboSoundfile->InsertString(i,"sound\\" +arrSoundfile[i]);
		}
		//init skin
		pcboSkin = (CComboBox *)GetDlgItem(IDC_COMBO_SKIN+k);
		pcboSkin->InsertString(0,_T("Blue"));
		pcboSkin->InsertString(1,_T("Red"));
		pcboSkin->InsertString(2,_T("Yellow"));	
		pcboSkin->InsertString(3,_T("Green"));	
		pcboSkin->SetCurSel(0);


		//init plugin
		pcboPlugin = (CComboBox *)GetDlgItem(IDC_COMBO_PLUGIN+k);
		for(int i=0;i<arrPlugins.GetSize();i++)
		{
			//AfxMessageBox(arrSoundfile[i]);
			pcboPlugin->InsertString(i,"plugin\\" +arrPlugins[i]);
		}

		//init poptext
		pEditText = (CEdit *)GetDlgItem(IDC_EDIT_TEXT+k);
		pEditText->SetWindowText(_T("Two minutes left"));
	}

 	OnBnClickedCheckSound();
 	OnBnClickedCheckSkin();
 	OnBnClickedCheckPoptext();
 	OnBnClickedCheckPlugin();
}


CString CAddBulkAlarmDlg::GetMainPath()
{
	TCHAR cPath[MAX_PATH];
	CString rtnSting;

	GetModuleFileName( NULL, cPath, MAX_PATH ); //获取程序路径（包括程序名）
	int i = 0, j;
	while( cPath[i]!=0 )
	{
		if( cPath[i]=='\\' )
			j = i;
		i++;
	}
	cPath[j+1] = '\0';

	rtnSting.Format(_T("%s"),cPath);

	return rtnSting;   //分离路径名（去掉程序名）
}

void CAddBulkAlarmDlg::BrowseDir(CString strDir,CStringArray &arrFiles)        
{   
	if(strDir   ==   "")   
	{   
		return;   
	}   
	CFileFind   ff;   
	CString   szDir   =   strDir,strPath;   
	if(szDir.Right(1)   !=   "\\")       //保证目录是以\结尾的   
		szDir   +=   "\\";   
	szDir   +=   "*.*";   
	BOOL   res   =   ff.FindFile(szDir);   
	while(   res   )   
	{   
		res   =   ff.FindNextFile();   
		strPath   =   ff.GetFilePath();   
		if(ff.IsDirectory()   &&   !ff.IsDots())   
			BrowseDir(strPath,arrFiles);       //如果目标是个文件夹，则利用嵌套来遍历   
		else if(!ff.IsDirectory()   &&   !ff.IsDots())   //如果目标是个文件，则对它进行处理  
		{
			arrFiles.Add(ff.GetFileName());	
		}        
	}   
	ff.Close();   
}
void CAddBulkAlarmDlg::OnBnClickedCheckSound()
{
	// TODO: Add your control notification handler code here
	CButton * pbtn;
	CComboBox *pcbo;

	for(int k=0;k<MAXRULE;k++)
	{
		pcbo = (CComboBox *)GetDlgItem(IDC_COMBO_SOUND+k);
		pbtn = (CButton *)GetDlgItem(IDC_CHECK_SOUND+k);
		pcbo->EnableWindow(pbtn->GetCheck());
	}

}

void CAddBulkAlarmDlg::OnBnClickedCheckSkin()
{
	// TODO: Add your control notification handler code here
	CButton * pbtn;
	CComboBox *pcbo;

	for(int k=0;k<MAXRULE;k++)
	{
		pcbo = (CComboBox *)GetDlgItem(IDC_COMBO_SKIN+k);
		pbtn = (CButton *)GetDlgItem(IDC_CHECK_SKIN+k);
		pcbo->EnableWindow(pbtn->GetCheck());
	}
}

void CAddBulkAlarmDlg::OnBnClickedCheckPoptext()
{
	// TODO: Add your control notification handler code here
	CButton * pbtn;
	CEdit *pEdit ;

	for(int k=0;k<MAXRULE;k++)
	{
		pEdit = (CEdit *)GetDlgItem(IDC_EDIT_TEXT+k);
		pbtn = (CButton *)GetDlgItem(IDC_CHECK_POPTEXT+k);
		pEdit->EnableWindow(pbtn->GetCheck());
	}
}

void CAddBulkAlarmDlg::OnBnClickedCheckPlugin()
{
	// TODO: Add your control notification handler code here
	for(int k=0;k<MAXRULE;k++)
	{
		CComboBox *pcbo = (CComboBox *)GetDlgItem(IDC_COMBO_PLUGIN+k);
		CButton * pbtn = (CButton *)GetDlgItem(IDC_CHECK_PLUGIN+k);
		pcbo->EnableWindow(pbtn->GetCheck());
	}
}