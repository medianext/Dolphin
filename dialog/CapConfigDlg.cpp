// F:\Workspace\MediaStudio\Dolphin\dialog\CapConfigDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "../Config.h"
#include "../Dolphin.h"
#include "CapConfigDlg.h"
#include "afxdialogex.h"


// CapConfigDlg 对话框

IMPLEMENT_DYNAMIC(CapConfigDlg, CDialogEx)

CapConfigDlg::CapConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CONFIG_CAP_DLG, pParent)
	, m_VideoCapture(0)
	, m_VideoFormat(0)
	, m_bCaptureSpeaker(TRUE)
	, m_bCaptureMic(TRUE)
{

}

CapConfigDlg::~CapConfigDlg()
{
}

void CapConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_VIDEO_CAP, m_VideoCapture);
	DDX_CBIndex(pDX, IDC_VIDEO_FMT, m_VideoFormat);
	DDX_Check(pDX, IDC_CAP_SPEAKER, m_bCaptureSpeaker);
	DDX_Check(pDX, IDC_CAP_MIC, m_bCaptureMic);
}


BEGIN_MESSAGE_MAP(CapConfigDlg, CDialogEx)
END_MESSAGE_MAP()


// CapConfigDlg 消息处理程序


BOOL CapConfigDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CapConfigDlg::InitConfig(CaptureConfig& capture)
{
	m_VideoCapture = capture.videoCapIdx;
	m_VideoFormat = capture.videoFmtIdx;
	m_bCaptureSpeaker = capture.bCapSpeaker;
	m_bCaptureMic = capture.bCapMic;

	this->UpdateData(FALSE);
}


void CapConfigDlg::GetConfig(CaptureConfig& capture)
{
	this->UpdateData(TRUE);

	capture.videoCapIdx = m_VideoCapture;
	capture.videoFmtIdx = m_VideoFormat;
	capture.bCapSpeaker = m_bCaptureSpeaker;
	capture.bCapMic = m_bCaptureMic;
}