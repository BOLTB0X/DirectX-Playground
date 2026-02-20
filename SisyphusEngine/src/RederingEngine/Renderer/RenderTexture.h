#pragma once
#include <memory>
#include <d3d11.h>

class RenderTarget;

// 오프스크린 렌더링(반사, 굴절, 포스트 프로세싱 등)을 위한  리소스 클래스
class RenderTexture {
public:
    RenderTexture();
    RenderTexture(const RenderTexture&) = delete;
    ~RenderTexture();

    bool Init(ID3D11Device*, int, int);
    void Shutdown();

public:
    void SetLowResolutionRenderTarget(ID3D11DeviceContext*);
    void SetReflectionRenderTarget(ID3D11DeviceContext*);
    void SetRefractionRenderTarget(ID3D11DeviceContext*);

    void SetLowResolutionSRV(ID3D11DeviceContext*, UINT);
    void SetReflectionSRV(ID3D11DeviceContext*, UINT);
    void SetRefractionSRV(ID3D11DeviceContext*, UINT);

    RenderTarget* GetReflectionRT() const;
    RenderTarget* GetRefractionRT() const;
    RenderTarget* GetLowResRT() const;

    void ClearShaderResources(ID3D11DeviceContext*, UINT);

private:
    std::unique_ptr<RenderTarget> m_ReflectionRT;
    std::unique_ptr<RenderTarget> m_RefractionRT;
    std::unique_ptr<RenderTarget> m_LowResRT;
}; // RenderTexture