#pragma once


// CapConfigDlg 对话框

class CapConfigDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CapConfigDlg)

public:
	CapConfigDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CapConfigDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CONFIG_CAP_DLG };
#endif

private:
	virtual BOOL OnInitDialog();
	int m_VideoCapture;
	int m_VideoFormat;
	BOOL m_bCaptureSpeaker;
	BOOL m_bCaptureMic;

public:
	void InitConfig(CaptureConfig& capture);
	void GetConfig(CaptureConfig& capture);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
