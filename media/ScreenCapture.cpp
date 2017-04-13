/*
* Author：王德明
* Email：phight@163.com
* QQ群：220954528
*/

#include "stdafx.h"

#include <d3d11.h>
#include "ScreenCapture.h"


ScreenCapture::ScreenCapture()
{
	HRESULT hr = S_OK;

	ID3D11Device * pDevice = nullptr;

	IDXGIDevice * pDxgiDevice = nullptr;
	IDXGIAdapter * pAdapter = nullptr;
	IDXGIOutput * pOutput = nullptr;
	IDXGIOutput1 * pOutput1 = nullptr;

	// Driver types supported
	D3D_DRIVER_TYPE DriverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT NumDriverTypes = ARRAYSIZE(DriverTypes);

	// Feature levels supported
	D3D_FEATURE_LEVEL FeatureLevels[] =
	{ 
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_1
	};
	UINT NumFeatureLevels = ARRAYSIZE(FeatureLevels);
	D3D_FEATURE_LEVEL FeatureLevel;

	// Create device
	for (UINT DriverTypeIndex = 0; DriverTypeIndex < NumDriverTypes; ++DriverTypeIndex)
	{
		hr = D3D11CreateDevice(nullptr, DriverTypes[DriverTypeIndex], nullptr, 0, FeatureLevels, NumFeatureLevels,
			D3D11_SDK_VERSION, &pDevice, &FeatureLevel, nullptr);
		if (SUCCEEDED(hr))
		{
			// Device creation succeeded, no need to loop anymore
			break;
		}
	}
	if (FAILED(hr))
	{
	}

	hr = pDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&pDxgiDevice));
	if (FAILED(hr))
	{
	}

	hr = pDxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&pAdapter));
	if (FAILED(hr))
	{
	}

	hr = pAdapter->EnumOutputs(0, &pOutput);
	if (FAILED(hr))
	{
	}

	hr = pOutput->QueryInterface(__uuidof(IDXGIOutput1), (void**)(&pOutput1));
	if (FAILED(hr))
	{
	}

	hr = pOutput1->DuplicateOutput(pDevice, &m_pScreenDuplication);
	if (FAILED(hr))
	{
	}

	DXGI_OUTDUPL_DESC desc;
	m_pScreenDuplication->GetDesc(&desc);

	SafeRelease(&pOutput1);
	SafeRelease(&pOutput);
	SafeRelease(&pAdapter);
	SafeRelease(&pDxgiDevice);
	SafeRelease(&pDevice);
}


ScreenCapture::~ScreenCapture()
{
	SafeRelease(&m_pScreenDuplication);
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

