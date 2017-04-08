#pragma once


// SysConfigDlg 对话框

class SysConfigDlg : public CDialogEx
{
	DECLARE_DYNAMIC(SysConfigDlg)

public:
	SysConfigDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~SysConfigDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CONFIG_SYS_DLG };
#endif

public:
	void InitConfig(SoftwareConfig& soft);
	void GetConfig(SoftwareConfig& soft);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	BOOL m_bCloseCmd;
	BOOL m_bMinimizeCmd;
};
