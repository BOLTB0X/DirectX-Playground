#pragma once
#include <d3d11.h>
#include <unordered_map>
#include <string>
#include <memory>
#include <directxmath.h>
#include "ShaderBuffers.h"

class Shader;
class Light;


namespace ShaderKeys {
    const std::string Cloud = "Cloud";
    const std::string CloudArea = "CloudArea";
    const std::string Bicubic = "Bicubic";
    const std::string Sky = "Sky";
	const std::string LensFlare = "LensFlare";
    const std::string Water = "Water";
	const std::string Refraction = "Refraction";
    const std::string VolumetricCloud = "VolumetricCloud";
} // ShaderKeys


class ShaderManager {
public: // default
    ShaderManager();
    ShaderManager(const ShaderManager&) = delete;
    ~ShaderManager();

    bool Init(ID3D11Device* device, HWND hwnd);
    void Shutdown();

public: // template
    template<typename T>
    bool LoadShader(ID3D11Device* device, HWND hwnd,
        const std::wstring& vsPath, const std::wstring& psPath,
        const std::string& key)
    {
        if (m_shaders.find(key) != m_shaders.end()) return true;
        auto newShader = std::make_unique<T>();

        if (newShader->Init(device, hwnd, vsPath, psPath) == false)
            return false;
        m_shaders[key] = std::move(newShader);
        return true;
    }


    template<typename T>
    T* GetShader(const std::string& key)
    {
        auto it = m_shaders.find(key);
        if (it != m_shaders.end())
            return dynamic_cast<T*>(it->second.get());
        return nullptr;
    } // GetShader


public:
    void UpdateMatrixBuffer(const std::string, ID3D11DeviceContext*,
        DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX);
    void UpdateGlobalBuffer(const std::string, ID3D11DeviceContext*,
        float, float, DirectX::XMFLOAT3, DirectX::XMMATRIX, float, float);
    void UpdateLightBuffer(const std::string, ID3D11DeviceContext*,
        Light*, DirectX::XMFLOAT2);
    void UpdateCloudBuffer(ID3D11DeviceContext*, const CloudBuffer&);
    void UpdateSkyBuffer(ID3D11DeviceContext*, const SkyBuffer&);
    void UpdateVolumetricCloudsBuffer(ID3D11DeviceContext*, const VolumetricCloudsBuffer&);
    void UpdateLensFlareBuffer(ID3D11DeviceContext*, const LensFlareBuffer&);
    void UpdateWaterBuffer(ID3D11DeviceContext*, const WaterBuffer&);
    void UpdateWaterReflectionMatrix(ID3D11DeviceContext*, const DirectX::XMMATRIX&);
    void UpdateRefractionBuffer(ID3D11DeviceContext*, const RefractionBuffer&);

    void SetShaders(const std::string, ID3D11DeviceContext*);
    void SetConstantBuffers(const std::string, ID3D11DeviceContext*);

private:
    std::unordered_map<std::string, std::unique_ptr<Shader>> m_shaders;

}; // ShaderManager