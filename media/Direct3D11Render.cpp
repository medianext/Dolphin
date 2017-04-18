/*
* Author：王德明
* Email：phight@163.com
* QQ群：220954528
*/

#include "stdafx.h"
#include "Direct3D11Render.h"


Direct3D11Render::Direct3D11Render(HWND hWnd)
{
    HRESULT hr = S_OK;

    m_hWnd = hWnd;

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
    
    DXGI_SWAP_CHAIN_DESC swapchainDesc = {0};
    swapchainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    swapchainDesc.BufferDesc.Width = 0;
    swapchainDesc.BufferDesc.Height = 0;
    swapchainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_CENTERED;
    swapchainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
    swapchainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapchainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapchainDesc.SampleDesc.Count = 1;
    swapchainDesc.SampleDesc.Quality = 0;
    swapchainDesc.BufferCount = 4;
    swapchainDesc.Windowed = TRUE;
    swapchainDesc.OutputWindow = m_hWnd;
    swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    swapchainDesc.BufferUsage = 0;

    // Create device
    for (UINT DriverTypeIndex = 0; DriverTypeIndex < NumDriverTypes; ++DriverTypeIndex)
    {
        hr = D3D11CreateDeviceAndSwapChain(
            nullptr,
            DriverTypes[DriverTypeIndex],
            nullptr,
            0,
            FeatureLevels,
            NumFeatureLevels,
            D3D11_SDK_VERSION,
            &swapchainDesc,
            &m_pSwapChain,
            &m_pD3d11Device,
            &FeatureLevel,
            &m_pD3d11DeviceContext);
        if (SUCCEEDED(hr))
        {
            // Device creation succeeded, no need to loop anymore
            break;
        }
    }
    if (FAILED(hr))
    {
    }
}


Direct3D11Render::~Direct3D11Render()
{
}


//////////////////////////////////////////////////////////////////////////
// private method
//////////////////////////////////////////////////////////////////////////

HRESULT Direct3D11Render::ChooseConversionFunction(REFGUID subtype)
{
    m_convertFn = NULL;

    for (DWORD i = 0; i < g_cFormats; i++)
    {
        if (g_FormatConversions[i].subtype == subtype)
        {
            m_convertFn = g_FormatConversions[i].xform;
            return S_OK;
        }
    }

    return MF_E_INVALIDMEDIATYPE;
}


//////////////////////////////////////////////////////////////////////////
// public method
//////////////////////////////////////////////////////////////////////////

int Direct3D11Render::SetSourceAttribute(void* attribute, AttributeType type)
{
    if (attribute == NULL)
    {
        return -1;
    }
    VideoCaptureAttribute* attr = (VideoCaptureAttribute*)attribute;

    HRESULT hr = S_OK;

    hr = ChooseConversionFunction(attr->format);
    if (!SUCCEEDED(hr))
    {
        return -1;
    }
    m_subtype = attr->format;

    //
    RECT rcSrc = { 0, 0, attr->width, attr->height };
    //rcSrc = CorrectAspectRatio(rcSrc, m_PixelAR);
    m_rcDest = LetterBoxRect(rcSrc, m_rcCanvas);

    DXGI_SWAP_CHAIN_DESC swapchainDesc = { 0 };
    hr = m_pSwapChain->GetDesc(&swapchainDesc);
    hr = m_pSwapChain->ResizeBuffers(
        swapchainDesc.BufferCount,
        attr->width,
        attr->height,
        swapchainDesc.BufferDesc.Format,
        swapchainDesc.Flags
        );

    return 0;
}


int Direct3D11Render::SendFrame(MediaFrame * frame)
{
    DrawFrame(frame);
    return 0;
}


int Direct3D11Render::DrawFrame(MediaFrame * frame)
{

    if (m_pD3d11Device == NULL || m_pSwapChain == NULL || frame == NULL)
    {
        return S_OK;
    }

    HRESULT hr = S_OK;

    ID3D11Texture2D* pTexture = nullptr;
    IDXGISurface* pSurface = nullptr;
    DXGI_MAPPED_RECT surfaceRect = { 0 };
    DXGI_SURFACE_DESC surfaceDesc = { 0 };
    D3D11_TEXTURE2D_DESC textureDesc = { 0 };

    hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pTexture);

    pTexture->GetDesc(&textureDesc);

    hr = pTexture->QueryInterface(__uuidof(IDXGISurface), (void**)&pSurface);

    hr = pSurface->GetDesc(&surfaceDesc);

    hr = pSurface->Map(&surfaceRect, DXGI_MAP_READ);

    m_convertFn(
        (BYTE*)surfaceRect.pBits,
        surfaceRect.Pitch,
        frame->m_pData,
        frame->m_stride,
        frame->m_width,
        frame->m_height
        );

    hr = pSurface->Unmap();

    SafeRelease(&pSurface);
    SafeRelease(&pTexture);

    hr = m_pSwapChain->Present(0, 0);

    return 0;
}
