// Graphics/Shader/ColorShader.h
#pragma once
// Framework
#include "Shader/Shader.h"


class ColorShader : public Shader
{
public:
    ColorShader();
    ColorShader(const ColorShader&) = delete;
    virtual ~ColorShader();

    bool Init(ID3D11Device*, HWND, const std::wstring&, const std::wstring&) override;
    void Bind(ID3D11DeviceContext*) override;
}; // ColorShader