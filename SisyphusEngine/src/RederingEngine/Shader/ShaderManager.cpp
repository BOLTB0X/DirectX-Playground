#include "Pch.h"
#include "ShaderManager.h"
#include "CloudShader.h"
#include "BicubicShader.h"
#include "SkyShader.h"
#include "LensFlareShader.h"
#include "WaterShader.h"
#include "RefractionShader.h"
// Framework
#include "Shader.h"
// Common
#include "ConstantHelper.h"

using namespace DirectX;


ShaderManager::ShaderManager() {}


ShaderManager::~ShaderManager()
{
    Shutdown();
} // ShaderManager


bool ShaderManager::Init(ID3D11Device* device, HWND hwnd)
{
    auto cloudShader = std::make_unique<CloudShader>();
    if (cloudShader->Init(device, hwnd,
        ConstantHelper::DEFAULT_VS,
        ConstantHelper::CLOUD_PS)
        == false) return false;
    m_shaders[ShaderKeys::Cloud] = std::move(cloudShader);

    auto biShader = std::make_unique<BicubicShader>();
    if (biShader->Init(device, hwnd,
        ConstantHelper::QUAD_VS,
        ConstantHelper::BICUBIC_PS)
        == false) return false;

    m_shaders[ShaderKeys::Bicubic] = std::move(biShader);

    auto skyShader = std::make_unique<SkyShader>();
    if (skyShader->Init(device, hwnd,
        ConstantHelper::SKY_VS,
        ConstantHelper::SKY_PS)
        == false) return false;

    m_shaders[ShaderKeys::Sky] = std::move(skyShader);

    auto lfShader = std::make_unique<LensFlareShader>();
    if (lfShader->Init(device, hwnd,
        ConstantHelper::QUAD_VS,
        ConstantHelper::LENSFLARE_PS) == false) return false;

    m_shaders[ShaderKeys::LensFlare] = std::move(lfShader);

    auto waterShder = std::make_unique<WaterShader>();
    if (waterShder->Init(device, hwnd,
        ConstantHelper::WATER_VS,
        ConstantHelper::WATER_PS) == false) return false;
    m_shaders[ShaderKeys::Water] = std::move(waterShder);

	auto refractionShader = std::make_unique<RefractionShader>();

    return true;
} // Init


void ShaderManager::Shutdown()
{
    m_shaders.clear();
} // Shutdown


void ShaderManager::UpdateMatrixBuffer(const std::string key, ID3D11DeviceContext* context,
    XMMATRIX model, XMMATRIX view, XMMATRIX proj)
{
    auto it = m_shaders.find(key);
    if (it == m_shaders.end())
        return;
    it->second->UpdateMatrixBuffer(context, model, view, proj);
} // UpdateMatrixBuffer


void ShaderManager::UpdateGlobalBuffer(
    const std::string key,
    ID3D11DeviceContext* context,
    float time, float frame, XMFLOAT3 cameraPos)
{
    auto it = m_shaders.find(key);
    if (it == m_shaders.end())
        return;
    it->second->UpdateGlobalBuffer(context, time, frame, cameraPos);
} // UpdateGlobalBuffer


void ShaderManager::UpdateLightBuffer(const std::string key,
    ID3D11DeviceContext* context, Light* light, DirectX::XMFLOAT2 uv)
{
    auto it = m_shaders.find(key);
    if (it == m_shaders.end()) return;

    Shader* shader = it->second.get();
    if (shader->GetShaderType() == ShaderType::Sky)
        static_cast<SkyShader*>(shader)->UpdateLightBuffer(context, light, uv);
} // UpdateLightBuffer


void ShaderManager::UpdateCloudBuffer(ID3D11DeviceContext* context, const CloudBuffer& data)
{
    auto it = m_shaders.find(ShaderKeys::Cloud);
    if (it != m_shaders.end())
    {
        static_cast<CloudShader*>(it->second.get())->UpdateCloudBuffer(context, data);
    }
} // UpdateCloudBuffer


void ShaderManager::UpdateSkyBuffer(ID3D11DeviceContext* context, const SkyBuffer& data)
{
    auto it = m_shaders.find(ShaderKeys::Sky);
    if (it != m_shaders.end())
    {
        static_cast<SkyShader*>(it->second.get())->UpdateSkyBuffer(context, data);
    }
} // UpdateSkyBuffer


void ShaderManager::UpdateLensFlareBuffer(ID3D11DeviceContext* context, const LensFlareBuffer& data)
{
    auto* shader = GetShader<LensFlareShader>(ShaderKeys::LensFlare);
    if (shader)
    {
		shader->UpdateLensFlareBuffer(context, data);
    }
} // UpdateLensFlareBuffer


void ShaderManager::UpdateWaterBuffer(ID3D11DeviceContext* context, const WaterBuffer& data)
{
    auto* shader = GetShader<WaterShader>(ShaderKeys::Water);

    if (shader)
    {
        shader->UpdateWaterBuffer(context, data);
    }
} // UpdateOceanBuffer


void ShaderManager::UpdateWaterReflectionMatrix(ID3D11DeviceContext* context, const DirectX::XMMATRIX& reflectionMatrix)
{
    auto waterShader = GetShader<WaterShader>(ShaderKeys::Water);
    if (waterShader)
    {
        waterShader->UpdateReflectionBuffer(context, reflectionMatrix);
    }
} // UpdateWaterReflectionMatrix


void ShaderManager::UpdateRefractionBuffer(ID3D11DeviceContext* context, const RefractionBuffer& data)
{
    auto refractionShader = GetShader<RefractionShader>(ShaderKeys::Refraction);
    if (refractionShader)
    {
        refractionShader->UpdateRefractionBuffer(context, data);
    }
} // UpdateRefractionBuffer


void ShaderManager::SetShaders(const std::string key, ID3D11DeviceContext* context)
{
    auto it = m_shaders.find(key);
    if (it == m_shaders.end()) return;

    Shader* shader = it->second.get();
    shader->SetShaders(context);
} //SetShaders


void ShaderManager::SetConstantBuffers(const std::string key,
    ID3D11DeviceContext* context)
{
    auto it = m_shaders.find(key);
    if (it == m_shaders.end()) return;

    Shader* shader = it->second.get();
    ShaderType type = shader->GetShaderType();

    ID3D11Buffer* buffer = GetShader<SkyShader>(ShaderKeys::Sky)->GetLightBuffer();
    if (type == ShaderType::Cloud)
    {
        if (buffer == nullptr) return;
        static_cast<CloudShader*>(shader)->SetConstantBuffers(context, buffer);
    }
    else if (type == ShaderType::Sky)
    {
        static_cast<SkyShader*>(shader)->SetConstantBuffers(context);
    }
    else if (type == ShaderType::LensFlare)
    {
        //ID3D11Buffer* buffer = GetShader<SkyShader>(ShaderKeys::Sky)->GetLightBuffer();
        if (buffer == nullptr) return;
        static_cast<LensFlareShader*>(shader)->SetConstantBuffers(context, buffer);
    }
    else if (type == ShaderType::Water)
    {
        //ID3D11Buffer* buffer = GetShader<SkyShader>(ShaderKeys::Sky)->GetLightBuffer();
        if (buffer == nullptr) return;
        static_cast<WaterShader*>(shader)->SetConstantBuffers(context, buffer);
    }
    else if (key == ShaderKeys::Refraction)
    {
        if (buffer == nullptr) return;
        static_cast<RefractionShader*>(it->second.get())->SetConstantBuffers(context, buffer);
    }
} // SetConstantBuffers