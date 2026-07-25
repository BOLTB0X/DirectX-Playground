// Graphics/Shader/Shader.h
#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <wrl/client.h>
#include <string>
// Graphics
#include "Shader/ConstantBuffer.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

class Shader {
protected:
    struct MatrixBuffer
    {
        XMMATRIX world;
        XMMATRIX view;
        XMMATRIX projection;
    };

public:
    Shader();
	Shader(const Shader&) = delete;
    virtual ~Shader() = default;

    virtual bool Init(ID3D11Device*, HWND, const std::wstring&, const std::wstring&) = 0;
    virtual void Bind(ID3D11DeviceContext* context = 0) = 0;
    virtual void UpdateMaterialTag(
        ID3D11DeviceContext*, int,
        float time = 0.0f,
        XMFLOAT4 mPos = { 0,0,0,0 },
        XMFLOAT4 sPos = { 0,0,0,0 }
    ) = 0;

public:
    bool UpdateMatrixBuffer(ID3D11DeviceContext*, const XMMATRIX&, const XMMATRIX& v, const XMMATRIX&);

protected:
    bool Compile(ID3D11Device*, HWND , const std::wstring&, LPCSTR, LPCSTR, ID3DBlob**);
    void OutputError(ID3DBlob*, HWND, const std::wstring&);

protected:
    ComPtr<ID3D11VertexShader> m_vertexShader;
    ComPtr<ID3D11PixelShader> m_pixelShader;
    ComPtr<ID3D11InputLayout> m_layout;
    ComPtr<ID3D11SamplerState> m_sampleState;

	ConstantBuffer<MatrixBuffer> m_matrixBuffer;
}; // Shader
