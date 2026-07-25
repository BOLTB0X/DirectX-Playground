// Graphics/Renderer/Renderer.h
#include "Renderer.h"

#include "Common/EngineHelper.h"
#include "Common/EngineSettings.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

/* defualt */
/////////////////////////////////////////////////////////////////////

Renderer::Renderer() : m_vsync_enabled(false) {}


Renderer::~Renderer() {}


bool Renderer::Init(HWND hwnd, int screenWidth, int screenHeight)
{
    HRESULT result;
    m_vsync_enabled = EngineSettings::VSYNC_ENABLED;

    // // 디스플레이 정보 서베이
    m_DisplayInfo = std::make_unique<DisplayInfo>();
    if (m_DisplayInfo->Init(screenWidth, screenHeight) == false)
    {
        EngineHelper::ErrorBox(hwnd, L"1. DisplayInfo 실패");
        return false;
    }
    
    // DX11 디바이스 생성
    m_DX11Device = std::make_unique<DX11Device>();
    if (m_DX11Device->Init(hwnd, screenWidth, screenHeight,
        EngineSettings::FULL_SCREEN, EngineSettings::VSYNC_ENABLED,
        m_DisplayInfo->GetRefreshRateNumerator(),
        m_DisplayInfo->GetRefreshRateDenominator())
        == false)
    {
        EngineHelper::ErrorBox(hwnd, L"1.DX11 디바이스 생성");
        return false;
    }
    
    // RenderTarget 초기화
    m_RenderTarget = std::make_unique<RenderTarget>();
    if (m_RenderTarget->Init(m_DX11Device->GetDevice(),
        m_DX11Device->GetSwapChain(),
        screenWidth, screenHeight)
        == false)
    {
        EngineHelper::ErrorBox(hwnd, L"1.m_RenderTarget 생성");
        return false;
    }

    m_Rasterizer = std::make_unique<Rasterizer>();
    if (m_Rasterizer->Init(m_DX11Device->GetDevice())
        == false)
    {
        EngineHelper::ErrorBox(hwnd, L"1.m_Rasterizer 생성");
        return false;
    }
    
    m_Rasterizer->Bind(m_DX11Device->GetContext());
    
    m_DepthStencilState = std::make_unique<DepthStencilState>();
    if (m_DepthStencilState->Init(m_DX11Device->GetDevice()) == false)
    {
        EngineHelper::ErrorBox(hwnd, L"1.m_DepthStencilState 생성");
        return false;
    }

    // Depth 활성화
    m_DepthStencilState->Bind(m_DX11Device->GetContext(), true);

    m_BlendState = std::make_unique<BlendState>();
    if (m_BlendState->Init(m_DX11Device->GetDevice())
        == false)
    {
        EngineHelper::ErrorBox(hwnd, L"1.m_BlendState 실패");
        return false;
    }

    return true;
} // Init


void Renderer::Shutdown()
{

} // Shutdown

void Renderer::BeginScene(float r, float g, float b, float a)
{
    m_RenderTarget->Clear(m_DX11Device->GetContext(), r, g, b, a);
} // BeginScene


void Renderer::EndScene()
{
    bool vsync = EngineSettings::VSYNC_ENABLED;
    if (vsync)
    {
        m_DX11Device->GetSwapChain()->Present(1, 0);
    }
    else
    {
        m_DX11Device->GetSwapChain()->Present(0, 0);
    }
} // EndScene
/////////////////////////////////////////////////////////////////////


/* private */
/////////////////////////////////////////////////////////////////////

void Renderer::SetWireframeMode(bool enable)
{
    m_Rasterizer->Bind(m_DX11Device->GetContext(), enable);
}