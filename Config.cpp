#include "stdafx.h"
#include "Config.h"

#define CONFIG_FILE_MAGIC 0x221133ff


//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

Config::Config()
{
    Init();
}


Config::Config(CString name)
{
    filename = name;
    if (!LoadFromFile())
    {
        Init();
    } 
}


Config::~Config()
{
    SaveToFile();
}


//////////////////////////////////////////////////////////////////////////
// private method
//////////////////////////////////////////////////////////////////////////

uint32_t Config::CalculateCRC(uint8_t* data, uint32_t size)
{
    int count = size / 4;
    int rest = size % 4;

    uint32_t crc = 0;

    uint32_t* p = (uint32_t*)data;

    int idx = 0;
    for (; idx < count; idx++)
    {
        crc += p[idx];
    }

    if (rest)
    {
        uint32_t w = 0;
        idx = idx * 4;
        for (int i = 0; i < rest; i++)
        {
            w = data[idx + i] + (w << (i * 8));
        }
        w = w << ((4 - rest) * 8);
        crc += w;
    }

    return crc;
}


bool Config::CheckCRC(uint8_t* data, uint32_t size, uint32_t crc)
{
    uint32_t realcrc = CalculateCRC(data, size);
    return (realcrc == crc);
}


bool Config::IsConfigFileValid(uint8_t* data, uint32_t size, ConfigHead& head)
{
    if (head.magic != CONFIG_FILE_MAGIC)
    {
        return false;
    }

    if (size < head.size || size >= head.size+4)
    {
        return false;
    }

//     if (!CheckCRC(data, size, head.crc))
//     {
//         return false;
//     }

    return true;
}


bool Config::LoadFromFile()
{
    bool ret = false;

    while (filename.GetLength()>0)
    {
        HANDLE file = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

        int filesize = GetFileSize(file, nullptr);
        if (filesize <= sizeof(ConfigHead))
        {
            CloseHandle(file);
            break;
        }

        ConfigHead head;
        DWORD readCnt;
        ReadFile(file, &head, sizeof(ConfigHead),&readCnt, NULL);
        if (readCnt < sizeof(ConfigHead))
        {
            CloseHandle(file);
            break;
        }

        int size = filesize - sizeof(ConfigHead);
        uint8_t* data = new uint8_t[size];
        memset(data, 0, size);
        SetFilePointer(file, sizeof(ConfigHead), NULL, FILE_BEGIN);
        ReadFile(file, data, size, &readCnt, NULL);

        if (IsConfigFileValid(data, size, head))
        {
            SetFilePointer(file, sizeof(ConfigHead), NULL, FILE_BEGIN);
            ReadFile(file, &codecConfig, sizeof(CodecConfig), &readCnt, NULL);
            ReadFile(file, &recordConfig, sizeof(RecordConfig), &readCnt, NULL);
            ReadFile(file, &captureConfig, sizeof(CaptureConfig), &readCnt, NULL);
            ReadFile(file, &softwareConfig, sizeof(SoftwareConfig), &readCnt, NULL);
            ret = true;
        }
        CloseHandle(file);
        delete data;
        break;
        
    }
    return ret;
}


void Config::SaveToFile()
{
    if (filename.GetLength()>0)
    {

        ConfigHead head = {0};
        head.magic = CONFIG_FILE_MAGIC;

        HANDLE file = CreateFile(filename, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);

        int configsize = sizeof(CodecConfig) + sizeof(RecordConfig) + sizeof(CaptureConfig) + sizeof(SoftwareConfig);
        head.size = configsize;
        int size = configsize;
        if (size % 4 != 0)
        {
            size = (size / 4 + 1) * 4;
        }

        uint8_t * data = new uint8_t[size];
        memset(data, 0, size);
        int offset = 0;
        memcpy(&data[offset], &codecConfig, sizeof(CodecConfig));
        offset += sizeof(CodecConfig);
        memcpy(&data[offset], &recordConfig, sizeof(RecordConfig));
        offset += sizeof(RecordConfig);
        memcpy(&data[offset], &captureConfig, sizeof(CaptureConfig));
        offset += sizeof(CaptureConfig);
        memcpy(&data[offset], &softwareConfig, sizeof(SoftwareConfig));
        offset += sizeof(CaptureConfig);

        head.crc = CalculateCRC(data, head.size);

        WriteFile(file, &head, sizeof(ConfigHead), NULL, NULL);
        WriteFile(file, data, size, NULL, NULL);

        delete data;
        CloseHandle(file);
    }
}


void Config::Init()
{
    codecConfig.audioCodec = AUDIO_CODEC_AAC;
    codecConfig.audioProfile = AAC_PROFILE_LC;
    codecConfig.audioRCMethod = RC_METHOD_CBR;
    codecConfig.audioBitrate = 64;

    codecConfig.videoCodec = VIDEO_CODEC_H264;
    codecConfig.videoProfile = H264_PROFILE_HIGH;
    codecConfig.videoRCMethod = RC_METHOD_CBR;
    codecConfig.videoBitrate = 4000;
    codecConfig.videoFps = 25;
    codecConfig.videoGop = 25;

    memset(recordConfig.recordDir,0, sizeof(recordConfig.recordDir));
    wcscpy(recordConfig.recordDir, L"D:\\record");
    recordConfig.recordDiv = 60;
    recordConfig.recordFmt = RECORD_FMT_MP4;
    memset(recordConfig.recordName, 0, sizeof(recordConfig.recordName));
    wcscpy(recordConfig.recordName, L"record-${datetime}");

    captureConfig.videoCapIdx = 0;
    captureConfig.videoFmtIdx = 0;
    captureConfig.bCapMic = true;
    captureConfig.bCapSpeaker = true;

    softwareConfig.bExitBtn = false;
    softwareConfig.bMinBtn = true;
}


//////////////////////////////////////////////////////////////////////////
// public method
//////////////////////////////////////////////////////////////////////////

void Config::Save()
{
    SaveToFile();
}


void Config::SaveToFile(string& filename)
{
}


void Config::SetCodecConfig(CodecConfig& config)
{
    codecConfig = config;
}


void Config::GetCodecConfig(CodecConfig& config)
{
    config = codecConfig;
}


void Config::SetRecordConfig(RecordConfig& config)
{
    recordConfig = config;
}


void Config::GetRecordConfig(RecordConfig& config)
{
    config = recordConfig;
}


void Config::SetCaptureConfig(CaptureConfig& config)
{
    captureConfig = config;
}


void Config::GetCaptureConfig(CaptureConfig& config)
{
    config = captureConfig;
}


void Config::SetSoftwareConfig(SoftwareConfig& config)
{
    softwareConfig = config;
}


void Config::GetSoftwareConfig(SoftwareConfig& config)
{
    config = softwareConfig;
}
