// Graphics/Renderer/DX11Device.h
#include "DX11Device.h"
#include "Common/EngineHelper.h"

/* defualt */
/////////////////////////////////////////////////////////////////////

DX11Device::DX11Device() {} // DX11Device


DX11Device::~DX11Device()
{
    if (m_swapChain) m_swapChain->SetFullscreenState(false, nullptr);
} // ~DX11Device


bool DX11Device::Init(
    HWND hwnd,
    int width, int height,
    bool fullscreen, bool vsync,
    unsigned int numerator, unsigned int denominator)
{
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = width;
    swapChainDesc.BufferDesc.Height = height;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    if (vsync)
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
    }
    else
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    }

    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = hwnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    if (fullscreen)
        swapChainDesc.Windowed = false;
    else
    
        swapChainDesc.Windowed = true;

    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags = 0;

    // 장치 및 스왑 체인 생성
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        &featureLevel,
        1,
        D3D11_SDK_VERSION,
        &swapChainDesc,
        &m_swapChain,
        &m_device,
        nullptr,
        &m_deviceContext
    );

    if (EngineHelper::SuccessCheck(hr, "DX11 장치 및 스왑체인 생성 실패")
        == false)
    {
        return false;
    }

    return true;
} // Init