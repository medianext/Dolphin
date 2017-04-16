/*
* Author：王德明
* Email：phight@163.com
* QQ群：220954528
*/

#include "stdafx.h"
#include "SpeakerCapture.h"


SpeakerCapture::SpeakerCapture()
{
}


SpeakerCapture::~SpeakerCapture()
{
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
    *(vector<VideoCaptureAttribute*>**)attribute = &m_AttributeList;
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
			*(VideoCaptureAttribute*)attribute = *m_pCurrentAttribute;
		}
		else
		{
			*(VideoCaptureAttribute*)attribute = *m_pBestAttribute;
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

	LeaveCriticalSection(&m_critsec);

    return 0;
}


int SpeakerCapture::Stop()
{
	EnterCriticalSection(&m_critsec);

	m_Status = CAPTURE_STATUS_STOP;

	LeaveCriticalSection(&m_critsec);

    return 0;
}

