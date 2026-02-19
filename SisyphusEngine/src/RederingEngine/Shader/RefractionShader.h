#pragma once
#include "Shader.h"
#include "ShaderBuffers.h"


class RefractionShader : public Shader {
public:
    RefractionShader();
	RefractionShader(const RefractionShader& other) = delete;
    virtual ~RefractionShader() override = default;

    virtual bool Init(ID3D11Device*, HWND, const std::wstring&, const std::wstring&) override;
    virtual void SetShaders(ID3D11DeviceContext*) override;

public:
    bool UpdateRefractionBuffer(ID3D11DeviceContext*, const RefractionBuffer&);
    void SetConstantBuffers(ID3D11DeviceContext*, ID3D11Buffer*);

private:
    bool InitBuffers(ID3D11Device*);

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_clipPlaneBuffer; // b5
}; // RefractionShader
