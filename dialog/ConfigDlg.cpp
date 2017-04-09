// ConfigDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "../Rhino.h"
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
	m_encConfigDlg.InitConfig(m_codecConfig);
	m_encConfigDlg.MoveWindow(&rect);
	m_recConfigDlg.Create(IDD_CONFIG_REC_DLG, this);
	m_recConfigDlg.InitConfig(m_recordConfig);
	m_recConfigDlg.MoveWindow(&rect);
	m_capConfigDlg.Create(IDD_CONFIG_CAP_DLG, this);
	m_capConfigDlg.InitConfig(m_captureConfig);
	m_capConfigDlg.MoveWindow(&rect);
	m_sysConfigDlg.Create(IDD_CONFIG_SYS_DLG, this);
	m_sysConfigDlg.InitConfig(m_softConfig);
	m_sysConfigDlg.MoveWindow(&rect);

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
	m_encConfigDlg.GetConfig(m_codecConfig);
	m_recConfigDlg.GetConfig(m_recordConfig);
	m_capConfigDlg.GetConfig(m_captureConfig);
	m_sysConfigDlg.GetConfig(m_softConfig);
}


void ConfigDlg::LoadConfig()
{

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
		break;
	case 1:
		m_encConfigDlg.ShowWindow(SW_HIDE);
		m_recConfigDlg.ShowWindow(SW_SHOW);
		m_capConfigDlg.ShowWindow(SW_HIDE);
		m_sysConfigDlg.ShowWindow(SW_HIDE);
		break;
	case 2:
		m_encConfigDlg.ShowWindow(SW_HIDE);
		m_recConfigDlg.ShowWindow(SW_HIDE);
		m_capConfigDlg.ShowWindow(SW_SHOW);
		m_sysConfigDlg.ShowWindow(SW_HIDE);
		break;
	case 3:
		m_encConfigDlg.ShowWindow(SW_HIDE);
		m_recConfigDlg.ShowWindow(SW_HIDE);
		m_capConfigDlg.ShowWindow(SW_HIDE);
		m_sysConfigDlg.ShowWindow(SW_SHOW);
		break;
	case 4:
		m_encConfigDlg.ShowWindow(SW_HIDE);
		m_recConfigDlg.ShowWindow(SW_HIDE);
		m_capConfigDlg.ShowWindow(SW_HIDE);
		m_sysConfigDlg.ShowWindow(SW_HIDE);
		break;
	}
}
