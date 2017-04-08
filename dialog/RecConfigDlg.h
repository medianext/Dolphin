#pragma once


// RecConfigDlg 对话框

class RecConfigDlg : public CDialogEx
{
	DECLARE_DYNAMIC(RecConfigDlg)

public:
	RecConfigDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~RecConfigDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CONFIG_REC_DLG };
#endif

private:
	int m_RecordFmt;
	int m_RecordDiv;
	CString m_RecordPath;
	CString m_RecordName;

public:
	void InitConfig(RecordConfig& rec);
	void GetConfig(RecordConfig& rec);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
