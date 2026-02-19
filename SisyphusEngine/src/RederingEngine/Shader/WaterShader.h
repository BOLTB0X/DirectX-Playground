#pragma once
#include "ShaderBuffers.h"
// Framework
#include "Shader.h"


class WaterShader : public Shader {
public:
    WaterShader();
    WaterShader(const WaterShader& other) = delete;
    virtual ~WaterShader() override = default;

    virtual bool Init(ID3D11Device*, HWND,
        const std::wstring&, const std::wstring&) override;
    virtual void SetShaders(ID3D11DeviceContext*) override;

public:
    void SetConstantBuffers(ID3D11DeviceContext*, ID3D11Buffer*);
    bool UpdateReflectionBuffer(ID3D11DeviceContext*, const DirectX::XMMATRIX&);
    bool UpdateWaterBuffer(ID3D11DeviceContext*, const WaterBuffer&);

private:
    bool InitBuffers(ID3D11Device*);

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_reflectionBuffer; // b3
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_waterBuffer; // b4
}; // WaterShader
