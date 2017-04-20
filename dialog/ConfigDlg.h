
// DolphinDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"

#include "../Config.h"
#include "EncConfigDlg.h"
#include "CapConfigDlg.h"
#include "RecConfigDlg.h"
#include "SysConfigDlg.h"
#include "AboutDlg.h"

// ConfigDlg 对话框

class ConfigDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ConfigDlg)

public:
	ConfigDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~ConfigDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CONFIG_DIALOG };
#endif

private:
	CTabCtrl m_mainTab;
	EncConfigDlg m_encConfigDlg;
	CapConfigDlg m_capConfigDlg;
	RecConfigDlg m_recConfigDlg;
    SysConfigDlg m_sysConfigDlg;
    AboutDlg m_aboutDlg;

    Config* m_pConfig = nullptr;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()

    void SaveConfig();
    void LoadConfig();

public:
    void SetConfig(Config* config);
    void ShowAboutTab();

	afx_msg void OnSave();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnTabChange(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
