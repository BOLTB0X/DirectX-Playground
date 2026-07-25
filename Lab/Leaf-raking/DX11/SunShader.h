#pragma once
#include "Shader.h"
//#include "Model/Light.h"

class Light;

class SunShader : public Shader {
public:
    SunShader();
    SunShader(const SunShader& other) = delete;
    virtual ~SunShader() override = default;

    virtual bool Init(ID3D11Device*, HWND,
        const std::wstring&, const std::wstring&) override;
    virtual void SetShaders(ID3D11DeviceContext*) override;

public:
    void SetConstantBuffers(ID3D11DeviceContext*);

    ID3D11Buffer* GetLightBuffer() { return m_lightBuffer.Get(); }

    bool UpdateLightBuffer(ID3D11DeviceContext*, Light*);

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_lightBuffer;
}; // SunShader