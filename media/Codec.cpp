/*
* Author：王德明
* Email：phight@163.com
* QQ群：220954528
*/

#include "stdafx.h"
#include "Codec.h"


#define REC_CODEC_RAW              0
#define REC_CODEC_STREAM           1


#define COLOR_CONVERT_USE_TABLE    1


//-------------------------------------------------------------------
//
// Video Conversion functions
//
//-------------------------------------------------------------------

typedef struct tagYUVQUAD {
	BYTE    y;
	BYTE    u;
	BYTE    v;
} YUVQUAD;

__forceinline BYTE Clip(int clr)
{
    return (BYTE)(clr < 0 ? 0 : (clr > 255 ? 255 : clr));
}

__forceinline YUVQUAD ConvertRGBToYCrCb(int r, int g, int b)
{
	YUVQUAD yuv;
#if 1
	yuv.y = Clip(((66 * r + 129 * g + 25 * b + 128) >> 8) + 16);
	yuv.u = Clip(((-38 * r - 74 * g + 112 * b + 128) >> 8) + 128);
	yuv.v = Clip(((112 * r - 94 * g - 18 * b + 128) >> 8) + 128);
#else
	yuv.y = Clip((int)((0.257 * r) + (0.504 * g) + (0.098 * b) + 16));
	yuv.u = Clip((int)(-(0.148 * r) - (0.291 * g) + (0.439 * b) + 128));
	yuv.v = Clip((int)((0.439 * r) - (0.368 * g) - (0.071 * b) + 128));
#endif
	return yuv;
}


//-------------------------------------------------------------------
// RGBtoYUVTable
//-------------------------------------------------------------------

#if COLOR_CONVERT_USE_TABLE
YUVQUAD RGBtoYUVTable[256][256][256] = { 0 };

void InitRGBtoYUVTable()
{
	for (uint16_t r = 0; r < 256; r++)
	{
		for (uint16_t g = 0; g < 256; g++)
		{
			for (uint16_t b = 0; b < 256; b++)
			{
				RGBtoYUVTable[r][g][b] = ConvertRGBToYCrCb(r, g, b);
			}
		}
	}
}
#else
void InitRGBtoYUVTable(){}
#endif


//-------------------------------------------------------------------
// TransformImage_RGB24 
//
// RGB-24 to I420
//-------------------------------------------------------------------

static void TransformImage_RGB24(
    BYTE*       pDst,
    LONG        lDstStride,
    const BYTE* pSrc,
    LONG        lSrcStride,
    DWORD       dwWidthInPixels,
    DWORD       dwHeightInPixels
    )
{
	BYTE* lpDstBitsY = pDst;
	BYTE* lpDstBitsCb = lpDstBitsY + (dwHeightInPixels * lDstStride);
	BYTE* lpDstBitsCr = lpDstBitsCb + (dwHeightInPixels * lDstStride) / 4;

	const BYTE* lpRGB = pSrc;

	for (UINT x = 0; x < dwHeightInPixels; x++)
	{
		const BYTE* lpRGBSel = lpRGB;
		for (UINT y = 0; y < dwWidthInPixels; y++)
		{
			int blue = *(lpRGBSel++);
			int green = *(lpRGBSel++);
			int red = *(lpRGBSel++);

#if COLOR_CONVERT_USE_TABLE
			YUVQUAD* yuv = &RGBtoYUVTable[red][green][blue];
			*(lpDstBitsY++) = yuv->y;
			if (x % 2 == 0 && y % 2 == 0)
			{
				*(lpDstBitsCb++) = yuv->u;
				*(lpDstBitsCr++) = yuv->v;
			}
#else
			YUVQUAD yuv = ConvertRGBToYCrCb(red, green, blue);
			*(lpDstBitsY++) = yuv.y;
			if (x % 2 == 0 && y % 2 == 0)
			{
				*(lpDstBitsCb++) = yuv.u;
				*(lpDstBitsCr++) = yuv.v;
			}
#endif
		}

		lpRGB += lSrcStride;
	}
}

//-------------------------------------------------------------------
// TransformImage_RGB32
//
// RGB-32 to I420
//-------------------------------------------------------------------

static void TransformImage_RGB32(
    BYTE*       pDst,
    LONG        lDstStride,
    const BYTE* pSrc,
    LONG        lSrcStride,
    DWORD       dwWidthInPixels,
    DWORD       dwHeightInPixels
    )
{
	BYTE* lpDstBitsY = pDst;
	BYTE* lpDstBitsCb = lpDstBitsY + (dwHeightInPixels * lDstStride);
	BYTE* lpDstBitsCr = lpDstBitsCb + (dwHeightInPixels * lDstStride) / 4;

	const BYTE* lpRGB = pSrc;

	for (UINT x = 0; x < dwHeightInPixels; x++)
	{
		const BYTE* lpRGBSel = lpRGB;
		for (UINT y = 0; y < dwWidthInPixels; y++)
		{
			BYTE blue = *(lpRGBSel++);
			BYTE green = *(lpRGBSel++);
			BYTE red = *(lpRGBSel++);
			lpRGBSel++;

#if COLOR_CONVERT_USE_TABLE
			YUVQUAD* yuv = &RGBtoYUVTable[red][green][blue];
			*(lpDstBitsY++) = yuv->y;
			if (x % 2 == 0 && y % 2 == 0)
			{
				*(lpDstBitsCb++) = yuv->u;
				*(lpDstBitsCr++) = yuv->v;
			}
#else
			YUVQUAD yuv = ConvertRGBToYCrCb(red, green, blue);
			*(lpDstBitsY++) = yuv.y;
			if (x % 2 == 0 && y % 2 == 0)
			{
				*(lpDstBitsCb++) = yuv.u;
				*(lpDstBitsCr++) = yuv.v;
			}
#endif
		}

		lpRGB += lSrcStride;
	}
}

//-------------------------------------------------------------------
// TransformImage_YUY2 
//
// YUY2 to I420
//-------------------------------------------------------------------

static void TransformImage_YUY2(
    BYTE*       pDst,
    LONG        lDstStride,
    const BYTE* pSrc,
    LONG        lSrcStride,
    DWORD       dwWidthInPixels,
    DWORD       dwHeightInPixels
    )
{
	BYTE* lpDstBitsY = pDst;
	BYTE* lpDstBitsCb = lpDstBitsY + (dwHeightInPixels * lDstStride);
	BYTE* lpDstBitsCr = lpDstBitsCb + (dwHeightInPixels * lDstStride) / 4;

    for (DWORD y = 0; y < dwHeightInPixels; y++)
	{
        WORD *pSrcPel = (WORD*)pSrc;

        for (DWORD x = 0; x < dwWidthInPixels; x+=2)
        {
            // Byte order is U0 Y0 V0 Y1

            int y0 = (int)LOBYTE(pSrcPel[x]);
            int u0 = (int)HIBYTE(pSrcPel[x]);
            int y1 = (int)LOBYTE(pSrcPel[x + 1]);
            int v0 = (int)HIBYTE(pSrcPel[x + 1]);

			*(lpDstBitsY++) = y0;
			*(lpDstBitsY++) = y1;
			if (x % 2 == 0 && y % 2 == 0)
			{
				*(lpDstBitsCb++) = u0;
				*(lpDstBitsCr++) = v0;
			}
        }

        pSrc += lSrcStride;
    }

}


//-------------------------------------------------------------------
// TransformImage_I420
//
// I420 to I420
//-------------------------------------------------------------------

static void TransformImage_I420(
    BYTE* pDst,
    LONG lDstStride,
    const BYTE* pSrc,
    LONG lSrcStride,
    DWORD dwWidthInPixels,
    DWORD dwHeightInPixels
    )
{
    MFCopyImage(pDst, lDstStride, pSrc, lSrcStride, dwWidthInPixels * 3 / 2, dwHeightInPixels);
}


//-------------------------------------------------------------------
// TransformImage_NV12
//
// NV12 to I420
//-------------------------------------------------------------------

static void TransformImage_NV12(
    BYTE* pDst,
    LONG lDstStride,
    const BYTE* pSrc,
    LONG lSrcStride,
    DWORD dwWidthInPixels,
    DWORD dwHeightInPixels
    )
{
    BYTE* lpDstBitsY = pDst;
    BYTE* lpDstBitsCb = lpDstBitsY + (dwHeightInPixels * lDstStride);
    BYTE* lpDstBitsCr = lpDstBitsCb + (dwHeightInPixels * lDstStride) / 4;

    const BYTE* lpSrcBitsY = pSrc;
    const BYTE* lpSrcBitsCb = lpSrcBitsY + (dwHeightInPixels * lDstStride);
    //const BYTE* lpSrcBitsCr = lpSrcBitsCb + 1;

	for (DWORD y = 0; y < dwHeightInPixels; y++)
	{
		WORD *pSrcPel = (WORD*)pSrc;

		for (DWORD x = 0; x < dwWidthInPixels; x++ )
		{

			*(lpDstBitsY++) = *(lpSrcBitsY++);

			if (x % 2 == 0 && y % 2 == 0)
			{
                *(lpDstBitsCb++) =  *(lpSrcBitsCb++);
                *(lpDstBitsCr++) =  *(lpSrcBitsCb++);
			}
		}

		pSrc += lSrcStride;
	}
}


static VideoConversionFunction g_VideoConversions[] =
{
    { MFVideoFormat_RGB32, TransformImage_RGB32 },
    { MFVideoFormat_RGB24, TransformImage_RGB24 },
    { MFVideoFormat_YUY2,  TransformImage_YUY2 },
    { MFVideoFormat_I420,  TransformImage_I420 },
    { MFVideoFormat_NV12,  TransformImage_NV12 }
};

const DWORD   g_cVideoFormats = ARRAYSIZE(g_VideoConversions);


//-------------------------------------------------------------------
//
// Audio Conversion functions
//
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// TransformImage_FLTP
//
// FLT to S16(libfdk-aac) or FLTP(ffmpeg)
//-------------------------------------------------------------------

static void TransformAudio_FLT(MediaFrame* srcFrame, MediaFrame* dstFrame)
{
    int sampleCnt = srcFrame->m_dataSize * 8 / srcFrame->m_bitwide / srcFrame->m_channels;
    float* srcData = (float*)srcFrame->m_pData;
#if USE_FFMPEG
    float* lData = (float*)dstFrame->m_pData;
    float* rData = (float*)(dstFrame->m_pData + dstFrame->m_dataSize/2);
    for (int i = 0; i < sampleCnt; i++)
    {
        *lData++ = *srcData++;
        *rData++ = *srcData++;
    }
#else
    short* dstData = (short*)dstFrame->m_pData;
	if (srcFrame->m_channels==dstFrame->m_channels)
    {
        for (int i = 0; i < sampleCnt * 2; i++)
        {
            *dstData++ = (short)((*srcData++) * 32767.0f);
        }
	}else if (srcFrame->m_channels==1 && dstFrame->m_channels==2)
	{
		for (int i = 0; i < sampleCnt*2; i++)
		{
			*dstData++ = (short)((*srcData++) * 32767.0f);
			*dstData++ = *dstData;
		}
	}else if (srcFrame->m_channels == 2 && dstFrame->m_channels == 1)
	{
	}
#endif
}


static AudioConversionFunction g_AudioConversions[] =
{
	{ MFAudioFormat_Float,  TransformAudio_FLT }
};

const DWORD   g_cAudioFormats = ARRAYSIZE(g_AudioConversions);



//////////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////////

Codec::Codec()
{
	InitializeCriticalSection(&m_vfMtx);
	InitializeCriticalSection(&m_vpMtx);
	InitializeCriticalSection(&m_afMtx);
	InitializeCriticalSection(&m_apMtx);
	InitRGBtoYUVTable();
}


Codec::~Codec()
{

	this->Stop();

	DeleteCriticalSection(&m_vfMtx);
	DeleteCriticalSection(&m_vpMtx);
	DeleteCriticalSection(&m_afMtx);
	DeleteCriticalSection(&m_apMtx);
}


//////////////////////////////////////////////////////////////////////////
// private method
//////////////////////////////////////////////////////////////////////////

HRESULT Codec::ChooseConversionFunction(AttributeType type, REFGUID subtype)
{
	if (type == ATTRIBUTE_TYPE_VIDEO)
	{
		m_videoConvertFn = NULL;

		for (DWORD i = 0; i < g_cVideoFormats; i++)
		{
			if (g_VideoConversions[i].subtype == subtype)
			{
				m_videoConvertFn = g_VideoConversions[i].xform;
				return S_OK;
			}
		}
		return MF_E_INVALIDMEDIATYPE;

	}else if (type == ATTRIBUTE_TYPE_AUDIO)
	{
		m_audioConvertFn = NULL;

		for (DWORD i = 0; i < g_cAudioFormats; i++)
		{
			if (g_AudioConversions[i].subtype == subtype)
			{
				m_audioConvertFn = g_AudioConversions[i].xform;
				return S_OK;
			}
		}
		return MF_E_INVALIDMEDIATYPE;

	}

	return MF_E_INVALIDTYPE;
}


int Codec::ConfigVideoCodec()
{
    int ret = 0;
#if USE_FFMPEG
    AVCodec* codec = avcodec_find_encoder_by_name("libx264");
    if (codec==NULL)
    {
        codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    }
    if (codec==NULL)
    {
        return -1;
    }

    m_pVideoEncoder = avcodec_alloc_context3(codec);

    m_pVideoEncoder->width = m_videoAttribute.width;
    m_pVideoEncoder->height = m_videoAttribute.height;
    m_pVideoEncoder->gop_size = m_videoAttribute.fps;
	m_pVideoEncoder->bit_rate = m_videoAttribute.bitrate*1000;
    m_pVideoEncoder->max_b_frames = 0;
    m_pVideoEncoder->pix_fmt = AV_PIX_FMT_YUV420P;
    m_pVideoEncoder->time_base = { 1, 1000000 };
    av_opt_set(m_pVideoEncoder->priv_data, "preset", "slow", 0);

    ret = avcodec_open2(m_pVideoEncoder, codec, NULL);
    if (ret < 0)
    {
        avcodec_free_context(&m_pVideoEncoder);
        return -1;
    }
#else
	x264_param_t param;

    ret = x264_param_default_preset(&param, "veryfast","zerolatency");

    param.i_threads = 1;
    param.i_csp = X264_CSP_I420;
    param.i_width = m_videoAttribute.width;
    param.i_height = m_videoAttribute.height;
	param.b_vfr_input = 0;
    param.i_fps_num = m_videoAttribute.fps;
    param.i_fps_den = 1;
	param.i_keyint_max = m_videoAttribute.fps;
	param.i_keyint_min = 5;
    param.b_intra_refresh = 0;
    param.b_annexb = 1;

    param.rc.i_rc_method = X264_RC_ABR;
    param.rc.i_bitrate = m_videoAttribute.bitrate;

	switch (m_videoAttribute.profile)
	{
	case 0:
		ret = x264_param_apply_profile(&param, "high");
		break;
	case 1:
		ret = x264_param_apply_profile(&param, "main");
		break;
	case 2:
		ret = x264_param_apply_profile(&param, "baseline");
		break;
	default:
		ret = x264_param_apply_profile(&param, "high");
	}

	m_videoEncoder = x264_encoder_open(&param);

	if (m_videoEncoder)
	{
		x264_encoder_parameters(m_videoEncoder, &param);
	}
	else
	{
		return -1;
	}
#endif
    return 0;
}


int Codec::ConfigAudioCodec()
{
    int ret = 0;
#if USE_FFMPEG
    AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_AAC);
    if (codec == NULL)
    {
        return -1;
    }

    m_pAudioEncoder = avcodec_alloc_context3(codec);
    m_pAudioEncoder->bit_rate = m_audioAttribute.bitrate * 1000;
    m_pAudioEncoder->sample_fmt = AV_SAMPLE_FMT_FLTP;
    m_pAudioEncoder->sample_rate = m_audioAttribute.samplerate;
    m_pAudioEncoder->channels = m_audioAttribute.channel;
    m_pAudioEncoder->channel_layout = av_get_default_channel_layout(m_audioAttribute.channel);

    ret = avcodec_open2(m_pAudioEncoder, NULL, NULL);
    if (ret < 0)
    {
        avcodec_free_context(&m_pAudioEncoder);
        return -1;
    }
#else
	if (aacEncOpen(&m_audioEncoder, 0, m_audioAttribute.channel) != AACENC_OK) {
		OutputDebugString(TEXT("Open AAC encode handle failed!\n"));
        return -1;
    }
    int aot = 2;
    switch (m_audioAttribute.profile)
    {
    case 0: aot = AOT_AAC_LC;  break;
    case 1: aot = AOT_SBR;  break;
    case 2: aot = AOT_PS; break;
    }
    if (aacEncoder_SetParam(m_audioEncoder, AACENC_AOT, aot) != AACENC_OK) {
        OutputDebugString(TEXT("Unable to set the AOT\n"));
        return -1;
    }
    if (aacEncoder_SetParam(m_audioEncoder, AACENC_SAMPLERATE, m_audioAttribute.samplerate) != AACENC_OK) {
        OutputDebugString(TEXT("Unable to set the SAMPLERATE\n"));
        return -1;
    }
    int mode = MODE_1;
    switch (m_audioAttribute.channel) {
    case 1: mode = MODE_1;       break;
    case 2: mode = MODE_2;       break;
    case 3: mode = MODE_1_2;     break;
    case 4: mode = MODE_1_2_1;   break;
    case 5: mode = MODE_1_2_2;   break;
    case 6: mode = MODE_1_2_2_1; break;
    }
    if (aacEncoder_SetParam(m_audioEncoder, AACENC_CHANNELMODE, mode) != AACENC_OK) {
        OutputDebugString(TEXT("Unable to set the channel mode\n"));
        return -1;
    }

    if (aacEncoder_SetParam(m_audioEncoder, AACENC_CHANNELORDER, 1) != AACENC_OK) {
        OutputDebugString(TEXT("Unable to set the channel order\n"));
        return -1;
    }

	if (aacEncoder_SetParam(m_audioEncoder, AACENC_BITRATEMODE, 0) != AACENC_OK) {
		OutputDebugString(TEXT("Unable to set the bitrate mode\n"));
		return -1;
	}

	if (aacEncoder_SetParam(m_audioEncoder, AACENC_BITRATE, m_audioAttribute.bitrate * 1000) != AACENC_OK) {
		OutputDebugString(TEXT("Unable to set the bitrate\n"));
		return -1;
    }

    if (aacEncoder_SetParam(m_audioEncoder, AACENC_TRANSMUX, TT_MP4_ADTS) != AACENC_OK) {
        OutputDebugString(TEXT("Unable to set the transport type\n"));
        return -1;
    }

	if (aacEncEncode(m_audioEncoder, NULL, NULL, NULL, NULL) != AACENC_OK) {
		OutputDebugString(TEXT("Unable to initialize the encoder\n"));
		return 1;
	}
	AACENC_InfoStruct info = { 0 };
	if (aacEncInfo(m_audioEncoder, &info) != AACENC_OK) {
		OutputDebugString(TEXT("Unable to get the encoder info\n"));
		return 1;
	}
#endif
	return 0;
}

int Codec::AllocMemory()
{
	return 0;
}


int Codec::FreeMemory()
{
	EnterCriticalSection(&m_vfMtx);
	while (!videoFrameQueue.empty())
	{
        MediaFrame *pic = videoFrameQueue.front();
		videoFrameQueue.pop();
		delete pic;
	}
	LeaveCriticalSection(&m_vfMtx);

	EnterCriticalSection(&m_vpMtx);
	while (!videoPacketQueue.empty())
	{
		MediaPacket* packet = videoPacketQueue.front();
		videoPacketQueue.pop();
		delete packet;
	}
	LeaveCriticalSection(&m_vpMtx);

	EnterCriticalSection(&m_afMtx);
    while (!audioFrameQueue.empty())
    {
        MediaFrame *frame = audioFrameQueue.front();
        audioFrameQueue.pop();
        delete frame;
	}
	LeaveCriticalSection(&m_afMtx);

	EnterCriticalSection(&m_apMtx);
    while (!audioPacketQueue.empty())
    {
        MediaPacket* packet = audioPacketQueue.front();
        audioPacketQueue.pop();
        delete packet;
	}
	LeaveCriticalSection(&m_apMtx);
	return 0;
}


MediaFrame* Codec::PopVideoPicture()
{
    MediaFrame *pic = NULL;
	EnterCriticalSection(&m_vfMtx);
	if (!videoFrameQueue.empty())
	{
		pic = videoFrameQueue.front();
		videoFrameQueue.pop();
	}
	LeaveCriticalSection(&m_vfMtx);
	return pic;
}


void Codec::PushVideoPicture(MediaFrame* pic)
{
	if (pic)
	{
		EnterCriticalSection(&m_vfMtx);
		videoFrameQueue.push(pic);
		LeaveCriticalSection(&m_vfMtx);
	}
}


MediaPacket* Codec::PopVideoPacket()
{
	MediaPacket* packet = NULL;
	EnterCriticalSection(&m_vpMtx);
	if (!videoPacketQueue.empty())
	{
		packet = videoPacketQueue.front();
		videoPacketQueue.pop();
	}
	LeaveCriticalSection(&m_vpMtx);
	return packet;
}


void Codec::PushVideoPacket(MediaPacket* packet)
{
	if (packet!=NULL)
	{
		EnterCriticalSection(&m_vpMtx);
		videoPacketQueue.push(packet);
		LeaveCriticalSection(&m_vpMtx);
	}
}


MediaFrame* Codec::PopAudioFrame()
{
	MediaFrame *frame = NULL;
	EnterCriticalSection(&m_afMtx);
	if (!audioFrameQueue.empty())
	{
		frame = audioFrameQueue.front();
		audioFrameQueue.pop();
	}
	LeaveCriticalSection(&m_afMtx);
	return frame;
}


void Codec::PushAudioFrame(MediaFrame* frame)
{
	if (frame)
	{
		EnterCriticalSection(&m_afMtx);
		audioFrameQueue.push(frame);
		LeaveCriticalSection(&m_afMtx);
	}
}


MediaPacket* Codec::PopAudioPacket()
{
	MediaPacket* packet = NULL;
	EnterCriticalSection(&m_apMtx);
	if (!audioPacketQueue.empty())
	{
		packet = audioPacketQueue.front();
		audioPacketQueue.pop();
	}
	LeaveCriticalSection(&m_apMtx);
	return packet;
}


void Codec::PushAudioPacket(MediaPacket* packet)
{
	if (packet != NULL)
	{
		EnterCriticalSection(&m_apMtx);
		audioPacketQueue.push(packet);
		LeaveCriticalSection(&m_apMtx);
	}
}


int Codec::InitCodec()
{
#if USE_FFMPEG
    av_register_all();
#endif
	ConfigVideoCodec();
	ConfigAudioCodec();
	AllocMemory();
	return 0;
}


int Codec::UninitCodec()
{
#if USE_FFMPEG
    if (m_pVideoEncoder)
    {
        avcodec_free_context(&m_pVideoEncoder);
    }
    if (m_pAudioEncoder)
    {
        avcodec_free_context(&m_pAudioEncoder);
    }
#else
	if (m_videoEncoder)
	{
		x264_encoder_close(m_videoEncoder);
		m_videoEncoder = NULL;
	}

	if (m_audioEncoder)
	{
		aacEncClose(&m_audioEncoder);
		m_audioEncoder = NULL;
	}
#endif
	FreeMemory();
	return 0;
}


int Codec::InitMuxer()
{
#if USE_FFMPEG
	int ret = 0;
	char* filename = "recorder.mp4";

	AVStream* a_st = NULL;
	AVStream* v_st = NULL;

	ret = avformat_alloc_output_context2(&m_pFormatCtx, NULL, NULL, filename);

	//a_st = avformat_new_stream(m_pFormatCtx, m_pAudioEncoder->codec);
	v_st = avformat_new_stream(m_pFormatCtx, m_pVideoEncoder->codec);
	v_st->time_base = m_pVideoEncoder->time_base;

	ret = avio_open(&m_pFormatCtx->pb, filename, AVIO_FLAG_WRITE);

	ret = avformat_write_header(m_pFormatCtx, NULL);

#endif
	return 0;
}


int Codec::UninitMuxer()
{
#if USE_FFMPEG
	av_write_trailer(m_pFormatCtx);
	avio_close(m_pFormatCtx->pb);
	avformat_free_context(m_pFormatCtx);
#endif
	return 0;
}


DWORD WINAPI Codec::VideoEncodecThread(LPVOID lpParam)
{
	Codec* codec = (Codec*)lpParam;

	clock_t start = clock();
	clock_t finish;
	int preVideoCnt = 0;

#if REC_CODEC_RAW
	ofstream yuvfile;
	yuvfile.open("codec.yuv", ios::out | ios::binary);
#endif
#if REC_CODEC_STREAM
	ofstream h264file;
	h264file.open("codec.h264", ios::out | ios::binary);
#endif

#if USE_FFMPEG
    AVFrame* pic = av_frame_alloc();
    AVPacket pkt;
	int stream_index = av_find_best_stream(codec->m_pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);;
#else
    x264_picture_t* pic = new x264_picture_t;
#endif
    if (pic == NULL)
    {
        return -1;
    }

	while (1)
	{
		if (codec->m_QuitCmd==1)
		{ 
			break;
		}

        MediaFrame* frame = codec->PopVideoPicture();
		if (frame == NULL)
        {
            Sleep(10);
			continue;
        }

        int get_packet = 0;
        MediaPacket* packet = nullptr;

#if REC_CODEC_RAW
        if (yuvfile.is_open())
        {
			yuvfile.write((char*)frame->m_pData, frame->GetFrameSize());
        }
#endif
#if USE_FFMPEG
        av_init_packet(&pkt);
        pkt.data = NULL;    // packet data will be allocated by the encoder
        pkt.size = 0;

        pic->format = codec->m_pVideoEncoder->pix_fmt;
        pic->width = codec->m_pVideoEncoder->width;
        pic->height = codec->m_pVideoEncoder->height;
		pic->linesize[0] = codec->m_pVideoEncoder->width;
		pic->linesize[1] = pic->width / 2;
		pic->linesize[2] = pic->width / 2;
		pic->data[0] = frame->m_pData;
		pic->data[1] = pic->data[0] + pic->width * pic->height;
		pic->data[2] = pic->data[1] + pic->width * pic->height / 4;
		pic->pts = frame->m_uTimestamp;

        int ret = avcodec_encode_video2(codec->m_pVideoEncoder, &pkt, pic, &get_packet);
        if (ret<0)
        {

        }
        if (get_packet)
        {
            packet = new MediaPacket(PACKET_TYPE_VIDEO, pkt.size);
            memcpy(packet->m_pData, pkt.data, pkt.size);

            if (pkt.flags|AV_PKT_FLAG_KEY)
            {
                packet->m_bKeyframe = true;
            }
            packet->m_uTimestamp = pkt.pts / 1000;

			pkt.stream_index = stream_index;
			ret = av_write_frame(codec->m_pFormatCtx, &pkt);
        }
#else
        x264_picture_init(pic);
        pic->img.i_csp = X264_CSP_I420;
        pic->img.i_plane = 3;
        pic->img.i_stride[0] = frame->m_width;
        pic->img.i_stride[1] = frame->m_width / 2;
        pic->img.i_stride[2] = frame->m_width / 2;
        pic->img.plane[0] = frame->m_pData;
        pic->img.plane[1] = pic->img.plane[0] + frame->m_stride * frame->m_height;
        pic->img.plane[2] = pic->img.plane[1] + frame->m_stride * frame->m_height / 4;
        pic->i_pts = frame->m_uTimestamp;

		x264_picture_t pic_out;
		x264_nal_t *nal;
		int i_nal;
		int i_frame_size = 0;

		i_frame_size = x264_encoder_encode(codec->m_videoEncoder, &nal, &i_nal, pic, &pic_out);
		if (i_frame_size > 0)
		{
            get_packet = 1;
			//OutputDebugString(TEXT("get video packet.\n"));
			packet = new MediaPacket(PACKET_TYPE_VIDEO, i_frame_size);
			int size = 0;
			for (int i = 0; i < i_nal; ++i)
			{
				memcpy(&(packet->m_pData[size]), nal[i].p_payload, nal[i].i_payload);
				size += nal[i].i_payload;				
			}

			if (pic_out.b_keyframe)
			{
				packet->m_bKeyframe = true;
			}
			packet->m_uTimestamp = pic_out.i_pts / 1000;

        }
#endif
        if (get_packet)
        {
#if REC_CODEC_STREAM
            if (h264file.is_open())
            {
                h264file.write((char *)packet->m_pData, packet->m_dataSize);
            }
#endif
            codec->m_videoDecCnt++;
            //codec->PushVideoPacket(packet);
            delete packet;
        }
		delete frame;

        //Statistics
        finish = clock();
        if (finish - start >= CLOCKS_PER_SEC)
        {
            double fps = ((double)(codec->m_videoDecCnt - preVideoCnt)) * CLOCKS_PER_SEC / (finish - start);
            codec->m_videoDecFps = fps;
            preVideoCnt = codec->m_videoDecCnt;
            start = finish;
        }
    }
#if USE_FFMPEG
    av_frame_free(&pic);
#else
    delete pic;
#endif
#if REC_CODEC_STREAM
	if (h264file.is_open())
	{
		h264file.flush();
		h264file.close();
	}
#endif
#if REC_CODEC_RAW
	if (yuvfile.is_open())
	{
		yuvfile.flush();
		yuvfile.close();
	}
#endif

	return 0;
}


DWORD WINAPI Codec::AudioEncodecThread(LPVOID lpParam)
{
    Codec* codec = (Codec*)lpParam;
    AudioCodecAttribute attr = codec->m_audioAttribute;

    int in_cursize = 0;
    int in_size = 1024 * attr.channel * attr.bitwide / 8;
    void* pinbuf = malloc(in_size);

#if USE_FFMPEG
    AVFrame* pcm;
    AVPacket pkt;

    pcm = av_frame_alloc();
    pcm->nb_samples = 1024;
    pcm->format = codec->m_pAudioEncoder->sample_fmt;
    pcm->channel_layout = codec->m_pAudioEncoder->channel_layout;
    if (attr.channel==1)
    {
        pcm->linesize[0] = in_size;
        pcm->data[0] = (unsigned char*)pinbuf;
    }
    else if (attr.channel == 2)
    {
        in_size /= 2;
        pcm->linesize[0] = in_size;
        pcm->linesize[1] = in_size;
        pcm->data[0] = (unsigned char*)pinbuf;
        pcm->data[1] = (unsigned char*)pinbuf + in_size;
    }
#else
    AACENC_InfoStruct info = { 0 };
    if (aacEncInfo(codec->m_audioEncoder, &info) != AACENC_OK) {
        OutputDebugString(TEXT("Unable to get the encoder info\n"));
        return 1;
    }

    AACENC_BufDesc inbuf = { 0 };
    int in_identifier = IN_AUDIO_DATA;
    int in_elem_size = 2;

    inbuf.numBufs = 1;
    inbuf.bufferIdentifiers = &in_identifier;
    inbuf.bufElSizes = &in_elem_size;
    inbuf.bufs = &pinbuf;
    inbuf.bufSizes = &in_size;

    AACENC_BufDesc outbuf = { 0 };
    int out_size = info.maxOutBufBytes;
    void* poutbuf = malloc(out_size);
    if (poutbuf==NULL)
    {
        return -1;
    }
    int out_elem_size = 1;
    int out_identifier = OUT_BITSTREAM_DATA;
    outbuf.numBufs = 1;
    outbuf.bufs = &poutbuf;
    outbuf.bufferIdentifiers = &out_identifier;
    outbuf.bufSizes = &out_size;
    outbuf.bufElSizes = &out_elem_size;

    AACENC_InArgs in_args = { 0 };
    in_args.numInSamples = 1024 * attr.channel;

    AACENC_OutArgs out_args = { 0 };
#endif
    unsigned long long timestamp = 0;

#if REC_CODEC_RAW
	ofstream pcmfile;
	pcmfile.open("codec.pcm", ios::out | ios::binary);
#endif
#if REC_CODEC_STREAM
    ofstream aacfile;
    aacfile.open("codec.aac", ios::out | ios::binary);
#endif

    while (1)
    {
        if (codec->m_QuitCmd == 1)
        {
            break;
        }

        MediaFrame* frame = codec->PopAudioFrame();
        if (frame == NULL)
        {
            Sleep(10);
            continue;
        }

#if REC_CODEC_RAW
        if (pcmfile.is_open())
        {
			pcmfile.write((char*)frame->m_pData, frame->m_dataSize);
        }
#endif

#if USE_FFMPEG
        int pframeleft = frame->m_dataSize / 2;
#else
        int pframeleft = frame->m_dataSize;
#endif

        while (1)
        {
            if (pframeleft<=0)
            {
                break;
            }
#if USE_FFMPEG
            if (pframeleft < in_size - in_cursize)
            {
                memcpy(&(pcm->data[0][in_cursize]), &frame->m_pData[frame->m_dataSize/2 - pframeleft], pframeleft);
                memcpy(&(pcm->data[1][in_cursize]), &(frame->m_pData+ frame->m_dataSize/2)[frame->m_dataSize/2 - pframeleft], pframeleft);
                in_cursize += pframeleft;
                pframeleft = 0;
                break;
            }
            else
            {
                memcpy(&pcm->data[0][in_cursize], &frame->m_pData[frame->m_dataSize/2 - pframeleft], in_size - in_cursize);
                memcpy(&pcm->data[1][in_cursize], &(frame->m_pData + frame->m_dataSize / 2)[frame->m_dataSize/2 - pframeleft], in_size - in_cursize);
                pframeleft -= in_size - in_cursize;
                in_cursize = 0;
            }

            av_init_packet(&pkt);
            pkt.data = NULL;    // packet data will be allocated by the encoder
            pkt.size = 0;

            int get_stream;
            int ret = avcodec_encode_audio2(codec->m_pAudioEncoder, &pkt, pcm, &get_stream);
            if (ret < 0)
            {
                OutputDebugString(TEXT("Audio Encoding failed\n"));
                break;
            }
            int size = 0;
            void* pktbuf = NULL;
            if (get_stream)
            {
                size = pkt.size;
                pktbuf = pkt.data;
            }
#else
            if (pframeleft < in_size - in_cursize)
            {
                memcpy(&((char*)pinbuf)[in_cursize], &frame->m_pData[frame->m_dataSize - pframeleft], pframeleft);
                in_cursize += pframeleft;
                pframeleft = 0;
                break;
            }
            else
            {
                memcpy(&((char*)pinbuf)[in_cursize], &frame->m_pData[frame->m_dataSize - pframeleft], in_size - in_cursize);
                pframeleft -= in_size - in_cursize;
                in_cursize = 0;
            }

            AACENC_ERROR err;
            if ((err = aacEncEncode(codec->m_audioEncoder, &inbuf, &outbuf, &in_args, &out_args)) != AACENC_OK) {
                if (err != AACENC_ENCODE_EOF)
                    OutputDebugString(TEXT("Audio Encoding failed\n"));
                break;
            }
            int size = out_args.numOutBytes;
            void* pktbuf = outbuf.bufs[0];
#endif
            if (timestamp == 0)
            {
                timestamp = frame->m_uTimestamp;
            }
            else
            {
                timestamp += 1024 * 1000 * 1000 / attr.samplerate;
            }
            if (size)
            {
                MediaPacket* packet = new MediaPacket(PACKET_TYPE_AUDIO, size);
                memcpy(packet->m_pData, pktbuf, size);
                packet->m_uTimestamp = timestamp / 1000;

#if REC_CODEC_STREAM
                if (aacfile.is_open())
                {
                    aacfile.write((char *)packet->m_pData, packet->m_dataSize);
                }
#endif
                //codec->PushAudioPacket(packet);
				delete packet;
            }
        }

        delete frame;
	}

#if REC_CODEC_RAW
	if (pcmfile.is_open())
	{
		pcmfile.flush();
		pcmfile.close();
	}
#endif
#if REC_CODEC_STREAM
    if (aacfile.is_open())
    {
        aacfile.flush();
        aacfile.close();
    }
#endif

    free(pinbuf);
#if USE_FFMPEG
    av_frame_free(&pcm);
#else
    free(poutbuf);
#endif

	return 0;
}


//////////////////////////////////////////////////////////////////////////
// inherited method
//////////////////////////////////////////////////////////////////////////

int Codec::SetSourceAttribute(void* attribute, AttributeType type)
{
	if (type == ATTRIBUTE_TYPE_VIDEO)
	{
		VideoCaptureAttribute *pattr = (VideoCaptureAttribute *)attribute;
		m_videoSrcAttribute = *pattr; 
		ChooseConversionFunction(type, pattr->format);

	}
	else if (type == ATTRIBUTE_TYPE_AUDIO)
	{
		AudioCaptureAttribute *pattr = (AudioCaptureAttribute *)attribute;
		m_audioSrcAttribute = *pattr;
		ChooseConversionFunction(type, pattr->format);
	}
	return 0;
}


int Codec::SendFrame(MediaFrame * frame)
{
    if (frame == NULL || m_Status!= CODEC_STATUS_START) {
        return -1;
    }

	int ret = 0;

    if (frame->m_FrameType==FRAME_TYPE_VIDEO)
	{

		if (m_videoConvertFn == NULL)
		{
			return MF_E_INVALIDMEDIATYPE;
		}

		if (videoFrameQueue.size()>=MAX_VIDEO_FRAME || videoPacketQueue.size()>=MAX_VIDEO_PACKET)
		{
			m_videoLostCnt++;
			return -1;
		}

        VideoCaptureAttribute attr = {0};
        attr.width = m_videoAttribute.width;
        attr.height = m_videoAttribute.height;
        attr.stride = m_videoAttribute.width;
        attr.format = MFVideoFormat_I420;
        MediaFrame* yuvFrame = new MediaFrame(FRAME_TYPE_VIDEO, &attr);
        yuvFrame->m_uTimestamp = frame->m_uTimestamp;

		m_videoConvertFn(
            yuvFrame->m_pData,
            yuvFrame->m_stride,
			frame->m_pData,
			frame->m_stride,
			frame->m_width,
			frame->m_height
			);

		PushVideoPicture(yuvFrame);
    } 
    else if(frame->m_FrameType == FRAME_TYPE_AUDIO)
	{
		if (m_audioConvertFn == NULL)
		{
			return MF_E_INVALIDMEDIATYPE;
		}

		if (audioFrameQueue.size() >= MAX_AUDIO_FRAME || audioPacketQueue.size() >= MAX_AUDIO_PACKET)
		{
			return -1;
		}

		MediaFrame* sample = new MediaFrame(FRAME_TYPE_AUDIO, MFAudioFormat_PCM, frame->m_dataSize * m_audioAttribute.bitwide * m_audioAttribute.channel / (frame->m_bitwide * frame->m_channels));
        sample->m_FrameType = frame->m_FrameType;
		sample->m_uTimestamp = frame->m_uTimestamp;
		sample->m_channels = m_audioAttribute.channel;
		sample->m_bitwide = m_audioAttribute.bitwide; //支持S16格式的音频
		sample->m_samplerate = m_audioAttribute.samplerate;

		m_audioConvertFn(frame, sample);

		PushAudioFrame(sample);
    }

	return 0;
}


//////////////////////////////////////////////////////////////////////////
// public method
//////////////////////////////////////////////////////////////////////////

int Codec::GetCodecStatistics(CodecStatistics* statistics)
{
	if (statistics)
	{
		statistics->videoFrameCnt  = videoFrameQueue.size();
		statistics->videoPacketCnt = videoPacketQueue.size();
		statistics->videoLostCnt   = m_videoLostCnt;
		statistics->videoDecCnt    = m_videoDecCnt;
		statistics->videoDecFps    = m_videoDecFps;
	}
	return 0;
}


int Codec::SetVideoCodecAttribute(VideoCodecAttribute* attribute)
{
    if (attribute != NULL)
    {
        m_videoAttribute = *attribute;
		m_videoAttribute.width = m_videoSrcAttribute.width;
		m_videoAttribute.height = m_videoSrcAttribute.height;
		m_videoAttribute.fps = m_videoSrcAttribute.fps;
    }
	return 0;
}


int Codec::GetVideoCodecAttribute(const VideoCodecAttribute** attribute)
{
    if (attribute != NULL)
    {
        *attribute = &m_videoAttribute;
    }
    return 0;
}


int Codec::SetAudioCodecAttribute(AudioCodecAttribute* attribute)
{
    if (attribute != NULL)
    {
		m_audioAttribute = *attribute;
#if USE_FFMPEG
        m_audioAttribute.bitwide = 32;
#else
        m_audioAttribute.bitwide = 16;
#endif
    }
	return 0;
}


int Codec::GetAudioCodecAttribute(const AudioCodecAttribute** attribute)
{
    if (attribute != NULL)
    {
        *attribute = &m_audioAttribute;
    }
    return 0;
}


int Codec::Start()
{
	InitCodec();
	InitMuxer();
	m_QuitCmd = 0;
    m_videoThread = CreateThread(NULL, 0, VideoEncodecThread, this, 0, NULL);
    m_audioThread = CreateThread(NULL, 0, AudioEncodecThread, this, 0, NULL);
    m_Status = CODEC_STATUS_START;
	return 0;
}


int Codec::Pause()
{
	return 0;
}


int Codec::Stop()
{
    m_Status = CODEC_STATUS_STOP;
	m_QuitCmd = 1;
    WaitForSingleObject(m_videoThread, INFINITE);
    WaitForSingleObject(m_audioThread, INFINITE);
	UninitMuxer();
	UninitCodec();

	return 0;
}


MediaPacket* Codec::GetVideoPacket()
{
    return PopVideoPacket();
}


MediaPacket* Codec::GetAudioPacket()
{
    return PopAudioPacket();
}


int Codec::GetVideoPacketCount()
{
	return (int)videoPacketQueue.size();
}


int Codec::GetAudioPacketCount()
{
	return (int)audioPacketQueue.size();
}

