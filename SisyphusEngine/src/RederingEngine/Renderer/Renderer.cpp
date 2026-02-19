#include "Pch.h"
#include "Renderer.h"
#include "DisplayInfo.h"
#include "DX11Device.h"
#include "RenderTarget.h"
#include "Rasterizer.h"
#include "DepthStencilState.h"
#include "BlendState.h"
#include "SamplerState.h"
// Common
#include "ConstantHelper.h"


using namespace ConstantHelper;


Renderer::Renderer()
    : m_vsync_enabled(false)
{
    m_DisplayInfo = std::make_unique<DisplayInfo>();
    m_DX11Device = std::make_unique<DX11Device>();
    m_MainRenderTarget = std::make_unique<RenderTarget>();
    m_LowResRenderTarget = std::make_unique<RenderTarget>();
	m_ReflectionRT = std::make_unique<RenderTarget>();
	m_RefractionRT = std::make_unique<RenderTarget>();
    m_Rasterizer = std::make_unique<Rasterizer>();
    m_DepthStencilState = std::make_unique<DepthStencilState>();
    m_BlendState = std::make_unique<BlendState>();
    m_WrapSampler = std::make_unique<SamplerState>();
	m_BorderSampler = std::make_unique<SamplerState>();
} // Renderer


Renderer::~Renderer() {}


bool Renderer::Init(HWND hwnd, bool vsync)
{
    m_vsync_enabled = vsync;

    // 장치 생성
    if (m_DisplayInfo->Init(
        SCREEN_WIDTH, SCREEN_HEIGHT) == false) return false;

    // DisplayInfo의 정보 주입 및 스왑체인 생성
    if (m_DX11Device->Init(hwnd, SCREEN_WIDTH,
        SCREEN_HEIGHT, FULL_SCREEN, vsync,
        m_DisplayInfo->GetNumerator(),
        m_DisplayInfo->GetDenominator())
        == false)
        return false;

    // 렌더 타겟뷰 생성 
    if (m_MainRenderTarget->Init(
        m_DX11Device->GetDevice(),
        m_DX11Device->GetSwapChain(),
        SCREEN_WIDTH, SCREEN_HEIGHT)
        == false)
        return false;

    if (m_LowResRenderTarget->Init(
        m_DX11Device->GetDevice(),
        SCREEN_WIDTH, SCREEN_HEIGHT)
        == false)
        return false;

    if (m_ReflectionRT->Init(
        m_DX11Device->GetDevice(),
        SCREEN_WIDTH, SCREEN_HEIGHT) == false)
        return false;

    if (m_RefractionRT->Init(
        m_DX11Device->GetDevice(),
        SCREEN_WIDTH, SCREEN_HEIGHT) == false)
        return false;

    // 각종 파이프라인 상태 객체 초기화
    ID3D11Device* device = m_DX11Device->GetDevice();
    if (m_Rasterizer->Init(device) == false)
        return false;
    if (m_DepthStencilState->Init(device) == false)
        return false;
    if (m_BlendState->Init(device) == false)
        return false;
    if (m_WrapSampler->Init(device, D3D11_TEXTURE_ADDRESS_WRAP)
        == false) return false;
    if (m_BorderSampler->Init(device, D3D11_TEXTURE_ADDRESS_BORDER)
        == false) return false;
    return true;
} // Init


void Renderer::BeginScene(float r, float g, float b, float a)
{
    ID3D11DeviceContext* context = m_DX11Device->GetDeviceContext();

    m_MainRenderTarget->Clear(context, r, g, b, a);
} // BeginScene


void Renderer::EndScene()
{
    bool vsync = ConstantHelper::VSYNC_ENABLED;
    if (vsync)
    {
        m_DX11Device->GetSwapChain()->Present(1, 0);
    }
    else
    {
        m_DX11Device->GetSwapChain()->Present(0, 0);
    }
} // EndScene


void Renderer::ClearShaderResources(UINT slot)
{
    ID3D11ShaderResourceView* nullSRV = nullptr;
    m_DX11Device->GetDeviceContext()->PSSetShaderResources(slot, 1, &nullSRV);
} // ClearShaderResources


void Renderer::Shutdown()
{
	if (m_BorderSampler) m_BorderSampler.reset();
    if (m_WrapSampler) m_WrapSampler.reset();
    if (m_BlendState) m_BlendState.reset();
    if (m_DepthStencilState) m_DepthStencilState.reset();
    if (m_Rasterizer) m_Rasterizer.reset();
    if (m_ReflectionRT) m_ReflectionRT.reset();
    if (m_RefractionRT) m_RefractionRT.reset();
    if (m_MainRenderTarget) m_MainRenderTarget.reset();
    if (m_DX11Device) m_DX11Device.reset();
    if (m_DisplayInfo) m_DisplayInfo.reset();
} // Shutdown


void Renderer::SetMode(bool enable, bool back)
{
    if (enable)
        m_Rasterizer->SetWireframeState(m_DX11Device->GetDeviceContext());
    else
    {
        if (back)
            m_Rasterizer->SetSolidState(m_DX11Device->GetDeviceContext());
        else
            m_Rasterizer->SetNoCullingState(m_DX11Device->GetDeviceContext());
    }
} // SetMode


void Renderer::SetAlphaBlending(bool enable)
{
    m_BlendState->OMSetBlendState(m_DX11Device->GetDeviceContext(), enable);
} // SetAlphaBlending


void Renderer::SetDepthBuffer(bool enable)
{
    m_DepthStencilState->OMSetDepthStencilState(m_DX11Device->GetDeviceContext(), enable);
} // SetDepthBuffer


void Renderer::SetBackBufferRenderTarget()
{
    ID3D11DeviceContext* context = m_DX11Device->GetDeviceContext();
    ID3D11RenderTargetView* rtv = m_MainRenderTarget->GetRenderTargetView();
    ID3D11DepthStencilView* dsv = m_MainRenderTarget->GetDepthStencilView();

    context->OMSetRenderTargets(1, &rtv, dsv);
    context->RSSetViewports(1, &m_MainRenderTarget->GetViewport());
} // SetBackBufferRenderTarget


void Renderer::SetWrapSampler(UINT slot)
{
    m_WrapSampler->VSSetSamplers(m_DX11Device->GetDeviceContext(), slot);
    m_WrapSampler->PSSetSamplers(m_DX11Device->GetDeviceContext(), slot);
} // SetWrapSampler


void Renderer::SetBorderSampler(UINT slot)
{
    m_BorderSampler->VSSetSamplers(m_DX11Device->GetDeviceContext(), slot);
    m_BorderSampler->PSSetSamplers(m_DX11Device->GetDeviceContext(), slot);
} // SetBorderSampler


void Renderer::SetLowResolutionRenderTarget()
{
    ID3D11DeviceContext* context = m_DX11Device->GetDeviceContext();
    ClearShaderResources(0);

    m_LowResRenderTarget->Clear(context, 0, 0, 0, 1);
} // SetLowResolutionRenderTarget


void Renderer::SetLowResolutionShaderResources(UINT slot)
{
    ID3D11DeviceContext* context = m_DX11Device->GetDeviceContext();
    auto lowResSRV  = m_LowResRenderTarget->GetSRV();
    context->PSSetShaderResources(slot, 1, &lowResSRV);
} // SetLowResolutionShaderResources


void Renderer::SetAdditiveAlphaBlending()
{
    m_BlendState->SetAdditiveBlendState(m_DX11Device->GetDeviceContext());
} // SetAdditiveAlphaBlending


void Renderer::SetMainDepthShaderResource(UINT slot)
{
    ID3D11DeviceContext* context = m_DX11Device->GetDeviceContext();

    auto depthSRV = m_MainRenderTarget->GetDepthSRV();
    context->PSSetShaderResources(slot, 1, &depthSRV);
} // SetMainDepthShaderResource


void Renderer::SetRenderTarget(RenderTarget* target, float r, float g, float b, float a)
{
    ID3D11DeviceContext* context = m_DX11Device->GetDeviceContext();

    // 기존에 바인딩된 SRV들 해제
    ClearShaderResources(0);
    ClearShaderResources(1);

    // 타겟 클리어 및 설정
    target->Clear(context, r, g, b, a);

    ID3D11RenderTargetView* rtv = target->GetRenderTargetView();
    ID3D11DepthStencilView* dsv = target->GetDepthStencilView();

    context->OMSetRenderTargets(1, &rtv, dsv);
    context->RSSetViewports(1, &target->GetViewport());
} // SetRenderTarget


void Renderer::SetReflectionShaderResource(UINT slot)
{
    ID3D11DeviceContext* context = m_DX11Device->GetDeviceContext();
    auto srv = m_ReflectionRT->GetSRV();
    context->PSSetShaderResources(slot, 1, &srv);
} // SetReflectionShaderResource


void Renderer::SetRefractionShaderResource(UINT slot)
{
    ID3D11DeviceContext* context = m_DX11Device->GetDeviceContext();
    auto srv = m_RefractionRT->GetSRV();
    context->PSSetShaderResources(slot, 1, &srv);
} // SetRefractionShaderResource


ID3D11Device* Renderer::GetDevice() const
{
    return m_DX11Device->GetDevice();
} // GetDevice


ID3D11DeviceContext* Renderer::GetDeviceContext() const
{
    return m_DX11Device->GetDeviceContext();
} // GetDeviceContext


RenderTarget* Renderer::GetReflectionRT() const
{
    return m_ReflectionRT.get();
} // GetReflectionRT


RenderTarget* Renderer::GetRefractionRT() const
{
    return m_RefractionRT.get();
} // GetRefractionRT
