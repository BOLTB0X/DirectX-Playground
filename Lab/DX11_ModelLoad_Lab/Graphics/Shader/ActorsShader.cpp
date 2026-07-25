// Graphics/Shader/ActorsShader.h
#include "ActorsShader.h"
// Common
#include "EngineHelper.h"
#include "EngineSettings.h"

/* defualt */
///////////////////////////////////////////////////////

using namespace EngineHelper;

ActorsShader::ActorsShader() : Shader() {};


ActorsShader::~ActorsShader() {};


bool ActorsShader::Init(ID3D11Device* device, HWND hwnd, const std::wstring& vsPath, const std::wstring& psPath)
{
    ComPtr<ID3DBlob> vsBlob;
    ComPtr<ID3DBlob> psBlob;

    // 셰이더 컴파일
    if (SuccessCheck(Compile(device, hwnd,
        vsPath, "main", "vs_5_0",
        vsBlob.GetAddressOf()), "버텍스 셰이더 컴파일 에러")
        == false) return false;
    if (SuccessCheck(Compile(device, hwnd,
        psPath, "main", "ps_5_0",
        psBlob.GetAddressOf()), "픽셀 셰이더 컴파일 에러")
        == false) return false;

    // 셰이더 생성
    device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, m_vertexShader.GetAddressOf());
    device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, m_pixelShader.GetAddressOf());

    m_materialBuffer = std::make_unique<ConstantBuffer<MaterialBuffer>>();
    if (SuccessCheck(m_materialBuffer->Init(device), "머테리얼버퍼 초기화 에러") == false)
    {
        return false;
    }

    D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    device->CreateInputLayout(layoutDesc, 5, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), m_layout.GetAddressOf());

    if (SuccessCheck(m_matrixBuffer.Init(device), "상수 버퍼 초기화 에러")
        == false) return false;

    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    return SUCCEEDED(device->CreateSamplerState(&samplerDesc, m_sampleState.GetAddressOf()));
} // Init

void ActorsShader::Bind(ID3D11DeviceContext* context)
{
    Shader::Bind(context);
    m_matrixBuffer.BindPS(context, 0);
    m_materialBuffer->BindPS(context, 2);
    context->PSSetSamplers(0, 1, m_sampleState.GetAddressOf());
} // Bind


void ActorsShader::UpdateMaterialTag(ID3D11DeviceContext* context, int type, float time, XMFLOAT4 mPos, XMFLOAT4 sPos)
{
    MaterialBuffer data;
    data.type = type;
    m_materialBuffer->Update(context, data);
} // UpdateMaterialTag