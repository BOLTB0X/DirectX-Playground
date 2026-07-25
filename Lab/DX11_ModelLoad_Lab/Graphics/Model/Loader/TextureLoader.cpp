// Model/Loader/TextureLoader.cpp
#define STB_IMAGE_IMPLEMENTATION
#include "TextureLoader.h"
// 외부
#include "stb_image.h"
#include <DDSTextureLoader.h>
// Common
#include "EngineHelper.h"
// etc
#include <algorithm>

bool TextureLoader::CreateTextureFromFile(
    ID3D11Device* device,
    ID3D11DeviceContext* context,
    const std::string& filename,
    ID3D11ShaderResourceView** outSRV)
{
    std::string ext = filename.substr(filename.find_last_of(".") + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == "dds")
    {
        return LoadUsingDDSTex(device, context, filename, outSRV);
    }

    return LoadUsingStb(device, context, filename, outSRV);
}

/* private */
//////////////////////////////////////////////////////////////////////////////////////////////////////////

bool TextureLoader::LoadUsingDDSTex(
    ID3D11Device* device,
    ID3D11DeviceContext* context,
    const std::string& filename,
    ID3D11ShaderResourceView** outSRV)
{
    std::wstring wFilename(filename.begin(), filename.end());
    HRESULT hr = DirectX::CreateDDSTextureFromFile(device, context, wFilename.c_str(), nullptr, outSRV);

    if (FAILED(hr))
    {
        EngineHelper::DebugPrint("TextureLoader:: DDS 로드 실패: " + filename);
        return false;
    }

    return true;
} // TextureLoader


bool TextureLoader::LoadUsingStb(
    ID3D11Device* device,
    ID3D11DeviceContext* context,
    const std::string& filename,
    ID3D11ShaderResourceView** outSRV)
{
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* imageData = stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb_alpha);

    if (imageData == nullptr)
    {
        EngineHelper::DebugPrint("텍스처 파일 로드 실패: " + filename);
        return false;
    }

    // Texture Desc 설정
    D3D11_TEXTURE2D_DESC td = {};
    td.Width = width;
    td.Height = height;
    td.MipLevels = 0; // 자동 밉맵 생성
    td.ArraySize = 1;
    td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    td.SampleDesc.Count = 1;
    td.Usage = D3D11_USAGE_DEFAULT;
    td.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    td.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    HRESULT hr = device->CreateTexture2D(&td, nullptr, texture.GetAddressOf());
    if (FAILED(hr)) { stbi_image_free(imageData); return false; }

    // 데이터 업로드 및 밉맵 생성
    unsigned int rowPitch = (width * 4) * sizeof(unsigned char);
    context->UpdateSubresource(texture.Get(), 0, NULL, imageData, rowPitch, 0);

    D3D11_SHADER_RESOURCE_VIEW_DESC srvd = {};
    srvd.Format = td.Format;
    srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvd.Texture2D.MipLevels = -1;

    hr = device->CreateShaderResourceView(texture.Get(), &srvd, outSRV);
    if (SUCCEEDED(hr)) context->GenerateMips(*outSRV);

    stbi_image_free(imageData);
    return SUCCEEDED(hr);
} // LoadUsingStb