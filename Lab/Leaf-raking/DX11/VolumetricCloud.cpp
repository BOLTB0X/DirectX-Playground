#include "Pch.h"
#include "VolumetricCloud.h"
#include "VolumeTexture.h"
#include "NoiseGenerator.h"
#include "Texture.h"
#include "TextureManager.h"
// Utils
#include "Helpers/DebugHelper.h"
#include "Helpers/ShaderHelper.h"
#include "SharedConstants/PathConstants.h"
#include "SharedConstants/BuffersConstants.h"

#define MESH_SCALE  1
#define CAMERA_SLOT 0
#define LIGHT_SLOT  1
#define CLOUD_SLOT  2
#define VOLUME_TEX_SLOT    0
#define WEATHER_MAP_SLOT   1
#define SAMPLER_WRAP_SLOT  0
#define SAMPLER_CLAMP_SLOT 1

using namespace DirectX;
using namespace SharedConstants;
using namespace PathConstants;
using namespace ConstantBuffer;
using namespace BuffersConstants;
using namespace DebugHelper;
using namespace ShaderHelper;


VolumetricCloud::VolumetricCloud() {
	m_mesh = std::make_unique<DefaultMesh>();
	m_VolumeTexture = std::make_unique<VolumeTexture>();
	m_NoiseGenerator = std::make_unique<NoiseGenerator>();
	m_textureMgr = nullptr;
	m_wSampler = nullptr;
	m_cSampler = nullptr;
	m_CloudData = CloudBuffer();
	m_CameraVSData = CameraBuffer();
	m_CameraPSData = CameraBuffer();
	m_LightData = LightBuffer();
	m_renderCount = 0.0f;
} // VolumetricCloud

VolumetricCloud::~VolumetricCloud() {
	m_textureMgr = nullptr;
	m_wSampler = nullptr;
	m_cSampler = nullptr;
} // ~VolumetricCloud

bool VolumetricCloud::Init(ID3D11Device* device, ID3D11DeviceContext* context, HWND hwnd,
	ID3D11SamplerState* wrap, ID3D11SamplerState* clamp, TextureManager* texMgr) {

	if (!m_mesh->Init(device, MESH_SCALE, DefaultMesh::DefaultMeshType::Quad)) {
		return false;
	}

	if (!m_VolumeTexture->Init(device, (UINT)TEXTURE_SIZE.x, (UINT)TEXTURE_SIZE.y, (UINT)TEXTURE_SIZE.z, DXGI_FORMAT_R8G8B8A8_UNORM)) {
		return false;
	}

	if (!m_NoiseGenerator->Init(device, hwnd, NOISEGEN_CS)) {
		return false;
	}

	if (!InitShader(device, hwnd)) {
		return false;
	}

	m_textureMgr = texMgr;
	m_WeatherMap = m_textureMgr->GetTexture(device, context, WMAP);
	if (!m_WeatherMap) {
		DebugHelper::DebugPrint("WeatherMap 로드 실패: " + WMAP);
		return false;
	}
	m_wSampler = wrap;
	m_cSampler = clamp;
	GenerateCloudNoise(context);

	return true;
} // Init

void VolumetricCloud::Render(ID3D11DeviceContext* context, const RenderParams& params) {
	m_renderCount += 0.1f;
	if (RenderShader(context, params)) {
		m_mesh->RenderBuffer(context);
		context->DrawIndexed(m_mesh->GetIndexCount(), 0, 0);
	} else {
		DebugHelper::DebugPrint("VolumetricCloud Shader Render 실패!");
	}
} // Render

bool VolumetricCloud::InitShader(ID3D11Device* device, HWND hwnd) {
	using namespace ShaderHelper;
	using namespace ConstantBuffer;
	using namespace PathConstants;

	D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	if (!InitVertexShader(device, hwnd, VOL_CLOUD_VS,
		layoutDesc, ARRAYSIZE(layoutDesc), m_vertexShader.GetAddressOf(), m_layout.GetAddressOf())) {
		return false;
	}
	if (!InitPixelShader(device, hwnd, VOL_CLOUD_PS, m_pixelShader.GetAddressOf())) {
		return false;
	}

	if (!InitConstantBuffer<CameraBuffer>(device, m_cameraVSBuffer.GetAddressOf())) {
		return false;
	}

	if (!InitConstantBuffer<CameraBuffer>(device, m_cameraPSBuffer.GetAddressOf())) {
		return false;
	}

	if (!InitConstantBuffer<LightBuffer>(device, m_lightBuffer.GetAddressOf())) {
		return false;
	}

	if (!InitConstantBuffer<CloudBuffer>(device, m_cloudBuffer.GetAddressOf())) {
		return false;
	}

	return true;
} // InitShader

bool VolumetricCloud::UpdateCameraBuffer(ID3D11DeviceContext* context, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj, const DirectX::XMFLOAT3& camPos) {
	XMVECTOR det;

	m_CameraVSData.viewInv = XMMatrixTranspose(XMMatrixInverse(&det, view));
	m_CameraVSData.projInv = XMMatrixTranspose(XMMatrixInverse(&det, proj));
	m_CameraVSData.cameraPosition = camPos;

	m_CameraPSData.viewInv = XMMatrixTranspose(XMMatrixInverse(&det, view));
	m_CameraPSData.projInv = XMMatrixTranspose(XMMatrixInverse(&det, proj));
	m_CameraPSData.cameraPosition = camPos;

	if (memcmp(&m_prevCameraPSData, &m_CameraPSData, sizeof(CameraBuffer)) == 0
		&& memcmp(&m_prevCameraVSData, &m_CameraVSData, sizeof(CameraBuffer)) == 0) {
		return true;
	}

	if (!UpdateConstantBuffer(context, m_cameraVSBuffer.Get(), m_CameraVSData)) {
		return false;
	}
	
	if (!UpdateConstantBuffer(context, m_cameraPSBuffer.Get(), m_CameraPSData)) {
		return false;
	}

	m_prevCameraVSData = m_CameraVSData;
	m_prevCameraPSData = m_CameraPSData;

	return true;
} // UpdateCameraBuffer

bool VolumetricCloud::UpdateLightBuffer(ID3D11DeviceContext* context, const DirectX::XMFLOAT4& diffuse, const DirectX::XMFLOAT3& lightDir) {
	m_LightData.diffuseColor = diffuse;
	m_LightData.lightDirection = lightDir;

	if (memcmp(&m_prevLightData, &m_LightData, sizeof(CloudBuffer)) == 0) {
		return true;
	}

	if (!UpdateConstantBuffer(context, m_lightBuffer.Get(), m_LightData)) {
		return false;
	}

	m_prevLightData = m_LightData;
	return true;
} // UpdateLightBuffer

bool VolumetricCloud::UpdateCloudBuffer(ID3D11DeviceContext* context,
	const XMFLOAT3& planetCenter, const float& planetRadius,
	const float& cloudMinHeight, const float& cloudMaxHeight) {
	m_CloudData.planetCenter = planetCenter;
	m_CloudData.planetRadius = planetRadius;
	m_CloudData.cloudMinHeight = cloudMinHeight;
	m_CloudData.cloudMaxHeight = cloudMaxHeight;
	m_CloudData.time += m_renderCount;

	if (memcmp(&m_prevCloudData, &m_CloudData, sizeof(CloudBuffer)) == 0) {
		return true;
	}

	if (!UpdateConstantBuffer(context, m_cloudBuffer.Get(), m_CloudData)) {
		return false;
	}
	m_prevCloudData = m_CloudData;
	return true;
} // UpdateCloudBuffer

bool VolumetricCloud::RenderShader(ID3D11DeviceContext* context, const RenderParams& params) {
	if (!UpdateCameraBuffer(context, params.view, params.projection, params.camPos)) {
		DebugHelper::DebugPrint("VolumetricCloud: UpdateCameraBuffer 실패!");
		return false;
	}

	if (!UpdateLightBuffer(context, params.diffuse, params.LightDir)) {
		DebugHelper::DebugPrint("VolumetricCloud: UpdateLightBuffer 실패!");
		return false;
	}

	if (!UpdateCloudBuffer(context, params.planetCenter, params.planetRadius, params.cloudMinHeight, params.cloudMaxHeight)) {
		DebugHelper::DebugPrint("VolumetricCloud: UpdateCloudBuffer 실패!");
		return false;
	}

	context->VSSetConstantBuffers(CAMERA_SLOT, 1, m_cameraVSBuffer.GetAddressOf());
	context->PSSetConstantBuffers(CAMERA_SLOT, 1, m_cameraPSBuffer.GetAddressOf());
	context->PSSetConstantBuffers(LIGHT_SLOT, 1, m_lightBuffer.GetAddressOf());
	context->PSSetConstantBuffers(CLOUD_SLOT, 1, m_cloudBuffer.GetAddressOf());
	context->IASetInputLayout(m_layout.Get());

	ID3D11ShaderResourceView* vSrv = m_VolumeTexture->GetSRV();
	ID3D11ShaderResourceView* wSrv = m_WeatherMap->GetSRV();

	context->PSSetShaderResources(VOLUME_TEX_SLOT, 1, &vSrv);
	context->PSSetShaderResources(WEATHER_MAP_SLOT, 1, &wSrv);
	context->PSSetSamplers(SAMPLER_WRAP_SLOT, 1, &m_wSampler);
	context->PSSetSamplers(SAMPLER_CLAMP_SLOT, 1, &m_cSampler);

	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
	return true;
} // RenderShader

void VolumetricCloud::GenerateCloudNoise(ID3D11DeviceContext* context) {
	if (!m_NoiseGenerator || !m_VolumeTexture) return;

	NoiseBuffer noiseParams;
	noiseParams.textureSize = TEXTURE_SIZE;
	noiseParams.perlinFreq = PERLIM_FREQ;
	noiseParams.worleyFreq = WORLEY_FREQ;
	noiseParams.detailFreqG = VOL_FREQ_G;
	noiseParams.detailFreqB = VOL_FREQ_B;
	noiseParams.detailFreqA = VOL_FREQ_A;
	noiseParams.octaves = VOL_OCTAVES;
	noiseParams.remapBias = VOL_REMAP_BIAS;

	m_NoiseGenerator->Generate(context, m_VolumeTexture.get(), noiseParams);
	DebugPrint("노이즈 굽기 완료");
} // GenerateCloudNoise