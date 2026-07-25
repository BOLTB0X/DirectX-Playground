#include "Pch.h"
#include "SunShader.h"
// 
#include "Model/Light.h"


SunShader::SunShader()
    : Shader() {}


bool SunShader::Init(ID3D11Device* device, HWND hwnd,
    const std::wstring& vsPath, const std::wstring& psPath)
{
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;

    if (Compile(device, hwnd, vsPath, "main", "vs_5_0", &vsBlob)
        == false) return false;
    if (Compile(device, hwnd, psPath, "main", "ps_5_0", &psBlob)
        == false) return false;

    device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_vertexShader);
    device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_pixelShader);

    D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    device->CreateInputLayout(layoutDesc, sizeof(layoutDesc) / sizeof(layoutDesc[0]), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_layout);

    D3D11_BUFFER_DESC matrixDesc = { sizeof(MatrixBuffer), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, 0 };
    device->CreateBuffer(&matrixDesc, nullptr, &m_matrixBuffer);

    D3D11_BUFFER_DESC globalDesc = { sizeof(GlobalBuffer), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, 0 };
    device->CreateBuffer(&globalDesc, nullptr, &m_globalBuffer);

    D3D11_BUFFER_DESC lightDesc = { sizeof(LightBuffer), D3D11_USAGE_DYNAMIC, D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, 0, 0 };
    device->CreateBuffer(&lightDesc, nullptr, &m_lightBuffer);

    m_type = ShaderType::Sun;

    return true;
} // Init


void SunShader::SetShaders(ID3D11DeviceContext* context)
{
    context->IASetInputLayout(m_layout.Get());
    context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
    context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
} // SetShaders


void SunShader::SetConstantBuffers(ID3D11DeviceContext* context)
{
    context->VSSetConstantBuffers(0, 1, m_matrixBuffer.GetAddressOf()); // 행렬(b0)
    context->PSSetConstantBuffers(1, 1, m_globalBuffer.GetAddressOf()); // 레이마칭(b1)
    context->PSSetConstantBuffers(2, 1, m_lightBuffer.GetAddressOf()); //
} // SetConstantBuffers


bool SunShader::UpdateLightBuffer(ID3D11DeviceContext* context, Light* light)
{
    D3D11_MAPPED_SUBRESOURCE mapped;
    if (FAILED(context->Map(m_lightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) return false;

    LightBuffer* data = (LightBuffer*)mapped.pData;
    data->position = light->GetPosition();
    data->color = light->GetColor();
    data->intensity = light->GetIntensity();

    context->Unmap(m_lightBuffer.Get(), 0);
    return true;
} // UpdateLightBuffer