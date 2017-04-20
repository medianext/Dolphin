// F:\Workspace\MediaStudio\Dolphin\dialog\SysConfigDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "../Config.h"
#include "../Dolphin.h"
#include "SysConfigDlg.h"
#include "afxdialogex.h"


// SysConfigDlg 对话框

IMPLEMENT_DYNAMIC(SysConfigDlg, CDialogEx)

SysConfigDlg::SysConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CONFIG_SYS_DLG, pParent)
	, m_bCloseCmd(FALSE)
	, m_bMinimizeCmd(FALSE)
{

}

SysConfigDlg::~SysConfigDlg()
{
}

void SysConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_CLOSE_EXIT, m_bCloseCmd);
	DDX_Radio(pDX, IDC_MIN_MIN, m_bMinimizeCmd);
}


BEGIN_MESSAGE_MAP(SysConfigDlg, CDialogEx)
END_MESSAGE_MAP()


// SysConfigDlg 消息处理程序


BOOL SysConfigDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	CEdit* hEdit = NULL;
	hEdit = (CEdit*)GetDlgItem(IDC_EDIT1);
	hEdit->SetWindowText(TEXT("Alt+F1"));
	hEdit = (CEdit*)GetDlgItem(IDC_EDIT2);
	hEdit->SetWindowText(TEXT("Alt+F2"));
	hEdit = (CEdit*)GetDlgItem(IDC_EDIT3);
	hEdit->SetWindowText(TEXT("Alt+F3"));
	hEdit = (CEdit*)GetDlgItem(IDC_EDIT4);
	hEdit->SetWindowText(TEXT("Alt+F5"));
	hEdit = (CEdit*)GetDlgItem(IDC_EDIT5);
	hEdit->SetWindowText(TEXT("Alt+F6"));
	hEdit = (CEdit*)GetDlgItem(IDC_EDIT6);
	hEdit->SetWindowText(TEXT("Alt+F9"));
	hEdit = (CEdit*)GetDlgItem(IDC_EDIT7);
	hEdit->SetWindowText(TEXT("Alt+F10"));
	hEdit = (CEdit*)GetDlgItem(IDC_EDIT8);
	hEdit->SetWindowText(TEXT("Alt+F11"));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}

void SysConfigDlg::InitConfig(SoftwareConfig& soft)
{
    if (soft.bExitBtn)
    {
        m_bCloseCmd = 0;
    } 
    else
    {
        m_bCloseCmd = 1;
    }

    if (soft.bMinBtn)
    {
        m_bMinimizeCmd = 0;
    }
    else
    {
        m_bMinimizeCmd = 1;
    }

	this->UpdateData(FALSE);
}


void SysConfigDlg::GetConfig(SoftwareConfig& soft) 
{
	this->UpdateData(TRUE);

    switch (m_bCloseCmd)
    {
    case 0: soft.bExitBtn = true; break;
    case 1: soft.bExitBtn = false; break;
    }

    switch (m_bMinimizeCmd)
    {
    case 0: soft.bMinBtn = true; break;
    case 1: soft.bMinBtn = false; break;
    }

}