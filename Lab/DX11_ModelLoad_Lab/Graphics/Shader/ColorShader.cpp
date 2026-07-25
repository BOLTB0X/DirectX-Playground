// Graphics/Shader/ColorShader.cpp
#include "ColorShader.h"
#include "EngineHelper.h"

/* default */
///////////////////////////////////////////////////

ColorShader::ColorShader() {}


ColorShader::~ColorShader() {}


bool ColorShader::Init(ID3D11Device* device, HWND hwnd, const std::wstring& vsPath, const std::wstring& psPath)
{
    ComPtr<ID3DBlob> vsBlob;
    ComPtr<ID3DBlob> psBlob;

    if (EngineHelper::SuccessCheck(Compile(device, hwnd,
        vsPath, "ColorVertexShader", "vs_5_0",
        vsBlob.GetAddressOf()),
        "Color VS 컴파일 에러") == false) return false;

    if (EngineHelper::SuccessCheck(Compile(device, hwnd,
        psPath, "ColorPixelShader", "ps_5_0",
        psBlob.GetAddressOf()),
        "Color PS 컴파일 에러") == false) return false;

    device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, m_vertexShader.GetAddressOf());
    device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, m_pixelShader.GetAddressOf());

    D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    device->CreateInputLayout(layoutDesc, 3, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), m_layout.GetAddressOf());

    // 4. 공통 행렬 버퍼 초기화
    if (EngineHelper::SuccessCheck(m_matrixBuffer.Init(device), "Color MatrixBuffer 초기화 에러")
        == false) return false;

    return true;
} // Init


void ColorShader::Bind(ID3D11DeviceContext* context)
{
    Shader::Bind(context);
} // Bind