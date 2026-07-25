// Application/TexturesManager/TexturesManager.h
#pragma once
#include <d3d11.h>
#include <unordered_map>
#include <string>
#include <memory>

class Texture;

class TexturesManager {
public:
    TexturesManager();
	TexturesManager(const TexturesManager&) = delete;
    ~TexturesManager();

    bool Init();
    void Shutdown();
    std::shared_ptr<Texture> GetTexture(ID3D11Device*, ID3D11DeviceContext*, const std::string&);

private:
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_textures;
}; // TexturesManager