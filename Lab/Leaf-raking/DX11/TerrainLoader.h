// Graphics/Model/Loader/TerrainLoader.h
#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <string>
#include <vector>
// Graphics
#include "Model/Mesh/VertexTypes.h"
#include "EngineSettings.h"

class TerrainModel;
class TexturesManager;

class TerrainLoader {
public:
    TerrainLoader();
    TerrainLoader(const TerrainLoader& other) = delete;
    ~TerrainLoader();

    bool SetupTerrain(ID3D11Device*, ID3D11DeviceContext*, TexturesManager*, TerrainModel*);

//public:
//    struct TerrainInfo {
//        int terrainHeight = EngineSettings::terrainHeight;
//        int terrainWidth = EngineSettings::terrainWidth;
//        float terrainScaling = EngineSettings::terrainScaling;
//    };
//
//private:
//    bool LoadSetupFile(const std::string& path, TerrainInfo& info);
//
//    bool LoadRawHeightMap(const TerrainInfo& info, std::vector<DirectX::XMFLOAT3>& outPositions);
//    bool CalculateNormals(const TerrainInfo& info, std::vector<ModelVertex>& vertices);
}; // TerrainLoader