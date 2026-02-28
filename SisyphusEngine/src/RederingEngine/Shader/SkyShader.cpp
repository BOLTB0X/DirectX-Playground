#include "Pch.h"
#include "SkyShader.h"
#include "ShaderBuffers.h"
// Rendering
#include "Model/Light.h"
// Common
#include "ConstantHelper.h"
#include "DebugHelper.h"


SkyShader::SkyShader() : Shader()
{
    m_type = ShaderType::Sky;
} // SkyShader


bool SkyShader::Init(ID3D11Device* device, HWND hwnd,
    const std::wstring& vsPath, const std::wstring& psPath)
{
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;

    // 셰이더 컴파일
    if (DebugHelper::SuccessCheck(Compile(device, hwnd, vsPath, "main", "vs_5_0", &vsBlob),
        "CloudShader::Init, vs 컴파일 에러") == false) return false;
    if (DebugHelper::SuccessCheck(Compile(device, hwnd, psPath, "main", "ps_5_0", &psBlob),
        "CloudShader::Init, ps 컴파일 에러") == false) return false;

    // 셰이더 생성
    device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_vertexShader);
    device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_pixelShader);

    // 입력 레이아웃 생성
    D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    device->CreateInputLayout(layoutDesc, sizeof(layoutDesc) / sizeof(layoutDesc[0]),
        vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_layout);
    vsBlob->Release();
    psBlob->Release();

    return InitBuffers(device);
} // Init


bool SkyShader::InitBuffers(ID3D11Device* device)
{
    D3D11_BUFFER_DESC matrixDesc = {};
    matrixDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixDesc.ByteWidth = sizeof(MatrixBuffer);
    matrixDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    if (FAILED(device->CreateBuffer(&matrixDesc, nullptr, &m_matrixBuffer)))
        return false;

    D3D11_BUFFER_DESC globalDesc = {};
    globalDesc.Usage = D3D11_USAGE_DYNAMIC;
    globalDesc.ByteWidth = sizeof(GlobalBuffer);
    globalDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    globalDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    if (FAILED(device->CreateBuffer(&globalDesc, nullptr, &m_globalBuffer)))
        return false;

    D3D11_BUFFER_DESC lightDesc = {};
    lightDesc.Usage = D3D11_USAGE_DYNAMIC;
    lightDesc.ByteWidth = sizeof(LightBuffer);
    lightDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    lightDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    lightDesc.MiscFlags = 0;
    lightDesc.StructureByteStride = 0;
    if (FAILED(device->CreateBuffer(&lightDesc, nullptr, &m_lightBuffer)))
        return false;

    D3D11_BUFFER_DESC skyDesc = {};
    skyDesc.Usage = D3D11_USAGE_DYNAMIC;
    skyDesc.ByteWidth = sizeof(SkyBuffer);
    skyDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    skyDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    if (FAILED(device->CreateBuffer(&skyDesc, nullptr, &m_skyBuffer)))
        return false;

    return true;
} // InitBuffers


void SkyShader::SetShaders(ID3D11DeviceContext* context)
{
    context->IASetInputLayout(m_layout.Get());
    context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
    context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
} // SetShaders


void SkyShader::SetConstantBuffers(ID3D11DeviceContext* context)
{
    context->VSSetConstantBuffers(0, 1, m_matrixBuffer.GetAddressOf()); // 행렬(b0)
    context->PSSetConstantBuffers(1, 1, m_globalBuffer.GetAddressOf());
    context->PSSetConstantBuffers(2, 1, m_lightBuffer.GetAddressOf());
    context->PSSetConstantBuffers(3, 1, m_skyBuffer.GetAddressOf());
} // SetConstantBuffers


bool SkyShader::UpdateLightBuffer(ID3D11DeviceContext* context, Light* light, DirectX::XMFLOAT2 uv)
{
    D3D11_MAPPED_SUBRESOURCE mapped;
    if (FAILED(context->Map(m_lightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) return false;

    LightBuffer* data = (LightBuffer*)mapped.pData;
    data->direction = light->GetPosition();
    data->intensity = light->GetIntensity();
    data->color = light->GetColor();
    data->lightUV = uv;

    context->Unmap(m_lightBuffer.Get(), 0);
    return true;
} // UpdateLightBuffer


bool SkyShader::UpdateSkyBuffer(ID3D11DeviceContext* context, const SkyBuffer& data)
{
    D3D11_MAPPED_SUBRESOURCE mapped;
    if (FAILED(context->Map(m_skyBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) return false;

    SkyBuffer* pData = (SkyBuffer*)mapped.pData;
    *pData = data;

    context->Unmap(m_skyBuffer.Get(), 0);
    return true;
} // UpdateSkyBuffer
