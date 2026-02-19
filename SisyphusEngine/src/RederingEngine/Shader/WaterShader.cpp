#include "Pch.h"
#include "WaterShader.h"
// Common
#include "ConstantHelper.h"
#include "DebugHelper.h"


WaterShader::WaterShader()
	: Shader()
{
	m_type = ShaderType::Water;
} // WaterShader


bool WaterShader::Init(ID3D11Device* device, HWND hwnd,
	const std::wstring& vsPath, const std::wstring& psPath)
{
	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* psBlob = nullptr;

	// 셰이더 컴파일
	if (DebugHelper::SuccessCheck(Compile(device, hwnd, vsPath, "main", "vs_5_0", &vsBlob),
		"WaterShader::Init, vs 컴파일 에러") == false) return false;
	if (DebugHelper::SuccessCheck(Compile(device, hwnd, psPath, "main", "ps_5_0", &psBlob),
		"WaterShader::Init, ps 컴파일 에러") == false) return false;

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


bool WaterShader::InitBuffers(ID3D11Device* device)
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

	// b3: Reflection (VS용)
	desc.ByteWidth = sizeof(DirectX::XMMATRIX);
	if (FAILED(device->CreateBuffer(&desc, nullptr, &m_reflectionBuffer))) return false;

	// b4: Water (PS용)
	desc.ByteWidth = sizeof(WaterBuffer);
	if (FAILED(device->CreateBuffer(&desc, nullptr, &m_waterBuffer))) return false;

	return true;
} // InitBuffers


void WaterShader::SetShaders(ID3D11DeviceContext* context)
{
	context->IASetInputLayout(m_layout.Get());
	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
} // SetShaders


void WaterShader::SetConstantBuffers(ID3D11DeviceContext* context, ID3D11Buffer* lightBuffer)
{
	// VS: Matrix(b0), Reflection(b3)
	context->VSSetConstantBuffers(0, 1, m_matrixBuffer.GetAddressOf());
	context->VSSetConstantBuffers(3, 1, m_reflectionBuffer.GetAddressOf());

	// PS: Global(b1), Light(b2), Water(b4)
	context->PSSetConstantBuffers(1, 1, m_globalBuffer.GetAddressOf());
	context->PSSetConstantBuffers(2, 1, &lightBuffer);
	context->PSSetConstantBuffers(4, 1, m_waterBuffer.GetAddressOf());
} // SetConstantBuffers


bool WaterShader::UpdateReflectionBuffer(ID3D11DeviceContext* context, const DirectX::XMMATRIX& reflectionMatrix)
{
	D3D11_MAPPED_SUBRESOURCE mapped;
	if (FAILED(context->Map(m_reflectionBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
		return false;

	// Transpose 처리
	*(DirectX::XMMATRIX*)mapped.pData = DirectX::XMMatrixTranspose(reflectionMatrix);
	context->Unmap(m_reflectionBuffer.Get(), 0);
	return true;
} // UpdateReflectionBuffer


bool WaterShader::UpdateWaterBuffer(ID3D11DeviceContext* context, const WaterBuffer& data)
{
	D3D11_MAPPED_SUBRESOURCE mapped;
	if (FAILED(context->Map(m_waterBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
		return false;

	*(WaterBuffer*)mapped.pData = data;
	context->Unmap(m_waterBuffer.Get(), 0);
	return true;
} // UpdateWaterBuffer