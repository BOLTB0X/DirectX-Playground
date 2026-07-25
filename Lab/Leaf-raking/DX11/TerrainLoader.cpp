// Graphics/Model/Loader/TerrainLoader.cpp
#include "TerrainLoader.h"
#include "Model/TerrainModel.h"
#include "Model/Texture/Material.h"
#include "Model/HeightMap/HeightMap.h"
// Application
#include "Manager/TexturesManager.h"
// Common
#include "EngineHelper.h"
#include "EngineSettings.h"
// etc
#include <fstream>
#include <iostream>

/* default */
//////////////////////////////////////////////////////////////////////////////////////

TerrainLoader::TerrainLoader() {}


TerrainLoader::~TerrainLoader() {}


bool TerrainLoader::SetupTerrain(
    ID3D11Device* device, ID3D11DeviceContext* context,
    TexturesManager* texManager,
    TerrainModel* outModel)
{
    //TerrainInfo info;
    int w = EngineSettings::terrainWidth, h = EngineSettings::terrainHeight;

    //outModel->SetDimensions(w, h);
    auto heightMap = std::make_unique<HeightMap>();
    //heightMap->InitPlane(w, h);
    outModel->SetHeightMap(std::move(heightMap)); // Loader가 HeightMap을 소유권을 넘겨줌

    Material terrainMat;
    terrainMat.name = "TerrainMaterial";
    terrainMat.normal = texManager->GetTexture(device, context, EngineSettings::TERRAINNORM_PATH);

    outModel->AddMaterial(terrainMat);

    return true;
} // SetupTerrain

//////////////////////////////////////////////////////////////////////////////////////


/* private */
//////////////////////////////////////////////////////////////////////////////////////

//bool TerrainLoader::LoadSetupFile(const std::string& path, TerrainInfo& info)
//{
//    std::ifstream file(path);
//    if (!file.is_open()) return false;
//
//    std::string line;
//    while (std::getline(file, line))
//    {
//        if (line.empty()) continue;
//
//        size_t delimiterPos = line.find(':'); // ':'을 기준으로 분리
//        if (delimiterPos == std::string::npos) continue;
//
//        std::string key = line.substr(0, delimiterPos);
//        std::string value = line.substr(delimiterPos + 1);
//
//        // 앞뒤 공백 제거
//        value.erase(0, value.find_first_not_of(" \t\r\n"));
//        value.erase(value.find_last_not_of(" \t\r\n") + 1);
//
//        if (key == "Terrain Filename") info.heightMapFilename = value;
//        else if (key == "Terrain Height") info.terrainHeight = std::stoi(value);
//        else if (key == "Terrain Width")  info.terrainWidth = std::stoi(value);
//        else if (key == "Terrain Scaling") info.terrainScaling = std::stof(value);
//    }
//
//    file.close();
//    return true;
//} // LoadSetupFile


//bool TerrainLoader::LoadRawHeightMap(
//    const TerrainInfo& info,
//    std::vector<DirectX::XMFLOAT3>& outPositions)
//{
//    int size = info.terrainWidth * info.terrainHeight;
//    outPositions.resize(size);
//
//    std::ifstream file(info.heightMapFilename, std::ios::binary);
//    if (!file) return false;
//
//    std::vector<unsigned short> rawImage(size);
//    file.read(reinterpret_cast<char*>(rawImage.data()), size * sizeof(unsigned short));
//    file.close();
//
//    for (unsigned int j = 0; j < info.terrainHeight; j++)
//    {
//        for (unsigned int i = 0; i < info.terrainWidth; i++)
//        {
//            unsigned int index = (info.terrainWidth * j) + i;
//
//            outPositions[index].x = (float)i;
//            outPositions[index].y = (float)rawImage[index] / info.terrainScaling;
//            // Z축 좌표 조정
//            outPositions[index].z = (float)(info.terrainHeight - 1) - (float)j;
//        }
//    }
//    return true;
//} // LoadRawHeightMap


//bool TerrainLoader::CalculateNormals(const TerrainInfo& info, std::vector<ModelVertex>& vertices)
//{
//    unsigned int width = info.terrainWidth;
//    unsigned int height = info.terrainHeight;
//
//    // 모든 면의 법선 계산용 임시 배열
//    int faceCount = (width - 1) * (height - 1);
//    std::vector<DirectX::XMFLOAT3> faceNormals(faceCount);
//
//    for (unsigned int j = 0; j < (height - 1); j++)
//    {
//        for (unsigned int i = 0; i < (width - 1); i++)
//        {
//            int i1 = (j * width) + i; // 좌상
//            int i2 = (j * width) + (i + 1); // 우상
//            int i3 = ((j + 1) * width) + i; // 좌하
//
//            DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&vertices[i1].position);
//            DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(&vertices[i2].position);
//            DirectX::XMVECTOR v3 = DirectX::XMLoadFloat3(&vertices[i3].position);
//
//            DirectX::XMVECTOR edge1 = DirectX::XMVectorSubtract(v1, v3);
//            DirectX::XMVECTOR edge2 = DirectX::XMVectorSubtract(v3, v2);
//            DirectX::XMVECTOR normal = DirectX::XMVector3Cross(edge1, edge2);
//
//            DirectX::XMStoreFloat3(&faceNormals[(j * (width - 1)) + i], DirectX::XMVector3Normalize(normal));
//        }
//    } // for - 버텍스 
//
//    // 버텍스별로 인접한 면의 법선 합산
//    for (unsigned int j = 0; j < height; j++)
//    {
//        for (unsigned int i = 0; i < width; i++)
//        {
//            DirectX::XMVECTOR sum = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
//
//            // 인접한 4개의 면 법선을 합산
//            if ((i - 1 >= 0) && (j - 1 >= 0))
//                sum = DirectX::XMVectorAdd(sum, DirectX::XMLoadFloat3(&faceNormals[((j - 1) * (width - 1)) + (i - 1)]));
//
//            if ((i < width - 1) && (j - 1 >= 0))
//                sum = DirectX::XMVectorAdd(sum, DirectX::XMLoadFloat3(&faceNormals[((j - 1) * (width - 1)) + i]));
//
//            if ((i - 1 >= 0) && (j < height - 1))
//                sum = DirectX::XMVectorAdd(sum, DirectX::XMLoadFloat3(&faceNormals[(j * (width - 1)) + (i - 1)]));
//
//            if ((i < width - 1) && (j < height - 1))
//                sum = DirectX::XMVectorAdd(sum, DirectX::XMLoadFloat3(&faceNormals[(j * (width - 1)) + i]));
//
//            DirectX::XMStoreFloat3(&vertices[(j * width) + i].normal, DirectX::XMVector3Normalize(sum));
//        }
//    } // for
//
//    return true;
//} // CalculateNormals

