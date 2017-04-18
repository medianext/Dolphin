/*
* Author：王德明
* Email：phight@163.com
* QQ群：220954528
*/

#pragma once


enum FrameType {

    FRAME_TYPE_VIDEO = 0,
    FRAME_TYPE_AUDIO,
};


class MediaFrame
{
public:
    MediaFrame();
    MediaFrame(FrameType frameType, void* attribute);
	MediaFrame(FrameType frameType, GUID subtype, DWORD dataSize);
	MediaFrame(BYTE *pData, FrameType type, void* attribute);
    MediaFrame(IMFMediaBuffer*, FrameType type, void* attribute);
    ~MediaFrame();

private:
	void Inversion();

public:
    // common filed
    FrameType m_FrameType;
	GUID  m_subtype;

    uint8_t  *m_pData = NULL;
	uint32_t m_dataSize = 0;

	uint64_t m_uTimestamp = 0;

    // video specific filed
    int m_stride = 0;
    int m_width = 0;
    int m_height = 0;

    // audio specific filed
    int m_samplesize = 0;
    int m_samplerate = 0;
    int m_channels = 0;
    int m_bitwide = 0;

public:
    uint32_t CopyTo(void* pData, uint32_t dataSize) const;
    uint32_t CopyFrom(void* pData, uint32_t dataSize) const;
    FrameType GetFrameType() const;
    uint32_t GetFrameSize() const;
};

