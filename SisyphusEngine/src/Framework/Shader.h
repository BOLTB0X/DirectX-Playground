#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <wrl/client.h>
#include <string>

enum class ShaderType { None, Sun, Cloud, Sky, Bicubic, LensFlare, Ocean };

class Shader {
public:
    Shader();
	Shader(const Shader&) = delete;

public:
    // virtual
    virtual ~Shader() = default;
    virtual bool Init(ID3D11Device*, HWND, const std::wstring&, const std::wstring&) = 0;
    virtual void SetShaders(ID3D11DeviceContext*) = 0;

    // common
    bool UpdateMatrixBuffer(ID3D11DeviceContext*, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX);
    bool UpdateGlobalBuffer(ID3D11DeviceContext*, float, float, DirectX::XMFLOAT3);
    ShaderType GetShaderType() const { return m_type; }

protected:
    bool Compile(ID3D11Device*, HWND , const std::wstring&, LPCSTR, LPCSTR, ID3DBlob**);
    void OutputError(ID3DBlob*, HWND, const std::wstring&);

protected:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_layout;

    // 상수버퍼
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_matrixBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_globalBuffer;

    ShaderType m_type;
}; // Shader
