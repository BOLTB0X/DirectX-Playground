#include "Pch.h"
#include "VolumetricCloudsShader.h"
// Common
#include "ConstantHelper.h"
#include "DebugHelper.h"


VolumetricCloudsShader::VolumetricCloudsShader()
	: Shader()
{
	m_type = ShaderType::VolumetricClouds;
} // VolumetricCloudsShader

bool VolumetricCloudsShader::Init(ID3D11Device* device, HWND hwnd,
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

    device->CreateInputLayout(layoutDesc, sizeof(layoutDesc) / sizeof(layoutDesc[0]), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_layout);

    vsBlob->Release();
    psBlob->Release();

    return InitBuffers(device);
} // Init


bool VolumetricCloudsShader::InitBuffers(ID3D11Device* device)
{
    D3D11_BUFFER_DESC matrixBufferDesc = {};
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    device->CreateBuffer(&matrixBufferDesc, nullptr, &m_matrixBuffer);

    matrixBufferDesc.ByteWidth = sizeof(GlobalBuffer);
    device->CreateBuffer(&matrixBufferDesc, nullptr, &m_globalBuffer);

    D3D11_BUFFER_DESC cloudBufferDesc = {};
    cloudBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    cloudBufferDesc.ByteWidth = sizeof(VolumetricCloudsBuffer);
    cloudBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cloudBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    if (FAILED(device->CreateBuffer(&cloudBufferDesc, nullptr, &m_cloudBuffer)))
        return false;

    return true;
} // InitBuffers


void VolumetricCloudsShader::SetShaders(ID3D11DeviceContext* context)
{
    context->IASetInputLayout(m_layout.Get());
    context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
    context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
} // SetShaders


void VolumetricCloudsShader::SetConstantBuffers(ID3D11DeviceContext* context, ID3D11Buffer* lightBuffer)
{
    //context->VSSetConstantBuffers(0, 1, m_matrixBuffer.GetAddressOf());
    context->PSSetConstantBuffers(0, 1, m_matrixBuffer.GetAddressOf());
    context->PSSetConstantBuffers(1, 1, m_globalBuffer.GetAddressOf());

    // 조명 버퍼
    ID3D11Buffer* buffers[] = { lightBuffer };
    context->PSSetConstantBuffers(2, 1, buffers);

    // 구름 버퍼
    context->PSSetConstantBuffers(3, 1, m_cloudBuffer.GetAddressOf());
} // SetConstantBuffers


bool VolumetricCloudsShader::UpdateVolumetricCloudsBuffer(ID3D11DeviceContext* context, const VolumetricCloudsBuffer& data)
{
    D3D11_MAPPED_SUBRESOURCE mapped;
    if (FAILED(context->Map(m_cloudBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
        return false;

    VolumetricCloudsBuffer* pData = (VolumetricCloudsBuffer*)mapped.pData;
    *pData = data;

    context->Unmap(m_cloudBuffer.Get(), 0);
    return true;
} // UpdateVolumetricCloudsBuffer