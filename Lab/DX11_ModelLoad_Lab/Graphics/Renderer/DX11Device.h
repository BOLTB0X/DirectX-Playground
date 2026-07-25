// Graphics/Renderer/DX11Device.h
// 장치, 컨텍스트, 스왑체인 생성
#pragma once
#include <d3d11.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class DX11Device {
public:
    DX11Device();
    DX11Device(const DX11Device&) = delete;
    ~DX11Device();

    bool Init(HWND hwnd, int width, int height, 
        bool fullscreen, bool vsync,
        unsigned int numerator, unsigned int denominator);

public:
    ID3D11Device* GetDevice() const { return m_device.Get(); }
    ID3D11DeviceContext* GetContext() const { return m_deviceContext.Get(); }
    IDXGISwapChain* GetSwapChain() const { return m_swapChain.Get(); }

private:
    ComPtr<ID3D11Device> m_device;
    ComPtr<ID3D11DeviceContext> m_deviceContext;
    ComPtr<IDXGISwapChain> m_swapChain;
}; // DX11Device