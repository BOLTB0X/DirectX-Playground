// Graphics/MeshModel/Texture/Material.h
#pragma once
#include "Texture.h"

#include <string>
#include <memory>
#include <DirectXMath.h>

enum class MaterialType : int
{
    BASE = 0,
    CLIFF = 1,
    CLOUD = 2,
    STONE = 3
}; // MaterialType


struct Material {
    std::string name;
    MaterialType type = MaterialType::BASE;

    std::shared_ptr<Texture> volumeNoise;
    std::shared_ptr<Texture> diffuse; // 확산
    std::shared_ptr<Texture> ambient; // 주변광 (그늘/환경)
    std::shared_ptr<Texture> specular; // 반사/투사 (반짝임)
    std::shared_ptr<Texture> albedo;
    std::shared_ptr<Texture> normal; // 노멀 (표면 굴곡)
    std::shared_ptr<Texture> metallic;
    std::shared_ptr<Texture> roughness;
    std::shared_ptr<Texture> ao;
    std::shared_ptr<Texture> alpha;
}; // Material


struct MaterialBuffer
{
    int type;
    float gTime;
    DirectX::XMFLOAT4 gMountPos;
    DirectX::XMFLOAT4 gStonePos;
    DirectX::XMFLOAT2 padding;
}; // MaterialBuffer


struct CloudBuffer {
    float time;
    DirectX::XMFLOAT3 padding; // 16바이트 정렬
}; // CloudBuffer