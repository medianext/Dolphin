// ConfigDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "../Dolphin.h"
#include "ConfigDlg.h"
#include "afxdialogex.h"


// ConfigDlg 对话框

IMPLEMENT_DYNAMIC(ConfigDlg, CDialogEx)

ConfigDlg::ConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CONFIG_DIALOG, pParent)
{

}

ConfigDlg::~ConfigDlg()
{
}

void ConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SET_TAB, m_mainTab);
}


BEGIN_MESSAGE_MAP(ConfigDlg, CDialogEx)
	ON_BN_CLICKED(IDC_SAVE, &ConfigDlg::OnSave)
	ON_NOTIFY(TCN_SELCHANGE, IDC_SET_TAB, &ConfigDlg::OnTabChange)
    ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// ConfigDlg 消息处理程序


BOOL ConfigDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	LoadConfig();

	CTabCtrl* hTab = (CTabCtrl*)GetDlgItem(IDC_SET_TAB);
	hTab->InsertItem(0, TEXT("编码设置"));
	hTab->InsertItem(1, TEXT("录像设置"));
	hTab->InsertItem(2, TEXT("采集设置"));
	hTab->InsertItem(3, TEXT("系统设置"));
	hTab->InsertItem(4, TEXT("关于"));

	CRect rect;
	hTab->GetClientRect(&rect);
 	rect.top += 30;
	rect.bottom += 5;
	rect.left += 12;
	rect.right += 12;

	m_encConfigDlg.Create(IDD_CONFIG_ENC_DLG, this);
	m_encConfigDlg.MoveWindow(&rect);
    m_recConfigDlg.Create(IDD_CONFIG_REC_DLG, this);
	m_recConfigDlg.MoveWindow(&rect);
    m_capConfigDlg.Create(IDD_CONFIG_CAP_DLG, this);
	m_capConfigDlg.MoveWindow(&rect);
    m_sysConfigDlg.Create(IDD_CONFIG_SYS_DLG, this);
    m_sysConfigDlg.MoveWindow(&rect);
    m_aboutDlg.Create(IDD_ABOUT_DIALOG, this);
    m_aboutDlg.MoveWindow(&rect);

	m_encConfigDlg.ShowWindow(SW_SHOW);

	CenterWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void ConfigDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	SaveConfig();

	CDialogEx::OnOK();
}


void ConfigDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类

	CDialogEx::OnCancel();
}


void ConfigDlg::OnSave()
{
	// TODO: 在此添加控件通知处理程序代码
	SaveConfig();
}


void ConfigDlg::SaveConfig()
{
    CodecConfig codecConfig = { 0 };
    m_pConfig->GetCodecConfig(codecConfig);
    m_encConfigDlg.GetConfig(codecConfig);
    m_pConfig->SetCodecConfig(codecConfig);
    RecordConfig recConfig = { 0 };
    m_pConfig->GetRecordConfig(recConfig);
    m_recConfigDlg.GetConfig(recConfig);
    m_pConfig->SetRecordConfig(recConfig);
    CaptureConfig capConfig = { 0 };
    m_pConfig->GetCaptureConfig(capConfig);
    m_capConfigDlg.GetConfig(capConfig);
    m_pConfig->SetCaptureConfig(capConfig);
    SoftwareConfig softConfig = { 0 };
    m_pConfig->GetSoftwareConfig(softConfig);
    m_sysConfigDlg.GetConfig(softConfig);
    m_pConfig->SetSoftwareConfig(softConfig);
    m_pConfig->Save();
}


void ConfigDlg::LoadConfig()
{
    if (m_pConfig)
    {
        CodecConfig codecConfig = { 0 };
        m_pConfig->GetCodecConfig(codecConfig);
        m_encConfigDlg.InitConfig(codecConfig);
        RecordConfig recConfig = { 0 };
        m_pConfig->GetRecordConfig(recConfig);
        m_recConfigDlg.InitConfig(recConfig);
        CaptureConfig capConfig = { 0 };
        m_pConfig->GetCaptureConfig(capConfig);
        m_capConfigDlg.InitConfig(capConfig);
        SoftwareConfig softConfig = { 0 };
        m_pConfig->GetSoftwareConfig(softConfig);
        m_sysConfigDlg.InitConfig(softConfig);
    }
}


void ConfigDlg::SetConfig(Config* config)
{
    m_pConfig = config;
}


void ConfigDlg::ShowAboutTab()
{
    BOOL visibale = this->IsWindowVisible();
    if (visibale == FALSE)
    {
        this->ShowWindow(SW_SHOW);
    }
    if (m_mainTab.GetCurSel() != 4)
    {
        m_mainTab.SetCurSel(4);
        NMHDR param;
        param.hwndFrom = m_mainTab.GetSafeHwnd();
        param.idFrom = IDC_SET_TAB;
        param.code = TCN_SELCHANGE;
        SendMessage(WM_NOTIFY, IDC_SET_TAB, (LPARAM)&param);
    }
}


void ConfigDlg::OnTabChange(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	CTabCtrl* hTab = (CTabCtrl*)GetDlgItem(IDC_SET_TAB);
	int sel = hTab->GetCurSel();
	switch (sel)
	{
	case 0:
		m_encConfigDlg.ShowWindow(SW_SHOW);
		m_recConfigDlg.ShowWindow(SW_HIDE);
		m_capConfigDlg.ShowWindow(SW_HIDE);
        m_sysConfigDlg.ShowWindow(SW_HIDE);
        m_aboutDlg.ShowWindow(SW_HIDE);
		break;
	case 1:
		m_encConfigDlg.ShowWindow(SW_HIDE);
		m_recConfigDlg.ShowWindow(SW_SHOW);
		m_capConfigDlg.ShowWindow(SW_HIDE);
        m_sysConfigDlg.ShowWindow(SW_HIDE);
        m_aboutDlg.ShowWindow(SW_HIDE);
		break;
	case 2:
		m_encConfigDlg.ShowWindow(SW_HIDE);
		m_recConfigDlg.ShowWindow(SW_HIDE);
		m_capConfigDlg.ShowWindow(SW_SHOW);
        m_sysConfigDlg.ShowWindow(SW_HIDE);
        m_aboutDlg.ShowWindow(SW_HIDE);
		break;
	case 3:
		m_encConfigDlg.ShowWindow(SW_HIDE);
		m_recConfigDlg.ShowWindow(SW_HIDE);
		m_capConfigDlg.ShowWindow(SW_HIDE);
        m_sysConfigDlg.ShowWindow(SW_SHOW);
        m_aboutDlg.ShowWindow(SW_HIDE);
		break;
	case 4:
		m_encConfigDlg.ShowWindow(SW_HIDE);
		m_recConfigDlg.ShowWindow(SW_HIDE);
		m_capConfigDlg.ShowWindow(SW_HIDE);
		m_sysConfigDlg.ShowWindow(SW_HIDE);
        m_aboutDlg.ShowWindow(SW_SHOW);
		break;
	}
}


void ConfigDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
    CDialogEx::OnShowWindow(bShow, nStatus);

    // TODO: 在此处添加消息处理程序代码
    LoadConfig();
}
