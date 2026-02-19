#include "Pch.h"
#include "TexturesManager.h"
#include "Texture.h"
// comonm
#include "DebugHelper.h"
#include "ConstantHelper.h"


using namespace ConstantHelper;
using namespace DebugHelper;


TexturesManager::TexturesManager()
{
    m_Textures = std::unordered_map<std::string, std::shared_ptr<Texture>>();
} // TexturesManager


TexturesManager::~TexturesManager()
{
    Shutdown();
} // ~TexturesManager


bool TexturesManager::Init(ID3D11Device* device, ID3D11DeviceContext* context)
{
    if (GetTexture(device, context, NOISE_PATH) == nullptr)
    {
        DebugPrint("TexturesManager::Init 실패: " + NOISE_PATH);
        return false;
    }

    if (GetTexture(device, context, BLUE_NOISE_PATH) == nullptr)
    {
        DebugPrint("TexturesManager::Init 실패: " + BLUE_NOISE_PATH);
        return false;
    }

    if (GetTexture(device, context, WATER_PATH) == nullptr)
    {
        DebugPrint("TexturesManager::Init 실패: " + WATER_PATH);
        return false;
	}

    return true;
} // Init


void TexturesManager::Shutdown()
{
    m_Textures.clear();
} // Shutdown


std::shared_ptr<Texture> TexturesManager::GetTexture(
    ID3D11Device* device,
    ID3D11DeviceContext* context,
    const std::string& filename)
{
    auto it = m_Textures.find(filename);
    if (it != m_Textures.end())
        return it->second;

    auto newTexture = std::make_shared<Texture>();
    if (newTexture->Init(device, context, filename))
    {
        m_Textures[filename] = newTexture;
        return newTexture;
    }

    return nullptr;
} // GetTexture


void TexturesManager::PSSetShaderResources(ID3D11DeviceContext* context, const std::string& filename, UINT slot)
{
    auto it = m_Textures.find(filename);
    if (it != m_Textures.end() && it->second != nullptr)
        it->second->PSSetShaderResources(context, slot);
    else
        DebugPrint("TexturesManager::PSSetShaderResources 실패: " + filename + " 이 로드 X");
    
} // PSSetShaderResources


void TexturesManager::PSSetShaderResources(ID3D11DeviceContext* context, const std::string& filename, UINT slot, UINT size)
{
    auto it = m_Textures.find(filename);
    if (it != m_Textures.end() && it->second != nullptr)
        it->second->PSSetShaderResources(context, slot, size);
    else
		DebugPrint("TexturesManager::PSSetShaderResources 실패: " + filename + " 이 로드 X");
} // PSSetShaderResources