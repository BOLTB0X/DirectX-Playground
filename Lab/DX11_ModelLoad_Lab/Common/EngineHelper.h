// Common/EngineHelper.h
#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <string>
#include <iostream>

namespace EngineHelper {
    inline void ErrorBox(HWND hwnd, const std::wstring& message)
    {
        MessageBoxW(hwnd, message.c_str(), L"Engine Error", MB_OK | MB_ICONERROR);
    } // ErrorBox

    inline void DebugPrint(const std::string& message)
    {
        OutputDebugStringA((message + "\n").c_str());
    } // DebugPrint

    inline bool CheckFailed(HRESULT hr, const std::string& msg)
    {
        if (FAILED(hr)) {
            char buf[512];
            sprintf_s(buf, "[ERROR] %s (HR: 0x%08X)\n", msg.c_str(), (unsigned int)hr);
            OutputDebugStringA(buf);
            return true;
        }
        return false;
    } // CheckFailed

    inline bool SuccessCheck(HRESULT hr, const std::string& msg)
    {
        if (SUCCEEDED(hr)) return true;
        char buf[512];
        sprintf_s(buf, "[ERROR] %s (HR: 0x%08X)\n", msg.c_str(), (unsigned int)hr);
        OutputDebugStringA(buf);
        return false;
    } // SuccessCheck

    inline bool SuccessCheck(bool bSuccess, const std::string& msg)
    {
        if (bSuccess) return true;
        char buf[512];
        sprintf_s(buf, "[ERROR] %s (HR: 0x%08X)\n", msg.c_str(), (unsigned int)bSuccess);
        OutputDebugStringA(buf);
        return false;
    } // SuccessCheck

} // EngineHelper



LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);
