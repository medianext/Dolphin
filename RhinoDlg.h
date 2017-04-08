
// RhinoDlg.h : 头文件
//

#pragma once


// CRhinoDlg 对话框

class CRhinoDlg : public CDialogEx
{
// 构造
public:
	CRhinoDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RHINO_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

// 实现
protected:
	HICON m_hIcon;
	CMenu m_NotifyMenu;
	NOTIFYICONDATA m_NotifyIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClickBtnConfig();
	afx_msg void OnClickBtnRecord();
	afx_msg void OnClickBtnStop();

	LRESULT OnNotifyMsg(WPARAM wparam, LPARAM lparam);
	afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);
};
