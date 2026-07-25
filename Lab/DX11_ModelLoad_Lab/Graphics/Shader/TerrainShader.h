// Graphics/Shader/TerrainShader.h
#pragma once
#include <memory>
#include "ConstantBuffer.h"
// Framework
#include "Shader/Shader.h"
// Graphics
#include "Model/Texture/Material.h"


class TerrainShader : public Shader {
public:
    TerrainShader();
    TerrainShader(const TerrainShader& other) = delete;
    virtual ~TerrainShader() override;


    virtual bool Init(ID3D11Device* device, HWND hwnd, const std::wstring& vsPath, const std::wstring& psPath) override;
    virtual void Bind(ID3D11DeviceContext* context) override;
    virtual void UpdateMaterialTag(ID3D11DeviceContext*, int,
        float time = 0.0f,
        XMFLOAT4 mPos = { 0,0,0,0 },
        XMFLOAT4 sPos = { 0,0,0,0 }
    );

private:
    std::unique_ptr<ConstantBuffer<MaterialBuffer>> m_materialBuffer;
}; // TerrainShader