// Graphics/Shader/StoneShader.h
#pragma once
// Framework
#include "Shader/Shader.h"

class StoneShader : public Shader
{
public:
    StoneShader();
	StoneShader(const StoneShader&) = delete;
    virtual ~StoneShader() = default;

    bool Init(ID3D11Device*, HWND, const std::wstring&, const std::wstring&) override;
    void Bind(ID3D11DeviceContext*) override;
}; // StoneShader