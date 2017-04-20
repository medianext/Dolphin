/*
* Author：王德明
* Email：phight@163.com
* QQ群：220954528
*/

#include "stdafx.h"
#include "SpeakerCapture.h"

#include <Functiondiscoverykeys_devpkey.h>

#define REC_CAPTURE_RAW  0


SpeakerCapture::SpeakerCapture()
{
    HRESULT hr = S_OK;

    InitializeCriticalSection(&m_critsec);
     
    IMMDeviceEnumerator *pEnumerator = nullptr;
    IMMDevice *pDevice = nullptr;
    WAVEFORMATEX *pWaveFmt = NULL;

    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);

    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);

    hr = pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&m_pClient);

    hr = m_pClient->GetMixFormat(&pWaveFmt);

    AudioCaptureAttribute* attribute = new AudioCaptureAttribute();
    attribute->format = MFAudioFormat_Float;
    attribute->samplesize = 1024;
    attribute->samplerate = pWaveFmt->nSamplesPerSec;
    attribute->channel = pWaveFmt->nChannels;
    attribute->bitwide = pWaveFmt->wBitsPerSample;
    m_AttributeList.push_back(attribute);
    m_pBestAttribute = attribute;

    m_pCurrentAttribute = new AudioCaptureAttribute();
    *m_pCurrentAttribute = *attribute;

    hr = m_pClient->Initialize(
        AUDCLNT_SHAREMODE_SHARED,
        AUDCLNT_STREAMFLAGS_LOOPBACK,
        1000000,
        1000000,
        pWaveFmt,
        nullptr);

    hr = m_pClient->GetService(__uuidof(IAudioCaptureClient), (void**)&m_pCapture);
    if (AUDCLNT_E_WRONG_ENDPOINT_TYPE==hr)
    {
    }
    else if (AUDCLNT_E_NOT_INITIALIZED == hr)
    {
    }

    LPWSTR pwszID = nullptr;
    hr = pDevice->GetId(&pwszID);
    m_CaptureId = pwszID;
    CoTaskMemFree(pwszID);

    IPropertyStore *pProps = nullptr;
    PROPVARIANT varName;
    PropVariantInit(&varName);
    hr = pDevice->OpenPropertyStore(STGM_READ, &pProps);
    hr = pProps->GetValue(PKEY_Device_FriendlyName, &varName);
    m_CaptureName = varName.pwszVal;
    PropVariantClear(&varName);
    SafeRelease(&pProps);

    CoTaskMemFree(pWaveFmt);
    SafeRelease(&pDevice);
    SafeRelease(&pEnumerator);
}


SpeakerCapture::~SpeakerCapture()
{
    Stop();
    EnterCriticalSection(&m_critsec);
    SafeRelease(&m_pCapture);
    SafeRelease(&m_pClient);
    if (m_pCurrentAttribute)
    {
        delete m_pCurrentAttribute;
        m_pCurrentAttribute = NULL;
    }
    LeaveCriticalSection(&m_critsec);

    AudioCaptureAttribute* pattr = NULL;
    vector<AudioCaptureAttribute*>::iterator it;
    for (it = m_AttributeList.begin(); it != m_AttributeList.end();)
    {
        pattr = *it;
        it = m_AttributeList.erase(it);
        delete pattr;
    }

    DeleteCriticalSection(&m_critsec);
}



/////////////// Capture methods ///////////////

int SpeakerCapture::AddSink(Sink * sink)
{
    if ( sink != NULL )
    {
		if (m_pCurrentAttribute)
		{
			sink->SetSourceAttribute(m_pCurrentAttribute, ATTRIBUTE_TYPE_AUDIO);
		}
		else
		{
			sink->SetSourceAttribute(m_pBestAttribute, ATTRIBUTE_TYPE_AUDIO);
		}
        m_Sinks.push_back(sink);
    }
    return 0;
}


int SpeakerCapture::GetSupportAttribute(void* attribute)
{
    *(vector<AudioCaptureAttribute*>**)attribute = &m_AttributeList;
    return (int)m_AttributeList.size();
}


int SpeakerCapture::SetConfig(void* attribute)
{
	VideoCaptureAttribute* pattr = (VideoCaptureAttribute*)attribute;
	if (pattr==nullptr)
	{
		return -1;
	}

	HRESULT hr = S_OK;

	EnterCriticalSection(&m_critsec);


	LeaveCriticalSection(&m_critsec);

    return 0;
}


int SpeakerCapture::GetConfig(void* attribute)
{
    if (attribute)
    {
		if (m_pCurrentAttribute)
		{
			*(AudioCaptureAttribute*)attribute = *m_pCurrentAttribute;
		}
		else
		{
			*(AudioCaptureAttribute*)attribute = *m_pBestAttribute;
		}
    }
    return 0;
}


CString SpeakerCapture::GetName()
{
	return m_CaptureName;
}


CAPTURE_STATUS_E SpeakerCapture::GetStatus()
{
	return m_Status;
}


int SpeakerCapture::GetStatistics(void* statistics)
{
	if (statistics && m_Status == CAPTURE_STATUS_START)
	{
		AudioCaptureStatistics* pstatistics = (AudioCaptureStatistics*)statistics;
		*pstatistics = m_statistics;
		return 0;
	}
	return -1;
}


int SpeakerCapture::Start()
{

    EnterCriticalSection(&m_critsec);

    m_Status = CAPTURE_STATUS_START;

    m_QuitCmd = 0;
    m_pThread = CreateThread(NULL, 0, CaptureSpeakerThread, this, 0, NULL);
    LeaveCriticalSection(&m_critsec);

    return 0;
}


int SpeakerCapture::Stop()
{
    EnterCriticalSection(&m_critsec);

    m_QuitCmd = 1;
    WaitForSingleObject(m_pThread, INFINITE);

    m_Status = CAPTURE_STATUS_STOP;

    LeaveCriticalSection(&m_critsec);

    return 0;
}

DWORD WINAPI SpeakerCapture::CaptureSpeakerThread(LPVOID lpParam)
{
    SpeakerCapture* capture = (SpeakerCapture*)lpParam;
    IAudioClient* pAudio = capture->m_pClient;
    IAudioCaptureClient* pCapture = capture->m_pCapture;
    AudioCaptureAttribute* pattr = capture->m_pCurrentAttribute;

    BYTE* pData;
    UINT32 dataCnt;
    UINT32 dataSize;
    DWORD  dwFlags;

    bool getFrame = false;

    MediaFrame* pFrame = new MediaFrame(FRAME_TYPE_AUDIO, pattr);
    UINT32 frameSize = pFrame->GetFrameSize();

    uint8_t* tmpBuf = new uint8_t[frameSize];
    uint32_t tmpSize = 0;


    HRESULT hr = S_OK;

    capture->m_statistics = { 0 };

    clock_t start = clock();
    clock_t finish;
    int count = 0;

    hr = pAudio->Start();

    hr = pAudio->GetBufferSize(&dataSize);

#if REC_CAPTURE_RAW
    ofstream capfile;
    capfile.open("speaker.pcm", ios::out | ios::binary);
#endif

    while (1)
    {
        //DWORD idx = WaitForMultipleObjects(2, hEvent, FALSE, INFINITE);
        if (capture->m_QuitCmd)
        {
            break;
        }

        EnterCriticalSection(&capture->m_critsec);

        UINT32 size = 0;
        hr = pCapture->GetNextPacketSize(&size);

        hr = pCapture->GetBuffer(&pData, &dataCnt, &dwFlags, NULL, NULL);
        if (FAILED(hr))
        {
            Sleep(1);
            continue;
        }else if (dwFlags | AUDCLNT_BUFFERFLAGS_SILENT)
        {
        }
        else if (dwFlags | AUDCLNT_BUFFERFLAGS_DATA_DISCONTINUITY)
        {
            Sleep(1);
            continue;
        }
        else if (dwFlags | AUDCLNT_BUFFERFLAGS_TIMESTAMP_ERROR)
        {
            Sleep(1);
            continue;
        }

        dataSize = dataCnt * pattr->channel * pattr->bitwide / 8;
        if (dataSize >= frameSize-tmpSize)
        {
            memcpy(&tmpBuf[tmpSize], pData, frameSize - tmpSize);
            uint32_t copiedSize = (frameSize - tmpSize);
            pFrame->CopyFrom(tmpBuf, frameSize);
            tmpSize = dataSize - copiedSize;
            memcpy(tmpBuf, &pData[copiedSize], tmpSize);
            pFrame->m_uTimestamp = GetTickCount() * 1000;
            getFrame = true;
        }
        else
        {
            memcpy(&tmpBuf[tmpSize], pData, dataSize);
            tmpSize += dataSize;
        }
        
        hr = pCapture->ReleaseBuffer(dataCnt);

        if (getFrame)
        {

#if REC_CAPTURE_RAW
            if (capfile.is_open())
            {
                capfile.write((char*)pFrame->m_pData, frameSize);
            }
#endif
            vector<Sink*>::iterator iter = capture->m_Sinks.begin();
            for (; iter != capture->m_Sinks.end(); ++iter)
                (*iter)->SendFrame(pFrame);
            getFrame = false;
        }

        LeaveCriticalSection(&capture->m_critsec);

        Sleep(1);
    }

    hr = pAudio->Stop();

#if REC_CAPTURE_RAW
    if (capfile.is_open())
    {
        capfile.flush();
        capfile.close();
    }
#endif

    delete tmpBuf;
    delete pFrame;

    return 0;
}