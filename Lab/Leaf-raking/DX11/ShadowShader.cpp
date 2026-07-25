#include "Pch.h"
#include "ShadowShader.h"
#include "ConstantBufferType.h"
// Utils
#include "Helpers/DebugHelper.h"
#include "Helpers/ShaderHelper.h"
#include "SharedConstants/PathConstants.h"
#include "SharedConstants/BuffersConstants.h"
// define
#define BUFFER_SLOT_MAT      0
#define BUFFER_SLOT_LIGHTPOS 1
#define BUFFER_SLOT_LIGHT    0
#define TEXTURE_DEPH_SLOT    0
#define SAMPLER_CLAMP_SLOT   0
#define SAMPLER_WRAP_SLOT    1

using namespace DirectX;
using namespace SharedConstants;
using namespace PathConstants;

ShadowShader::ShadowShader() {
	//m_matrixBufferData.world = XMMatrixIdentity();
	//m_lightPositionBufferData.padding = 0.0f;
	//m_lightBufferData.padding = { 0.0f, 0.0f, 0.0f };
	m_prevMatrixBufferData.world = XMMatrixIdentity();
	m_prevLightPositionBufferData.padding = -1.0f;
	m_prevLightBufferData.padding.x = -1.0f;
	m_clampSampler = nullptr;
	m_wrapSampler = nullptr;
	m_bias = 0.001f;
} // ShadowShader

ShadowShader::~ShadowShader() {
	m_clampSampler = nullptr;
	m_wrapSampler = nullptr;
} // ~ShadowShader

bool ShadowShader::Init(const InitParams& params) {
	if (!params.device || !params.hwnd || !params.clampSampler || !params.wrapSampler) {
		return false;
	}

	m_clampSampler = params.clampSampler;
	m_wrapSampler = params.wrapSampler;

	//m_matrixBufferData.world = XMMatrixTranspose(params.worldMatrix);
	//m_matrixBufferData.view = XMMatrixTranspose(params.viewMatrix);
	//m_matrixBufferData.projection = XMMatrixTranspose(params.projectionMatrix);
	//m_matrixBufferData.lightView = XMMatrixTranspose(params.lightViewMatrix);
	//m_matrixBufferData.lightProjection = XMMatrixTranspose(params.lightProjectionMatrix);

	//m_lightPositionBufferData.lightPosition = params.lightPosition;

	//m_lightBufferData.ambientColor = params.ambientColor;
	//m_lightBufferData.diffuseColor = params.diffuseColor;
	//m_lightBufferData.bias = 0.001f;

	if (!InitShader(params.device, params.hwnd)) {
		return false;
	}

	return true;
} // Init

bool ShadowShader::Render(ID3D11DeviceContext* context, const RenderParams& params) {
	if (!context || !params.depthMapTexture) return false;

	if (!UpdateMatrixBuffer(context, params.worldMatrix, params.viewMatrix, params.projectionMatrix, params.lightViewMatrix, params.lightProjectionMatrix)) {
		return false;
	}
	if (!UpdateLightPositionBuffer(context, params.lightPosition)) {
		return false;
	}
	
	if (!UpdateLightBuffer(context, params.ambientColor, params.diffuseColor, m_bias)) {
		return false;
	}

	context->VSSetConstantBuffers(BUFFER_SLOT_MAT, 1, m_matrixBuffer.GetAddressOf());
	context->VSSetConstantBuffers(BUFFER_SLOT_LIGHTPOS, 1, m_lightPositionBuffer.GetAddressOf());
	context->PSSetConstantBuffers(BUFFER_SLOT_LIGHT, 1, m_lightBuffer.GetAddressOf());

	context->IASetInputLayout(m_layout.Get());
	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

	context->PSSetShaderResources(TEXTURE_DEPH_SLOT, 1, &params.depthMapTexture);

	context->PSSetSamplers(SAMPLER_CLAMP_SLOT, 1, &m_clampSampler);
	context->PSSetSamplers(SAMPLER_WRAP_SLOT, 1, &m_wrapSampler);
	return true;
} // Render

bool ShadowShader::InitShader(ID3D11Device* device, HWND hwnd) {
	using namespace ShaderHelper;
	using namespace ConstantBuffer;

	D3D11_INPUT_ELEMENT_DESC layoutDesc[3];
	layoutDesc[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	layoutDesc[1] = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	layoutDesc[2] = { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };

	if (!InitVertexShader(device, hwnd, PathConstants::SHADOW_VS,
		layoutDesc, ARRAYSIZE(layoutDesc), m_vertexShader.GetAddressOf(), m_layout.GetAddressOf())) {
		return false;
	}

	if (!InitPixelShader(device, hwnd, PathConstants::SHADOW_PS, m_pixelShader.GetAddressOf())) {
		return false;
	}

	if (!InitConstantBuffer<MatrixBuffer>(device, m_matrixBuffer.GetAddressOf()) ||
		!InitConstantBuffer<LightPositionBuffer>(device, m_lightPositionBuffer.GetAddressOf()) ||
		!InitConstantBuffer<LightBuffer>(device, m_lightBuffer.GetAddressOf())) {
		return false;
	}

	return true;
} // InitShader

bool ShadowShader::UpdateMatrixBuffer(ID3D11DeviceContext* context,
	const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection,
	const XMMATRIX& lightView, const XMMATRIX& lightProjection) {
	using namespace ShaderHelper;

	MatrixBuffer data;

	data.world = XMMatrixTranspose(world);
	data.view = XMMatrixTranspose(view);
	data.projection = XMMatrixTranspose(projection);
	data.lightView = XMMatrixTranspose(lightView);
	data.lightProjection = XMMatrixTranspose(lightProjection);

	if (memcmp(&m_prevMatrixBufferData, &data, sizeof(MatrixBuffer)) == 0) {
		return true;
	}

	if (!UpdateConstantBuffer(context, m_matrixBuffer.Get(), data)) {
		return false;
	}

	m_prevMatrixBufferData = data;
	//m_matrixBufferData = data;
	return true;
} // UpdateMatrixBuffer

bool ShadowShader::UpdateLightPositionBuffer(ID3D11DeviceContext* context, const XMFLOAT3& lightPosition) {
	using namespace ShaderHelper;

	LightPositionBuffer data;
	data.lightPosition = lightPosition;

	if (memcmp(&m_prevLightPositionBufferData, &data, sizeof(LightPositionBuffer)) == 0) {
		return true;
	}

	if (!UpdateConstantBuffer(context, m_lightPositionBuffer.Get(), data)) {
		return false;
	}

	m_prevLightPositionBufferData = data;
	//m_lightPositionBufferData = data;
	return true;
} // UpdateLightPositionBuffer

bool ShadowShader::UpdateLightBuffer(ID3D11DeviceContext* context, const XMFLOAT4& ambientColor, const XMFLOAT4& diffuseColor, const float& bias) {
	using namespace ShaderHelper;

	LightBuffer data;

	data.ambientColor = ambientColor;
	data.diffuseColor = diffuseColor;
	data.bias = bias;

	if (memcmp(&m_prevLightBufferData, &data, sizeof(LightBuffer)) == 0) {
		return true;
	}

	if (!UpdateConstantBuffer(context, m_lightBuffer.Get(), data)) {
		return false;
	}

	m_prevLightBufferData = data;
	//m_lightBufferData = data;

	return true;
} // UpdateLightBuffer