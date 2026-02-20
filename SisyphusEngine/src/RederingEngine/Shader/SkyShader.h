#pragma once
#include <DirectXMath.h>
#include "ShaderBuffers.h"
// Framework
#include "Shader.h"


class Light;


class SkyShader : public Shader {
public:
    SkyShader();
    virtual ~SkyShader() override = default;

    virtual bool Init(ID3D11Device*, HWND,
        const std::wstring&, const std::wstring&) override;
    virtual void SetShaders(ID3D11DeviceContext*) override;

public:
    void SetConstantBuffers(ID3D11DeviceContext*);
    ID3D11Buffer* GetLightBuffer() { return m_lightBuffer.Get(); }

    bool UpdateLightBuffer(ID3D11DeviceContext*, Light*, DirectX::XMFLOAT2);
    bool UpdateSkyBuffer(ID3D11DeviceContext*, const SkyBuffer&);

private:
    bool InitBuffers(ID3D11Device*);

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_lightBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_skyBuffer;
}; // SkyShader