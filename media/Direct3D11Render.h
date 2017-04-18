/*
* Author：王德明
* Email：phight@163.com
* QQ群：220954528
*/

#pragma once


#include <d3d11.h>

#include "stdafx.h"
#include "Render.h"


class Direct3D11Render :
    public Render
{

public:
    Direct3D11Render(HWND hWnd);
    ~Direct3D11Render();

public:
    int SetSourceAttribute(void* attribute, AttributeType type);
    int SendFrame(MediaFrame * frame);

    int DrawFrame(MediaFrame * frame);

private:
    HRESULT ChooseConversionFunction(REFGUID subtype);

private:
    HWND m_hWnd;

    ID3D11Device* m_pD3d11Device = nullptr;
    IDXGISwapChain* m_pSwapChain = nullptr;
    ID3D11DeviceContext* m_pD3d11DeviceContext = nullptr;
	ID3D11Texture2D* m_pTexture = nullptr;

    RECT                    m_rcCanvas;
    RECT                    m_rcSrc;

    GUID                    m_subtype;

    IMAGE_TRANSFORM_FN      m_convertFn;
};

