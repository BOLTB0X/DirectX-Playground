// Application/TexturesManager/TexturesManager.h
#include "TexturesManager.h"
// Graphics
#include "Model/Texture/Texture.h"
#include "Model/Loader/TextureLoader.h"

/* default */
///////////////////////////////////////////////////////////////////////////

TexturesManager::TexturesManager() {}


TexturesManager::~TexturesManager()
{
    Shutdown();
}


bool TexturesManager::Init()
{
    m_textures = std::unordered_map<std::string, std::shared_ptr<Texture>>();
    return true;
}


void TexturesManager::Shutdown()
{
    m_textures.clear();
} // Shutdown


std::shared_ptr<Texture> TexturesManager::GetTexture(
    ID3D11Device* device,
    ID3D11DeviceContext* context,
    const std::string& filename)
{
    auto it = m_textures.find(filename);
    if (it != m_textures.end())
        return it->second;

    auto newTexture = std::make_shared<Texture>();
    if (newTexture->Init(device, context, filename))
    {
        m_textures[filename] = newTexture;
        return newTexture;
    }

    return nullptr;
} // GetTexture