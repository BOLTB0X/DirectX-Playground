// Application/ShaderManager/ShaderManager.h
#pragma once
#include <d3d11.h>
#include <unordered_map>
#include <string>
#include <memory>
#include "Shader/CloudShader.h"

class Shader;

class ShaderManager {
public:
    ShaderManager();
    ShaderManager(const ShaderManager&) = delete;
    ~ShaderManager();

    bool Init(ID3D11Device* device, HWND hwnd);
    void Shutdown();

public:
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


    CloudShader* GetCloudShader() { return GetShader<CloudShader>("Cloud"); }

private:
    std::unordered_map<std::string, std::unique_ptr<Shader>> m_shaders;
}; // ShaderManager