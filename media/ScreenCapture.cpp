/*
* Author：王德明
* Email：phight@163.com
* QQ群：220954528
*/

#include "stdafx.h"
#include "ScreenCapture.h"


ScreenCapture::ScreenCapture()
{
}


ScreenCapture::~ScreenCapture()
{
}



/////////////// Capture methods ///////////////

int ScreenCapture::AddSink(Sink * sink)
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


int ScreenCapture::GetSupportAttribute(void* attribute)
{
    *(vector<VideoCaptureAttribute*>**)attribute = &m_AttributeList;
    return (int)m_AttributeList.size();
}


int ScreenCapture::SetConfig(void* attribute)
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


int ScreenCapture::GetConfig(void* attribute)
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


CString ScreenCapture::GetName()
{
	return m_CaptureName;
}


CAPTURE_STATUS_E ScreenCapture::GetStatus()
{
	return m_Status;
}


int ScreenCapture::Start()
{
	EnterCriticalSection(&m_critsec);

	m_Status = CAPTURE_STATUS_START;

	LeaveCriticalSection(&m_critsec);

    return 0;
}


int ScreenCapture::Stop()
{
	EnterCriticalSection(&m_critsec);

	m_Status = CAPTURE_STATUS_STOP;

	LeaveCriticalSection(&m_critsec);

    return 0;
}

