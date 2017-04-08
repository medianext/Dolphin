// F:\Workspace\MediaStudio\Rhino\dialog\EncConfigDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "../Config.h"
#include "../Rhino.h"
#include "EncConfigDlg.h"
#include "afxdialogex.h"


// EncConfigDlg 对话框

IMPLEMENT_DYNAMIC(EncConfigDlg, CDialogEx)

EncConfigDlg::EncConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CONFIG_ENC_DLG, pParent)
	, m_VideoCodec(0)
	, m_VideoProfile(0)
	, m_VideoRCMethod(0)
	, m_VideoBitrate(2000)
	, m_VideoFps(25)
	, m_VideoGop(25)
	, m_AudioCodec(0)
	, m_AudioProfile(0)
	, m_AudioRCMethod(0)
	, m_AudioBitrate(64)
{

}

EncConfigDlg::~EncConfigDlg()
{
}

void EncConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_VIDEO_CODEC, m_VideoCodec);
	DDX_CBIndex(pDX, IDC_VIDEO_PROFILE, m_VideoProfile);
	DDX_CBIndex(pDX, IDC_VIDEO_METHOD, m_VideoRCMethod);
	DDX_Text(pDX, IDC_VIDEO_BITRATE, m_VideoBitrate);
	DDX_Text(pDX, IDC_VIDEO_FPS, m_VideoFps);
	DDX_Text(pDX, IDC_VIDEO_GOP, m_VideoGop);
	DDX_CBIndex(pDX, IDC_AUDIO_CODEC, m_AudioCodec);
	DDX_CBIndex(pDX, IDC_AUDIO_PROFILE, m_AudioProfile);
	DDX_CBIndex(pDX, IDC_AUDIO_METHOD, m_AudioRCMethod);
	DDX_Text(pDX, IDC_AUDIO_BITRATE, m_AudioBitrate);
}


BEGIN_MESSAGE_MAP(EncConfigDlg, CDialogEx)
END_MESSAGE_MAP()


// EncConfigDlg 消息处理程序


BOOL EncConfigDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void EncConfigDlg::InitConfig(CodecConfig& codec)
{
	m_VideoCodec    = codec.videoCodec;
	m_VideoProfile  = codec.videoProfile;
	m_VideoRCMethod = codec.videoRCMethod;
	m_VideoBitrate  = codec.videoBitrate;
	m_VideoFps      = codec.videoFps;
	m_VideoGop      = codec.videoGop;

	m_AudioCodec    = codec.audioCodec;
	m_AudioProfile  = codec.audioProfile;
	m_AudioRCMethod = codec.audioRCMethod;
	m_AudioBitrate  = codec.audioBitrate;

	this->UpdateData(FALSE);
}


void EncConfigDlg::GetConfig(CodecConfig& codec)
{
	this->UpdateData(TRUE);

	codec.videoCodec    = m_VideoCodec;
	codec.videoProfile  = m_VideoProfile;
	codec.videoRCMethod = m_VideoRCMethod;
	codec.videoBitrate  = m_VideoBitrate;
	codec.videoFps      = m_VideoFps;
	codec.videoGop      = m_VideoGop;

	codec.audioCodec    = m_AudioCodec;
	codec.audioProfile  = m_AudioProfile;
	codec.audioRCMethod = m_AudioRCMethod;
	codec.audioBitrate  = m_AudioBitrate;
}