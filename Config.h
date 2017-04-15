#pragma once

#define VIDEO_CODEC_H264 0
#define VIDEO_CODEC_H265 1
#define VIDEO_CODEC_VP8  2
#define VIDEO_CODEC_VP9  3

#define AUDIO_CODEC_AAC   0
#define AUDIO_CODEC_OPUS  1

#define RC_METHOD_CBR   0
#define RC_METHOD_VBR   1

#define H264_PROFILE_HIGH 0
#define H264_PROFILE_MAIN 1
#define H264_PROFILE_LOW  2

#define AAC_PROFILE_LC   0
#define AAC_PROFILE_HE   1
#define AAC_PROFILE_HEV2 2

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

struct ConfigHead
{
    uint32_t magic;
    uint32_t size;
    uint32_t crc;
};

class Config {

public:
    Config();
    Config(CString filename);
    ~Config();

private:
    CodecConfig codecConfig;
    RecordConfig recordConfig;
    CaptureConfig captureConfig;
    SoftwareConfig softwareConfig;

    CString filename;

    uint32_t CalculateCRC(uint8_t* data, uint32_t size);
    bool CheckCRC(uint8_t* data, uint32_t size, uint32_t crc);

    bool IsConfigFileValid(uint8_t* data, uint32_t size, ConfigHead& head);

    bool LoadFromFile();
    void SaveToFile();
    void Init();

public:
    void Save();
    void SaveToFile(string& filename);
    void SetCodecConfig(CodecConfig& config);
    void GetCodecConfig(CodecConfig& config);
    void SetRecordConfig(RecordConfig& config);
    void GetRecordConfig(RecordConfig& config);
    void SetCaptureConfig(CaptureConfig& config);
    void GetCaptureConfig(CaptureConfig& config);
    void SetSoftwareConfig(SoftwareConfig& config);
    void GetSoftwareConfig(SoftwareConfig& config);
};