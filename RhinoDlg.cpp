
// RhinoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Rhino.h"
#include "dialog/ConfigDlg.h"
#include "RhinoDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_NOTYFICATION_MESSAGE (WM_USER+1000)

#define WM_STARTRECORD (WM_USER+1001)
#define WM_PAUSERECORD (WM_USER+1002)
#define WM_STOPRECORD (WM_USER+1003)

#define WM_SHOW_MAIN_DLG (WM_USER+1004)
#define WM_SHOW_CONFIG_DLG (WM_USER+1005)
#define WM_SHOW_ABOUT_DLG (WM_USER+1006)

LRESULT CALLBACK NotifyHookProc(int code, WPARAM wParam, LPARAM lParam);


// CRhinoDlg 对话框



CRhinoDlg::CRhinoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_RHINO_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRhinoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRhinoDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_CONFIG, &CRhinoDlg::OnClickBtnConfig)
	ON_BN_CLICKED(IDC_BTN_RECORD, &CRhinoDlg::OnClickBtnRecord)
	ON_BN_CLICKED(IDC_BTN_STOP, &CRhinoDlg::OnClickBtnStop)
	ON_MESSAGE(WM_NOTYFICATION_MESSAGE, OnNotifyMsg)
	ON_WM_HOTKEY()
END_MESSAGE_MAP()


// CRhinoDlg 消息处理程序

BOOL CRhinoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	BOOL ret;

	//Initialize NotifyIcon
	m_NotifyIcon.cbSize = sizeof(NOTIFYICONDATA);
	m_NotifyIcon.hWnd = this->m_hWnd;
	m_NotifyIcon.uID = IDR_MAINFRAME;
	m_NotifyIcon.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	m_NotifyIcon.uCallbackMessage = WM_USER + 1000;
	m_NotifyIcon.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	lstrcpy(m_NotifyIcon.szTip, TEXT("Rhino Screen Recorder"));//图标提示为"测试程序"
	Shell_NotifyIcon(NIM_ADD, &m_NotifyIcon);//向任务栏添加图标

	//Initialize NotifyMenu
	m_NotifyMenu.CreatePopupMenu();
	m_NotifyMenu.AppendMenu(MF_STRING, WM_SHOW_MAIN_DLG, TEXT("主界面"));
	m_NotifyMenu.AppendMenu(MF_SEPARATOR);
	m_NotifyMenu.AppendMenu(MF_STRING, WM_STARTRECORD, TEXT("开始录像"));
	m_NotifyMenu.AppendMenu(MF_STRING, WM_STOPRECORD, TEXT("停止录像"));
	m_NotifyMenu.AppendMenu(MF_SEPARATOR);
	m_NotifyMenu.AppendMenu(MF_STRING, WM_SHOW_CONFIG_DLG, TEXT("设置"));
	m_NotifyMenu.AppendMenu(MF_STRING, WM_SHOW_ABOUT_DLG, TEXT("关于"));
	m_NotifyMenu.AppendMenu(MF_SEPARATOR);
	m_NotifyMenu.AppendMenu(MF_STRING, WM_DESTROY, TEXT("退出"));

	//Register HotKey
	ret = RegisterHotKey(GetSafeHwnd(), WM_STARTRECORD, MOD_ALT | MOD_NOREPEAT, VK_F9);
	ret = RegisterHotKey(GetSafeHwnd(), WM_PAUSERECORD, MOD_ALT | MOD_NOREPEAT, VK_F10);
	ret = RegisterHotKey(GetSafeHwnd(), WM_STOPRECORD, MOD_ALT | MOD_NOREPEAT, VK_F11);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CRhinoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CRhinoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CRhinoDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//this->ShowWindow(SW_MINIMIZE);
	CDialogEx::OnClose();
}


void CRhinoDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	Shell_NotifyIcon(NIM_DELETE, &m_NotifyIcon);//删除任务栏图标
}


void CRhinoDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnTimer(nIDEvent);
}


void CRhinoDlg::OnClickBtnConfig()
{
	// TODO: 在此添加控件通知处理程序代码
	ConfigDlg m_configDlg;
	INT_PTR ret = m_configDlg.DoModal();
}


void CRhinoDlg::OnClickBtnRecord()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CRhinoDlg::OnClickBtnStop()
{
	// TODO: 在此添加控件通知处理程序代码
}


LRESULT  CRhinoDlg::OnNotifyMsg(WPARAM wparam, LPARAM lparam)
//wParam接收的是图标的ID，而lParam接收的是鼠标的行为     
{
	if (wparam != IDR_MAINFRAME)
		return    1;
	switch (lparam)
	{
	case WM_RBUTTONUP://右键起来时弹出快捷菜单，这里只有一个“关闭”     
	{
		LPPOINT lpoint = new tagPOINT;
		::GetCursorPos(lpoint);//得到鼠标位置  
		//this->SetForegroundWindow();

		HHOOK hook = SetWindowsHookEx(WH_MSGFILTER, NotifyHookProc, NULL, GetCurrentThreadId());
		m_NotifyMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERNEGANIMATION, lpoint->x, lpoint->y, this);
		UnhookWindowsHookEx(hook);

		delete    lpoint;
	}
	break;
	case WM_LBUTTONDBLCLK://双击左键的处理
	{
		this->ShowWindow(SW_SHOW);//简单的显示主窗口完事儿
	}
	break;
	case WM_KILLFOCUS:
	{
		OutputDebugString(TEXT("Fffffffffffffffffffffff\n"));
	}
	break;
	}
	return 0;
}

void CRhinoDlg::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	switch (nHotKeyId)
	{
	case WM_STARTRECORD:
		break;
	case WM_PAUSERECORD:
		break;
	case WM_STOPRECORD:
		break;
	default:
		break;
	}

	CDialogEx::OnHotKey(nHotKeyId, nKey1, nKey2);
}

LRESULT CALLBACK NotifyHookProc(int code, WPARAM wParam, LPARAM lParam)
{

	if (code==MSGF_MENU)
	{
		MSG* pMsg = (MSG*)lParam;
		if (pMsg->message == WM_MOUSEMOVE)
		{
			// 计算坐标，当鼠标到了某个位置需要销毁菜单时，给拥有菜单的窗口发送WM_CANCELMODE消息。
		}
	}
	return CallNextHookEx(NULL, code, wParam, lParam);
}