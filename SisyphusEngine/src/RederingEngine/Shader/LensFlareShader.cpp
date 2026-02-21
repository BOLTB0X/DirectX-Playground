#include "Pch.h"
#include "LensFlareShader.h"
// Common
#include "DebugHelper.h"


LensFlareShader::LensFlareShader()
	: Shader()
{
    m_type = ShaderType::LensFlare;
} // LensFlareShader


bool LensFlareShader::Init(ID3D11Device* device, HWND hwnd,
    const std::wstring& vsPath, const std::wstring& psPath)
{
    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;

    if (DebugHelper::SuccessCheck(
        Compile(device, hwnd, vsPath,
            "main", "vs_5_0", &vsBlob),
        "LF_VS Compile Error") == false) return false;
    if (DebugHelper::SuccessCheck(
        Compile(device, hwnd, psPath,
            "main", "ps_5_0", &psBlob),
        "LF_PS Compile Error") == false) return false;

    device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_vertexShader);
    device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_pixelShader);

    D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    device->CreateInputLayout(layoutDesc, 3, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_layout);

    vsBlob->Release();
    psBlob->Release();

    return InitBuffers(device);
} // Init


bool LensFlareShader::InitBuffers(ID3D11Device* device)
{
    // 매트릭스 버퍼 - b0
    // 후처리에선 단위행렬 생략 가능하다고는 함
	// 그래도 일단 만듦
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bd.ByteWidth = sizeof(MatrixBuffer);
    if (FAILED(device->CreateBuffer(&bd, nullptr, &m_matrixBuffer)))
        return false;

    // 상수 버퍼 생성 (글로벌 버퍼 - b1)
    bd.ByteWidth = sizeof(GlobalBuffer);
    if (FAILED(device->CreateBuffer(&bd, nullptr, &m_globalBuffer)))
        return false;

    // 상수 버퍼 생성 (버퍼 - b3)
    //bd.ByteWidth = sizeof(ThresholdBuffer);
    //if (FAILED(device->CreateBuffer(&bd, nullptr, &m_thresholdBuffer)))
    //    return false;

    bd.ByteWidth = sizeof(LensFlareBuffer);
    if (FAILED(device->CreateBuffer(&bd, nullptr, &m_ghostBuffer)))
        return false;

	//bd.ByteWidth = sizeof(GlowBuffer);
 //   if (FAILED(device->CreateBuffer(&bd, nullptr, &m_glowBuffer)))
	//	return false;


    return true;
} // InitBuffers


bool LensFlareShader::UpdateThresholdBuffer(ID3D11DeviceContext* context, const ThresholdBuffer& data)
{
    D3D11_MAPPED_SUBRESOURCE mapped;
    if (FAILED(context->Map(m_thresholdBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
        return false;
    
    memcpy(mapped.pData, &data, sizeof(ThresholdBuffer));
    context->Unmap(m_thresholdBuffer.Get(), 0);

    return true;
} // UpdateThresholdBuffer


bool LensFlareShader::UpdateLensFlareBuffer(ID3D11DeviceContext* context, const LensFlareBuffer& data)
{
    D3D11_MAPPED_SUBRESOURCE mapped;
    if (FAILED(context->Map(m_ghostBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
        return false;

    memcpy(mapped.pData, &data, sizeof(LensFlareBuffer));
    context->Unmap(m_ghostBuffer.Get(), 0);

    return true;
} // UpdateLensFlareBuffer


//bool LensFlareShader::UpdateGlowBuffer(ID3D11DeviceContext* context, const GlowBuffer& data)
//{
//    D3D11_MAPPED_SUBRESOURCE mapped;
//
//    if (FAILED(context->Map(m_glowBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
//        return false;
//
//    memcpy(mapped.pData, &data, sizeof(GlowBuffer));
//    context->Unmap(m_glowBuffer.Get(), 0);
//    return true;
//} // UpdateGlowBuffer


void LensFlareShader::SetShaders(ID3D11DeviceContext* context)
{
    context->IASetInputLayout(m_layout.Get());
    context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
    context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
} // SetShaders


void LensFlareShader::SetConstantBuffers(ID3D11DeviceContext* context, ID3D11Buffer* lightBuffer)
{
    context->VSSetConstantBuffers(0, 1, m_matrixBuffer.GetAddressOf());
    context->PSSetConstantBuffers(1, 1, m_globalBuffer.GetAddressOf());

    // b2
    ID3D11Buffer* buffers[] = { lightBuffer };
    context->PSSetConstantBuffers(2, 1, buffers);

    // b3
    //context->PSSetConstantBuffers(3, 1, m_thresholdBuffer.GetAddressOf());
    context->PSSetConstantBuffers(3, 1, m_ghostBuffer.GetAddressOf());
	// b4
	context->PSSetConstantBuffers(4, 1, m_glowBuffer.GetAddressOf());
} // SetConstantBuffers
