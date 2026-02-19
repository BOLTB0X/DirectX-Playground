#include "Pch.h"
#include "RefractionShader.h"
#include "DebugHelper.h"

RefractionShader::RefractionShader()
    : Shader()
{
    m_type = ShaderType::Refraction;
} // RefractionShader


bool RefractionShader::Init(ID3D11Device* device, HWND hwnd, const std::wstring& vsPath, const std::wstring& psPath)
{
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;

    if (DebugHelper::SuccessCheck(Compile(device, hwnd, vsPath, "main", "vs_5_0", &vsBlob), "RefractionVS 컴파일 실패")
        == false) return false;
    if (DebugHelper::SuccessCheck(Compile(device, hwnd, psPath, "main", "ps_5_0", &psBlob), "RefractionPS 컴파일 실패")
        == false) return false;

    device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_vertexShader);
    device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_pixelShader);

    D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    device->CreateInputLayout(layoutDesc, ARRAYSIZE(layoutDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_layout);
    vsBlob->Release();
    psBlob->Release();

    return InitBuffers(device);
} // Init


bool RefractionShader::InitBuffers(ID3D11Device* device)
{
    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    // b0: Matrix
    desc.ByteWidth = sizeof(MatrixBuffer);
    if (FAILED(device->CreateBuffer(&desc, nullptr, &m_matrixBuffer))) return false;

    // b1: Global
    desc.ByteWidth = sizeof(GlobalBuffer);
    if (FAILED(device->CreateBuffer(&desc, nullptr, &m_globalBuffer))) return false;

    // b5: ClipPlane
    desc.ByteWidth = sizeof(RefractionBuffer);
    if (FAILED(device->CreateBuffer(&desc, nullptr, &m_clipPlaneBuffer))) return false;

    return true;
} // InitBuffers


void RefractionShader::SetShaders(ID3D11DeviceContext* context)
{
    context->IASetInputLayout(m_layout.Get());
    context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
    context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
} // SetShaders


void RefractionShader::SetConstantBuffers(ID3D11DeviceContext* context, ID3D11Buffer* lightBuffer)
{
    // VS: Matrix(b0), ClipPlane(b5)
    context->VSSetConstantBuffers(0, 1, m_matrixBuffer.GetAddressOf());
    context->VSSetConstantBuffers(5, 1, m_clipPlaneBuffer.GetAddressOf());

    // PS: Global(b1), Light(b2)
    context->PSSetConstantBuffers(1, 1, m_globalBuffer.GetAddressOf());
    context->PSSetConstantBuffers(2, 1, &lightBuffer);
} // SetConstantBuffers


bool RefractionShader::UpdateRefractionBuffer(ID3D11DeviceContext* context, const RefractionBuffer& data)
{
    D3D11_MAPPED_SUBRESOURCE mapped;
    if (FAILED(context->Map(m_clipPlaneBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
        return false;
    *(RefractionBuffer*)mapped.pData = data;
    context->Unmap(m_clipPlaneBuffer.Get(), 0);
    return true;
} // UpdateRefractionBuffer