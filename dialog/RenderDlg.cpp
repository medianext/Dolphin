// dialog\RenderDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "../Rhino.h"
#include "afxdialogex.h"
#include "RenderDlg.h"


// RenderDlg 对话框

IMPLEMENT_DYNAMIC(RenderDlg, CDialogEx)

RenderDlg::RenderDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_RENDER_DIALOG, pParent)
	, m_bMoving(FALSE)
{

}

RenderDlg::~RenderDlg()
{
}

void RenderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(RenderDlg, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_COMMAND(WM_CLOSE, &RenderDlg::OnClose)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_MESSAGE(WM_SET_RENDER_SIZE, &RenderDlg::OnSetRenderSize)
END_MESSAGE_MAP()


// RenderDlg 消息处理程序


BOOL RenderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_ContextMenu.CreatePopupMenu();
	m_ContextMenu.AppendMenu(MF_STRING, WM_CLOSE, TEXT("隐藏"));

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void RenderDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	ShowWindow(SW_HIDE);

	CDialogEx::OnClose();
}


void RenderDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	m_ContextMenu.DestroyMenu();

	// TODO: 在此处添加消息处理程序代码
}


void RenderDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	LPPOINT lpoint = new tagPOINT;
	::GetCursorPos(lpoint);//得到鼠标位置

	m_ContextMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERNEGANIMATION, lpoint->x, lpoint->y, this);

	CDialogEx::OnRButtonUp(nFlags, point);
}


void RenderDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_bMoving = FALSE;

	CDialogEx::OnMButtonUp(nFlags, point);
}


void RenderDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_bMoving = TRUE;
	m_StartPos = point;

	CDialogEx::OnMButtonDown(nFlags, point);
}


void RenderDlg::OnMouseLeave()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_bMoving = FALSE;

	CDialogEx::OnMouseLeave();
}


void RenderDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_bMoving)
	{

		RECT rect;
		GetWindowRect(&rect);

		LONG xoffset = point.x - m_StartPos.x;
		LONG yoffset = point.y - m_StartPos.y;
		rect.left += xoffset;
		rect.right += xoffset;
		rect.top += yoffset;
		rect.bottom += yoffset;
		this->MoveWindow(&rect, 0);
	}

	CDialogEx::OnMouseMove(nFlags, point);
}


afx_msg LRESULT RenderDlg::OnSetRenderSize(WPARAM wParam, LPARAM lParam)
{
	RECT* prcSrc = (RECT*)lParam;
	RECT rcWin;
	GetWindowRect(&rcWin);
	rcWin = LetterBoxRect(*prcSrc, rcWin);
	//MoveWindow(rcWin.left, rcWin.top, rcWin.right - rcWin.left, rcWin.bottom - rcWin.top, FALSE);
	MoveWindow(&rcWin, FALSE);

	return NULL;
}