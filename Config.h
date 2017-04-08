#pragma once

#define VIDEO_CODEC_H264 0
#define VIDEO_CODEC_H265 1
#define VIDEO_CODEC_VP8  2
#define VIDEO_CODEC_VP9  3

#define AUDIO_CODEC_AAC   0
#define AUDIO_CODEC_OPUS  1

#define RC_METHOD_CBR
#define RC_METHOD_VBR

struct CodecConfig 
{
	int videoCodec;
	int videoProfile;
	int videoRCMethod;
	int videoBitrate;
	int videoFps;
	int videoGop;

	int audioCodec;
	int audioProfile;
	int audioRCMethod;
	int audioBitrate;
};

#define RECORD_FMT_MP4 0
#define RECORD_FMT_MKV 1
#define RECORD_FMT_FLV 2

struct RecordConfig
{
	int recordFmt;
	int recordDiv;
	wchar_t recordDir[256];
	wchar_t recordName[256];
};

struct CaptureConfig
{
	int videoCapIdx;
	int videoFmtIdx;

	bool bCapSpeaker;
	bool bCapMic;
};

struct SoftwareConfig
{
	bool bExitBtn;
	bool bMinBtn;
};