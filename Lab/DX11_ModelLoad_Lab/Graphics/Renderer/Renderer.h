// Graphics/Renderer/Renderer.h
#pragma once
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d11.h>
#include <wrl/client.h>
#include <directxmath.h>
#include <memory>

#include "DisplayInfo.h"
#include "DX11Device.h"
#include "RenderTarget.h"
#include "Rasterizer.h"
#include "DepthStencilState.h"
#include "BlendState.h"

class DisplayInfo;

using Microsoft::WRL::ComPtr;

class Renderer {
public:
    Renderer();
    Renderer(const Renderer&) = delete;
    ~Renderer();

    bool Init(HWND, int, int);
    void Shutdown();

    void BeginScene(float, float, float, float);
    void EndScene();

public:
    DX11Device* GetDX11Device() const { return m_DX11Device.get(); }
    ID3D11DeviceContext* GetDeviceContext() const { return m_DX11Device->GetContext(); }
    Rasterizer* GetRasterizer() const { return m_Rasterizer.get(); }
    DepthStencilState* GetDepthStencil() const { return m_DepthStencilState.get(); }
    BlendState* GetBlendState() const { return m_BlendState.get(); }

    void TurnOnAlphaBlending() { m_BlendState->Bind(m_DX11Device->GetContext(), true); }
    void TurnOffAlphaBlending() { m_BlendState->Bind(m_DX11Device->GetContext(), false); }
    void TurnOnDepthWrite() { m_DepthStencilState->Bind(m_DX11Device->GetContext(), true); }
    void TurnOffDepthWrite() { m_DepthStencilState->Bind(m_DX11Device->GetContext(), false); }

public:
    void SetWireframeMode(bool);

private:
    bool m_vsync_enabled;

private:
    // 하드웨어 및 기본 리소스
    std::unique_ptr<DisplayInfo> m_DisplayInfo;
    std::unique_ptr<DX11Device>  m_DX11Device;
    std::unique_ptr<RenderTarget> m_RenderTarget;

    // 파이프라인 규칙(States)
    std::unique_ptr<Rasterizer> m_Rasterizer;
    std::unique_ptr<DepthStencilState> m_DepthStencilState;
    std::unique_ptr<BlendState> m_BlendState;
}; // Renderer