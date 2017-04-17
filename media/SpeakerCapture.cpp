/*
* Author：王德明
* Email：phight@163.com
* QQ群：220954528
*/

#include "stdafx.h"
#include "SpeakerCapture.h"

#include <Functiondiscoverykeys_devpkey.h>


SpeakerCapture::SpeakerCapture()
{
    HRESULT hr = S_OK;

    InitializeCriticalSection(&m_critsec);
     
    IMMDeviceEnumerator *pEnumerator = nullptr;
    IMMDevice *pDevice = nullptr;
    WAVEFORMATEX *pWaveFmt = NULL;

    m_hSignalEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    m_hExitEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);

    hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);

    hr = pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, nullptr, (void**)&m_pClient);

    hr = m_pClient->GetMixFormat(&pWaveFmt);

    AudioCaptureAttribute* attribute = new AudioCaptureAttribute();
    attribute->format = MFAudioFormat_Float;
    attribute->samplerate = pWaveFmt->nSamplesPerSec;
    attribute->channel = pWaveFmt->nChannels;
    attribute->bitwide = pWaveFmt->wBitsPerSample;
    m_AttributeList.push_back(attribute);
    m_pBestAttribute = attribute;

    m_pCurrentAttribute = new AudioCaptureAttribute();
    *m_pCurrentAttribute = *attribute;

    hr = m_pClient->Initialize(
        AUDCLNT_SHAREMODE_SHARED,
        AUDCLNT_STREAMFLAGS_LOOPBACK | AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
        1000000,
        1000000,
        pWaveFmt,
        nullptr);

    hr = m_pClient->SetEventHandle(m_hSignalEvent);

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

    if (m_hSignalEvent != NULL)
    {
        CloseHandle(m_hSignalEvent);
    }
    if (m_hExitEvent != NULL)
    {
        CloseHandle(m_hExitEvent);
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
			sink->SetSourceAttribute(m_pCurrentAttribute, ATTRIBUTE_TYPE_VIDEO);
		}
		else
		{
			sink->SetSourceAttribute(m_pBestAttribute, ATTRIBUTE_TYPE_VIDEO);
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
    SetEvent(m_hExitEvent);
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

    HANDLE hEvent[] = { capture->m_hSignalEvent, capture->m_hExitEvent };

    BYTE* pData;
    UINT32 dataSize;
    DWORD  dwFlags;

    UINT32 frameSize = 1024 * capture->m_pCurrentAttribute->bitwide / 8;

    HRESULT hr = S_OK;

    capture->m_statistics = { 0 };

    clock_t start = clock();
    clock_t finish;
    int count = 0;

    hr = pAudio->Start();

    hr = pAudio->GetBufferSize(&dataSize);

    while (1)
    {
        DWORD idx = WaitForMultipleObjects(2, hEvent, FALSE, INFINITE);
        if (capture->m_QuitCmd)
        {
            break;
        }

        EnterCriticalSection(&capture->m_critsec);

        UINT32 size = 0;
        hr = pCapture->GetNextPacketSize(&size);

        hr = pCapture->GetBuffer(&pData, &dataSize, &dwFlags, NULL, NULL);
        if (dwFlags | AUDCLNT_BUFFERFLAGS_SILENT)
        {
        }else if (dwFlags | AUDCLNT_BUFFERFLAGS_TIMESTAMP_ERROR)
        {
        }
        
        hr = pCapture->ReleaseBuffer(dataSize);

//         vector<Sink*>::iterator iter = capture->m_Sinks.begin();
//         for (; iter != capture->m_Sinks.end(); ++iter)
//             (*iter)->SendFrame(frame);

        LeaveCriticalSection(&capture->m_critsec);
    }

    hr = pAudio->Stop();

    return 0;
}