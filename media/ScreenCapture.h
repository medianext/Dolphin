/*
* Author：王德明
* Email：phight@163.com
* QQ群：220954528
*/

#pragma once

#include <d3d11.h>
#include <dxgi1_2.h>

#include "Capture.h"


class ScreenCapture :
    public Capture
{
public:
    ScreenCapture();
    virtual ~ScreenCapture();

private:
    CRITICAL_SECTION        m_critsec;

    vector<VideoCaptureAttribute*> m_AttributeList;
    VideoCaptureAttribute*    m_pBestAttribute = NULL;
    VideoCaptureAttribute*    m_pCurrentAttribute = NULL;
    vector<Sink *>           m_Sinks;

    CAPTURE_STATUS_E         m_Status = CAPTURE_STATUS_STOP;

    CString                  m_CaptureName;

    HANDLE m_pThread = NULL;
    int m_QuitCmd = 0;

    ID3D11Device * m_pD3d11Device = nullptr;
    ID3D11DeviceContext* m_pD3d11DeviceContext;
	ID3D11Texture2D* m_pCurrentFrame;
    IDXGIOutputDuplication* m_pScreenDuplication;

    static DWORD WINAPI CaptureScreenThread(LPVOID lpParam);

public:
    int AddSink(Sink * sink);
    int GetSupportAttribute(void* attribute);
    int SetConfig(void* attribute);
    int GetConfig(void* attribute);
    CString GetName();
    CAPTURE_STATUS_E GetStatus();
    int Start();
    int Stop();
};

