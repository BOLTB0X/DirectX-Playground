// Graphics/Shader/CloudShader.h
#include "CloudShader.h"
// Common
#include "EngineHelper.h"
#include "EngineSettings.h"


/* default */
///////////////////////////////////////////////////////////////////////////


CloudShader::CloudShader() : Shader() {}



bool CloudShader::Init(ID3D11Device* device, HWND hwnd, const std::wstring& vsPath, const std::wstring& psPath)
{
    ComPtr<ID3DBlob> vsBlob;
    ComPtr<ID3DBlob> psBlob;

    if (EngineHelper::SuccessCheck(Compile(device, hwnd,
        vsPath, "main", "vs_5_0",
        vsBlob.GetAddressOf()), "버텍스 셰이더 컴파일 에러")
        == false) return false;
    if (EngineHelper::SuccessCheck(Compile(device, hwnd,
        psPath, "main", "ps_5_0",
        psBlob.GetAddressOf()), "픽셀 셰이더 컴파일 에러")
        == false) return false;

    // 셰이더 생성
    device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, m_vertexShader.GetAddressOf());
    device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, m_pixelShader.GetAddressOf());

    // Input Layout
    D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    device->CreateInputLayout(layoutDesc, 5, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), m_layout.GetAddressOf());

    // 버퍼 초기화
    if (m_matrixBuffer.Init(device) == false) return false;
    if (m_CloudBuffer.Init(device) == false) return false;
    if (m_lightBuffer.Init(device) == false) return false;

    // 샘플러 설정
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    if (FAILED(device->CreateSamplerState(&sampDesc, &m_sampler))) return false;
    return true;
} // Init


void CloudShader::Bind(ID3D11DeviceContext* context)
{
    // VS, PS, Layout, ConstantBuffers(b0) 바인딩
    Shader::Bind(context);

    // 레이마칭은 GS를 사용하지 않으므로 명시적 해제
    context->GSSetShader(nullptr, nullptr, 0);

    // b1 슬롯에 CloudBuffer (Time) 바인딩
    m_CloudBuffer.BindPS(context, 1);

    // b2 슬롯에 LightBuffer (InvViewProj, CamPos) 바인딩
    m_lightBuffer.BindPS(context, 2);

    // s0 슬롯에 샘플러 바인딩
    context->PSSetSamplers(0, 1, m_sampler.GetAddressOf());

    // t0 슬롯에 노이즈 텍스처 바인딩
    if (m_noiseTexture)
        context->PSSetShaderResources(0, 1, m_noiseTexture.GetAddressOf());
} // Bind


void CloudShader::UpdateCloud(ID3D11DeviceContext* context, float time,
    const DirectX::XMFLOAT3& cameraPos, const DirectX::XMMATRIX& invViewProj)
{
    CloudBuffer cData;
    cData.time = time;
    m_CloudBuffer.Update(context, cData);

    // LightBuffer 업데이트
    LightBuffer lData;
    lData.sunDirection = { 0.5f, -0.5f, 0.5f };
    lData.sunColor = { 1.0f, 0.9f, 0.8f, 1.0f };
    lData.ambientColor = { 0.1f, 0.1f, 0.15f, 1.0f };
    lData.cameraPosition = cameraPos;
    lData.padding2 = 0.0f;

    lData.inverseViewProjection = DirectX::XMMatrixTranspose(invViewProj);
    m_lightBuffer.Update(context, lData);
} // UpdateCloud