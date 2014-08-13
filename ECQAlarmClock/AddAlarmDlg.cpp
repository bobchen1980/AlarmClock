// ECQAlarmClock/AddAlarmDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ECQAlarmClock.h"
#include "AddAlarmDlg.h"


// CAddAlarmDlg dialog

IMPLEMENT_DYNAMIC(CAddAlarmDlg, CDialog)

CAddAlarmDlg::CAddAlarmDlg(CWnd* pParent /*=NULL*/)
: CDialog(CAddAlarmDlg::IDD, pParent)
{

}

CAddAlarmDlg::~CAddAlarmDlg()
{
}

void CAddAlarmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DATETIMEPICKER2, m_Date);
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_Time);
}


BEGIN_MESSAGE_MAP(CAddAlarmDlg, CDialog)
	ON_BN_CLICKED(IDC_CHECK_SOUND, &CAddAlarmDlg::OnBnClickedCheckSound)
	ON_BN_CLICKED(IDC_CHECK_SKIN, &CAddAlarmDlg::OnBnClickedCheckSkin)
	ON_BN_CLICKED(IDC_CHECK_POPTEXT, &CAddAlarmDlg::OnBnClickedCheckPoptext)
	ON_BN_CLICKED(IDC_CHECK_PLUGIN, &CAddAlarmDlg::OnBnClickedCheckPlugin)
	ON_BN_CLICKED(IDOK, &CAddAlarmDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_TODAY, &CAddAlarmDlg::OnBnClickedButtonToday)
END_MESSAGE_MAP()


// CAddAlarmDlg message handlers

BOOL CAddAlarmDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_exePath = GetMainPath();

	OnInitControl();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

CString CAddAlarmDlg::GetMainPath()
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

void CAddAlarmDlg::OnInitControl()
{

	CStringArray arrSoundfile;
	CStringArray arrSkins;
	CStringArray arrPlugins;

	//init sound	
	CComboBox *pcboSoundfile = (CComboBox *)GetDlgItem(IDC_COMBO_SOUND);
	BrowseDir(m_exePath+"sound",arrSoundfile);
	for(int i=0;i<arrSoundfile.GetSize();i++)
	{
		//AfxMessageBox(arrSoundfile[i]);
		pcboSoundfile->InsertString(i,"sound\\" +arrSoundfile[i]);
	}
// 	//init skin
// 	CComboBox *pcboSkin = (CComboBox *)GetDlgItem(IDC_COMBO_SKIN);
// 	BrowseDir(m_exePath+"skin",arrSkins);
// 	for(int i=0;i<arrSkins.GetSize();i++)
// 	{
// 		//AfxMessageBox(arrSoundfile[i]);
// 		pcboSkin->InsertString(i,"skin\\" +arrSkins[i]);
// 	}
	CComboBox *pcboSkin = (CComboBox *)GetDlgItem(IDC_COMBO_SKIN);
	pcboSkin->InsertString(0,_T("Blue"));
	pcboSkin->InsertString(1,_T("Red"));
	pcboSkin->InsertString(2,_T("Yellow"));	
	pcboSkin->InsertString(3,_T("Green"));	
	pcboSkin->SetCurSel(0);

	//init plugin
	CComboBox *pcboPlugin = (CComboBox *)GetDlgItem(IDC_COMBO_PLUGIN);
	BrowseDir(m_exePath+"plugin",arrPlugins);
	for(int i=0;i<arrPlugins.GetSize();i++)
	{
		//AfxMessageBox(arrSoundfile[i]);
		pcboPlugin->InsertString(i,"plugin\\" +arrPlugins[i]);
	}

	CEdit *pEditText = (CEdit *)GetDlgItem(IDC_EDIT_TEXT);
	pEditText->SetWindowText(_T("Two minutes left"));

	OnBnClickedCheckSound();
	OnBnClickedCheckSkin();
	OnBnClickedCheckPoptext();
	OnBnClickedCheckPlugin();
}

void CAddAlarmDlg::BrowseDir(CString strDir,CStringArray &arrFiles)        
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
void CAddAlarmDlg::OnBnClickedCheckSound()
{
	// TODO: Add your control notification handler code here
	CComboBox *pcbo = (CComboBox *)GetDlgItem(IDC_COMBO_SOUND);
	CButton * pbtn = (CButton *)GetDlgItem(IDC_CHECK_SOUND);
	pcbo->EnableWindow(pbtn->GetCheck());
}

void CAddAlarmDlg::OnBnClickedCheckSkin()
{
	// TODO: Add your control notification handler code here
	CComboBox *pcbo = (CComboBox *)GetDlgItem(IDC_COMBO_SKIN);
	CButton * pbtn = (CButton *)GetDlgItem(IDC_CHECK_SKIN);
	pcbo->EnableWindow(pbtn->GetCheck());
}

void CAddAlarmDlg::OnBnClickedCheckPoptext()
{
	// TODO: Add your control notification handler code here
	CEdit *pEdit = (CEdit *)GetDlgItem(IDC_EDIT_TEXT);
	CButton * pbtn = (CButton *)GetDlgItem(IDC_CHECK_POPTEXT);
	pEdit->EnableWindow(pbtn->GetCheck());
}

void CAddAlarmDlg::OnBnClickedCheckPlugin()
{
	// TODO: Add your control notification handler code here
	CComboBox *pcbo = (CComboBox *)GetDlgItem(IDC_COMBO_PLUGIN);
	CButton * pbtn = (CButton *)GetDlgItem(IDC_CHECK_PLUGIN);
	pcbo->EnableWindow(pbtn->GetCheck());
}

void CAddAlarmDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CString szSoundfile,szSkinfile,szPoptext,szPlugin;
	CTime timeDate,timeTime; 
	SYSTEMTIME st;

	CComboBox *pcbo1 = (CComboBox *)GetDlgItem(IDC_COMBO_SOUND);
	if( !((CButton *)GetDlgItem(IDC_CHECK_SOUND))->GetCheck() )
		szSoundfile = "";
	else
	{
		if(pcbo1->GetCurSel()>=0)	
			pcbo1->GetLBText(pcbo1->GetCurSel(),szSoundfile);
		else
			szSoundfile = "";
	}

	CComboBox *pcbo2 = (CComboBox *)GetDlgItem(IDC_COMBO_SKIN);
	if( !((CButton *)GetDlgItem(IDC_CHECK_SKIN))->GetCheck() )
		szSkinfile = "";
	else
	{
		if(pcbo2->GetCurSel()>=0)
			pcbo2->GetLBText(pcbo2->GetCurSel(),szSkinfile);
		else
			szSkinfile = "";
	}

	CEdit *pEdit3 = (CEdit *)GetDlgItem(IDC_EDIT_TEXT);
	if( !((CButton *)GetDlgItem(IDC_CHECK_POPTEXT))->GetCheck())
		szPoptext = "";
	else
		pEdit3->GetWindowText(szPoptext);

	CComboBox *pcbo4 = (CComboBox *)GetDlgItem(IDC_COMBO_PLUGIN);
	if( !((CButton *)GetDlgItem(IDC_CHECK_PLUGIN))->GetCheck())
		szPlugin = "";
	else
	{
		if(pcbo4->GetCurSel()>=0)
			pcbo4->GetLBText(pcbo4->GetCurSel(),szPlugin); 
		else
			szPlugin = "";
	}


	m_Date.GetTime(timeDate); 
	m_Time.GetTime(timeTime);
	st.wYear = timeDate.GetYear();
	st.wMonth = timeDate.GetMonth();
	st.wDay = timeDate.GetDay();
	st.wHour = timeTime.GetHour();
	st.wMinute =timeTime.GetMinute();
	st.wSecond = timeTime.GetSecond();
	st.wMilliseconds = 0;
	st.wDayOfWeek = timeDate.GetDayOfWeek();

	m_newAlarm.IsAlarmed = false;
	_tcscpy(m_newAlarm.AlarmSound , (TCHAR *)(LPCTSTR)szSoundfile);
	_tcscpy(m_newAlarm.AlarmSkin ,  (TCHAR *)(LPCTSTR)szSkinfile);
	_tcscpy(m_newAlarm.AlarmText , (TCHAR *)(LPCTSTR)szPoptext);
	_tcscpy(m_newAlarm.AlarmPlugin , (TCHAR *)(LPCTSTR)szPlugin);
	m_newAlarm.AlarmTime = st;

	OnOK();
}

void CAddAlarmDlg::OnBnClickedButtonToday()
{
	// TODO: Add your control notification handler code here
	CTime tm = CTime::GetCurrentTime();
	m_Date.SetTime(&tm);
	m_Time.SetTime(&tm);
}
