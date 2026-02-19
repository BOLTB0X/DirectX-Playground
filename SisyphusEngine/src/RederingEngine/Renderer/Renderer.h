#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <memory>
#include <wrl/client.h>

class DisplayInfo;
class DX11Device;
class RenderTarget;
class Rasterizer;
class DepthStencilState;
class BlendState;
class SamplerState;

class Renderer {
public:
    Renderer();
    Renderer(const Renderer& other) = delete;
    ~Renderer();

    bool Init(HWND hwnd, bool vsync);
    void Shutdown();

    void BeginScene(float r, float g, float b, float a);
    void EndScene();
    void ClearShaderResources(UINT);

public:
    void SetMode(bool, bool);
    void SetAlphaBlending(bool);
    void SetDepthBuffer(bool);
    void SetBackBufferRenderTarget();
    void SetWrapSampler(UINT slot);
    void SetBorderSampler(UINT slot);
    void SetLowResolutionRenderTarget();
    void SetLowResolutionShaderResources(UINT);
	void SetAdditiveAlphaBlending();
	void SetMainDepthShaderResource(UINT slot);
    void SetRenderTarget(RenderTarget*, float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f);
    void SetReflectionShaderResource(UINT slot);
    void SetRefractionShaderResource(UINT slot);

    ID3D11Device* GetDevice() const;
    ID3D11DeviceContext* GetDeviceContext() const;
    RenderTarget* GetReflectionRT() const;
    RenderTarget* GetRefractionRT() const;

private:
    bool m_vsync_enabled;

    // 하드웨어 및 기본 리소스
    std::unique_ptr<DisplayInfo> m_DisplayInfo;
    std::unique_ptr<DX11Device>  m_DX11Device;
    std::unique_ptr<RenderTarget> m_MainRenderTarget;
    std::unique_ptr<RenderTarget> m_LowResRenderTarget;
    std::unique_ptr<RenderTarget> m_ReflectionRT; // 반사용
    std::unique_ptr<RenderTarget> m_RefractionRT; // 굴절용

    // 파이프라인 규칙(States)
    std::unique_ptr<Rasterizer> m_Rasterizer;
    std::unique_ptr<DepthStencilState> m_DepthStencilState;
    std::unique_ptr<BlendState> m_BlendState;
    std::unique_ptr<SamplerState> m_WrapSampler;
    std::unique_ptr<SamplerState> m_BorderSampler;
}; // Renderer