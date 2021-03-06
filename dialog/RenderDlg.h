#pragma once


// RenderDlg 对话框

class RenderDlg : public CDialogEx
{
	DECLARE_DYNAMIC(RenderDlg)

public:
	RenderDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~RenderDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RENDER_DIALOG };
#endif

private:
    BOOL m_bTopMost = TRUE;
	BOOL m_bMoving = FALSE;
	CPoint m_StartPos;

	CMenu m_ContextMenu;

    Capture* m_capture = nullptr;

public:
    void SetVideoSource(Capture* capture);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnMouseLeave();
    afx_msg void OnTopWindow();
	afx_msg LRESULT OnSetRenderSize(WPARAM wParam, LPARAM lParam);
};
