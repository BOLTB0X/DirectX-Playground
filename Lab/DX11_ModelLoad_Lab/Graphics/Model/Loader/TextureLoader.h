// Model/Loader/TextureLoader.h
#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <string>

class TextureLoader {
public:

    static bool CreateTextureFromFile(
        ID3D11Device*,
        ID3D11DeviceContext*,
        const std::string&,
        ID3D11ShaderResourceView**);

private:

    static bool LoadUsingDDSTex(ID3D11Device*, ID3D11DeviceContext*,
        const std::string&, ID3D11ShaderResourceView**);

    static bool LoadUsingStb(ID3D11Device*, ID3D11DeviceContext*,
        const std::string&, ID3D11ShaderResourceView**);

}; // TextureLoader
