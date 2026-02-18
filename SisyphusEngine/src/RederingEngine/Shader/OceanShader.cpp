#include "Pch.h"
#include "OceanShader.h"
// Common
#include "ConstantHelper.h"
#include "DebugHelper.h"


OceanShader::OceanShader()
	: Shader()
{
	m_type = ShaderType::Ocean;
} // OceanShader


bool OceanShader::Init(ID3D11Device* device, HWND hwnd,
	const std::wstring& vsPath, const std::wstring& psPath)
{
	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* psBlob = nullptr;

	// 셰이더 컴파일
	if (DebugHelper::SuccessCheck(Compile(device, hwnd, vsPath, "main", "vs_5_0", &vsBlob),
		"OceanShader::Init, vs 컴파일 에러") == false) return false;
	if (DebugHelper::SuccessCheck(Compile(device, hwnd, psPath, "main", "ps_5_0", &psBlob),
		"OceanShader::Init, ps 컴파일 에러") == false) return false;

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


bool OceanShader::InitBuffers(ID3D11Device* device)
{
	// 상수 버퍼 생성 (매트릭스 버퍼 - b0)
	D3D11_BUFFER_DESC matrixBufferDesc = {};

	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	if (FAILED( device->CreateBuffer(&matrixBufferDesc, nullptr, &m_matrixBuffer) ))
		return false;

	// 상수 버퍼 생성 (글로벌 버퍼 - b1)
	matrixBufferDesc.ByteWidth = sizeof(GlobalBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	if (FAILED(device->CreateBuffer(&matrixBufferDesc, nullptr, &m_globalBuffer)))
		return false;

	// 상수 버퍼 생성 (오션 버퍼 - b2)
	matrixBufferDesc.ByteWidth = sizeof(OceanBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	if (FAILED(device->CreateBuffer(&matrixBufferDesc, nullptr, &m_oceanBuffer)))
		return false;

	return true;
} // InitBuffers


void OceanShader::SetShaders(ID3D11DeviceContext* context)
{
	context->IASetInputLayout(m_layout.Get());
	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
} // SetShaders


void OceanShader::SetConstantBuffers(ID3D11DeviceContext* context, ID3D11Buffer* lightBuffer)
{
	context->VSSetConstantBuffers(0, 1, m_matrixBuffer.GetAddressOf());
	context->PSSetConstantBuffers(1, 1, m_globalBuffer.GetAddressOf());

	// 조명 버퍼(b2)
	ID3D11Buffer* buffers[] = { lightBuffer };
	context->PSSetConstantBuffers(2, 1, buffers);

	// 오션 버퍼(b3)
	context->PSSetConstantBuffers(3, 1, m_oceanBuffer.GetAddressOf());
} // SetConstantBuffers


bool OceanShader::UpdateOceanBuffer(ID3D11DeviceContext* context, const OceanBuffer& data)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(context->Map(m_oceanBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		return false;

	OceanBuffer* oceanData = (OceanBuffer*)mappedResource.pData;
	*oceanData = data;
	context->Unmap(m_oceanBuffer.Get(), 0);

	return true;
} // UpdateOceanBuffer