// Graphics/Shade/CloudShader.h
#pragma once
#include <memory>
#include "ConstantBuffer.h"
#include "Model/Texture/Material.h"
// Framework
#include "Shader/Shader.h"
// Graphics
#include "Model/Texture/Material.h"

struct LightBuffer {
    DirectX::XMFLOAT3 sunDirection;
    float padding1;
    DirectX::XMFLOAT4 sunColor;
    DirectX::XMFLOAT4 ambientColor;
    DirectX::XMFLOAT3 cameraPosition; // 카메라 위치 (레이 오리진)
    float padding2;
    DirectX::XMMATRIX inverseViewProjection; // 역 View-Proj 행렬
};


class CloudShader : public Shader {
public:
    CloudShader();
    virtual ~CloudShader() override = default;

    virtual bool Init(ID3D11Device* device, HWND hwnd, const std::wstring& vsPath, const std::wstring& psPath) override;

    virtual void Bind(ID3D11DeviceContext* context) override;

    virtual void UpdateMaterialTag(ID3D11DeviceContext* context, int tag, float time = 0.0f, XMFLOAT4 mPos = { 0,0,0,0 }, XMFLOAT4 sPos = { 0,0,0,0 }) override
    {};

    void UpdateCloud(ID3D11DeviceContext* context, float time,
        const DirectX::XMFLOAT3& cameraPos, const DirectX::XMMATRIX& invViewProj);

private:
    ComPtr<ID3D11GeometryShader> m_geometryShader;
    ComPtr<ID3D11SamplerState> m_pointSampler; // FBM 노이즈용
    ComPtr<ID3D11ShaderResourceView> m_noiseTexture; // FBM 데이터가 담긴 텍스처
    ConstantBuffer<CloudBuffer> m_CloudBuffer;
    ConstantBuffer<LightBuffer> m_lightBuffer;
    ComPtr<ID3D11SamplerState> m_sampler;
};