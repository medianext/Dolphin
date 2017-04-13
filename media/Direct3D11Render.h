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
    HWND m_hWnd;
};

