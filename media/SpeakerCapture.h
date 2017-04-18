/*
* Author：王德明
* Email：phight@163.com
* QQ群：220954528
*/

#pragma once


#include "Capture.h"

#include <mmdeviceapi.h>
#include <Audioclient.h>


class SpeakerCapture :
    public Capture
{
public:
    SpeakerCapture();
    virtual ~SpeakerCapture();

private:
    CRITICAL_SECTION        m_critsec;

    vector<AudioCaptureAttribute*> m_AttributeList;
    AudioCaptureAttribute*    m_pBestAttribute = NULL;
    AudioCaptureAttribute*    m_pCurrentAttribute = NULL;
    vector<Sink *>           m_Sinks;

	CAPTURE_STATUS_E         m_Status = CAPTURE_STATUS_STOP;
	AudioCaptureStatistics   m_statistics;

    CString                  m_CaptureName;
    CString                  m_CaptureId;

    HANDLE m_pThread = NULL;
    int m_QuitCmd = 0;

    IAudioClient* m_pClient = nullptr;
    IAudioCaptureClient* m_pCapture = nullptr;

    static DWORD WINAPI CaptureSpeakerThread(LPVOID lpParam);

public:
    int AddSink(Sink * sink);
    int GetSupportAttribute(void* attribute);
    int SetConfig(void* attribute);
    int GetConfig(void* attribute);
    CString GetName();
	CAPTURE_STATUS_E GetStatus();
	int GetStatistics(void* statistics);
    int Start();
    int Stop();
};

