// Graphics/Renderer/RenderTarget.cpp
#include "RenderTarget.h"
#include "Common/EngineHelper.h"

/* default */
/////////////////////////////////////////////////////////////////////

RenderTarget::RenderTarget() { m_viewport = { 0 }; } // RenderTarget

RenderTarget::~RenderTarget() {} // ~RenderTarget


bool RenderTarget::Init(ID3D11Device* device, IDXGISwapChain* swapChain, int width, int height) {
    // RenderTargetView 생성
    ComPtr<ID3D11Texture2D> backBufferPtr;
    if (EngineHelper::SuccessCheck(
        swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backBufferPtr),
        "스왑 체인에서 백 버퍼 획득 실패")
        == false) return false;

    if (EngineHelper::SuccessCheck(
        device->CreateRenderTargetView(backBufferPtr.Get(), nullptr, &m_renderTargetView),
        "RenderTargetView 생성 실패") == false) return false;

    // DepthStencilBuffer (2D 텍스처) 설정 및 생성
    D3D11_TEXTURE2D_DESC depthBufferDesc = {};
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

    depthBufferDesc.Width = width;
    depthBufferDesc.Height = height;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    // 깊이 버퍼용 텍스처를 생성
    if (EngineHelper::SuccessCheck(
        device->CreateTexture2D(&depthBufferDesc, nullptr, &m_depthStencilBuffer),
        "깊이 버퍼 텍스처 생성 실패")
        == false) return false;

    // DepthStencilView 생성
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    if (!EngineHelper::SuccessCheck(device->CreateDepthStencilView(m_depthStencilBuffer.Get(), &depthStencilViewDesc, &m_depthStencilView),
        "DepthStencilView 생성 실패")) return false;

    // Viewport 설정 정보 저장
    m_viewport.Width = (float)width;
    m_viewport.Height = (float)height;
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;
    m_viewport.TopLeftX = 0.0f;
    m_viewport.TopLeftY = 0.0f;

    return true;
} // Init


void RenderTarget::Clear(ID3D11DeviceContext* context, float r, float g, float b, float a) {
    float color[4] = { r, g, b, a };

    // 화면과 깊이 버퍼 지우기
    context->ClearRenderTargetView(m_renderTargetView.Get(), color);
    context->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // 렌더 타겟 바인딩 및 뷰포트 설정
    context->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
    context->RSSetViewports(1, &m_viewport);
} // Clear