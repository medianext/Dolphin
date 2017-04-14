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

    InitializeCriticalSection(&m_critsec);

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
			D3D11_SDK_VERSION, &m_pD3d11Device, &FeatureLevel, &m_pD3d11DeviceContext);
		if (SUCCEEDED(hr))
		{
			// Device creation succeeded, no need to loop anymore
			break;
		}
	}
	if (FAILED(hr))
	{
	}

	hr = m_pD3d11Device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&pDxgiDevice));
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

	hr = pOutput1->DuplicateOutput(m_pD3d11Device, &m_pScreenDuplication);
	if (FAILED(hr))
	{
	}

	DXGI_OUTDUPL_DESC desc;
	m_pScreenDuplication->GetDesc(&desc);

	VideoCaptureAttribute* attribute = new VideoCaptureAttribute();
	attribute->format = MFVideoFormat_RGB32;
	attribute->width = desc.ModeDesc.Width;
	attribute->height = desc.ModeDesc.Height;
	attribute->stride = desc.ModeDesc.Width * 4;
	attribute->fps = desc.ModeDesc.RefreshRate.Numerator / desc.ModeDesc.RefreshRate.Denominator;
	m_AttributeList.push_back(attribute);
	m_pBestAttribute = attribute;

	m_pCurrentAttribute = new VideoCaptureAttribute();
	*m_pCurrentAttribute = *attribute;

	D3D11_TEXTURE2D_DESC DeskTexD;
	RtlZeroMemory(&DeskTexD, sizeof(D3D11_TEXTURE2D_DESC));
	DeskTexD.Width = desc.ModeDesc.Width;
	DeskTexD.Height = desc.ModeDesc.Height;
	DeskTexD.MipLevels = 1;
	DeskTexD.ArraySize = 1;
	DeskTexD.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	DeskTexD.SampleDesc.Count = 1;
	DeskTexD.Usage = D3D11_USAGE_STAGING;
	DeskTexD.BindFlags = 0;
	DeskTexD.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	DeskTexD.MiscFlags = 0;

	hr = m_pD3d11Device->CreateTexture2D(&DeskTexD, nullptr, &m_pCurrentFrame);
	if (FAILED(hr))
	{
	}



	SafeRelease(&pOutput1);
	SafeRelease(&pOutput);
	SafeRelease(&pAdapter);
	SafeRelease(&pDxgiDevice);
}


ScreenCapture::~ScreenCapture()
{
	SafeRelease(&m_pCurrentFrame);
    SafeRelease(&m_pScreenDuplication);
    SafeRelease(&m_pD3d11Device);
    SafeRelease(&m_pD3d11DeviceContext);

    DeleteCriticalSection(&m_critsec);
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

	*m_pCurrentAttribute = *pattr;

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

    m_QuitCmd = 0;
    m_pThread = CreateThread(NULL, 0, CaptureScreenThread, this, 0, NULL);
	LeaveCriticalSection(&m_critsec);

    return 0;
}


int ScreenCapture::Stop()
{
    EnterCriticalSection(&m_critsec);

    m_QuitCmd = 1;
    WaitForSingleObject(m_pThread, INFINITE);

	m_Status = CAPTURE_STATUS_STOP;

	LeaveCriticalSection(&m_critsec);

    return 0;
}


DWORD WINAPI ScreenCapture::CaptureScreenThread(LPVOID lpParam)
{
    ScreenCapture* capture = (ScreenCapture*)lpParam;
	IDXGIOutputDuplication* desktop = capture->m_pScreenDuplication;
	ID3D11DeviceContext* context = capture->m_pD3d11DeviceContext;

    HRESULT hr = S_OK;

    DXGI_OUTDUPL_FRAME_INFO frameInfo;
    IDXGIResource* pResource = nullptr;
    ID3D11Texture2D* pTexture = nullptr;

    while (!capture->m_QuitCmd)
    {
		hr = desktop->AcquireNextFrame(100, &frameInfo, &pResource);
		if (FAILED(hr))
		{
			Sleep(1);
			continue;
		}

        // QI for IDXGIResource
        hr = pResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&pTexture));
        SafeRelease(&pResource);
        if (FAILED(hr))
        {
        }

        D3D11_TEXTURE2D_DESC textureDesc;
        pTexture->GetDesc(&textureDesc);

		context->CopyResource(capture->m_pCurrentFrame, pTexture);
		SafeRelease(&pTexture);

        D3D11_MAPPED_SUBRESOURCE subRESOURCE;
        ZeroMemory(&subRESOURCE, sizeof(D3D11_MAPPED_SUBRESOURCE));
        hr = context->Map(capture->m_pCurrentFrame, 0, D3D11_MAP_READ, 0, &subRESOURCE);

		MediaFrame* frame = new MediaFrame((BYTE*)subRESOURCE.pData, FRAME_TYPE_VIDEO, (void*)capture->m_pCurrentAttribute);
		frame->m_uTimestamp = GetTickCount();

		context->Unmap(capture->m_pCurrentFrame, 0);

        desktop->ReleaseFrame();

		OutputDebugString(TEXT("Get frame...\n"));

		vector<Sink*>::iterator iter = capture->m_Sinks.begin();
		for (; iter != capture->m_Sinks.end(); ++iter)
			(*iter)->SendFrame(frame);

        Sleep(10);
    }

    return 0;
}