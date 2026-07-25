// Graphics/Shader/ActorsShader.h
#pragma once
#include <memory>
#include "ConstantBuffer.h"
// Framework
#include "Shader/Shader.h"
// Graphics
#include "Model/Texture/Material.h"

class ActorsShader : public Shader {
public:
    ActorsShader();
    virtual ~ActorsShader();

    bool Init(ID3D11Device*, HWND, const std::wstring&, const std::wstring&) override;
    void Bind(ID3D11DeviceContext* context) override;

public:
    void UpdateMaterialTag(
        ID3D11DeviceContext* context,
        int type,
        float time = 0.0f,
        XMFLOAT4 mPos = { 0,0,0,0 },
        XMFLOAT4 sPos = { 0,0,0,0 }
    ) override;

private:
    std::unique_ptr<ConstantBuffer<MaterialBuffer>> m_materialBuffer;
}; // ActorsShader