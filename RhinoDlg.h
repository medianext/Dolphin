
// RhinoDlg.h : 头文件
//

#pragma once

#include "dialog/ConfigDlg.h"
#include "dialog/RenderDlg.h"
#include "dialog/AboutDlg.h"

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

private:
	HICON m_hIcon;
	CMenu m_NotifyMenu;
	NOTIFYICONDATA m_NotifyIcon;

	ConfigDlg m_configDlg;
	RenderDlg m_renderDlg;
	AboutDlg m_aboutDlg;

    DWORD m_dwStartTime;

	void StartRecord();
	void PauseRecord();
	void StopRecord();


// 实现
protected:

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
protected:
	void OnShowMainDlg();
	void OnShowConfigDlg();
	void OnShowAboutDlg();
	void OnShowCameraDlg();
	void OnStartRecord();
	void OnPauseRecord();
	void OnStopRecord();
};
