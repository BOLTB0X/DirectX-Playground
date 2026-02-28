#pragma once
#include "ShaderBuffers.h"
// Framework
#include "Shader.h"


class VolumetricCloudsShader : public Shader {
public:
    VolumetricCloudsShader();
    VolumetricCloudsShader(const VolumetricCloudsShader& other) = delete;
    virtual ~VolumetricCloudsShader() = default;

    virtual bool Init(ID3D11Device*, HWND,
        const std::wstring&, const std::wstring&) override;
    virtual void SetShaders(ID3D11DeviceContext*) override;

public:
    void SetConstantBuffers(ID3D11DeviceContext*, ID3D11Buffer*);
    bool UpdateVolumetricCloudsBuffer(ID3D11DeviceContext*, const VolumetricCloudsBuffer&);

private:
    bool InitBuffers(ID3D11Device*);

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_cloudBuffer;
}; // CloudShader