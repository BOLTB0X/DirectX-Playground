// Application/Manager/ModelManager.h
#pragma once
#include <d3d11.h>
#include <unordered_map>
#include <string>
#include <memory>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class MeshModel;
class TerrainModel;
class TexturesManager;
class Mesh;
class Texture;
class AssimpLoader;

class ModelManager {
public:
    ModelManager();
	ModelManager(const ModelManager&) = delete;
    ~ModelManager();

    bool Init(ID3D11Device*, ID3D11DeviceContext*, TexturesManager*);
    void Shutdown();

public:

    MeshModel* GetMeshModel(ID3D11Device*,
        ID3D11DeviceContext*,
        TexturesManager*,
        const std::string&);

    TerrainModel* ModelManager::GetTerrainModel(
        ID3D11Device* device,
        ID3D11DeviceContext*,
        TexturesManager*,
        const std::string&);
   
private:
    std::unique_ptr<AssimpLoader> m_AssimpLoader;
    std::unordered_map<std::string, std::unique_ptr<MeshModel>> m_modelLibrary;
    std::unordered_map<std::string, std::unique_ptr<TerrainModel>> m_terrainLibrary;
}; // ModelManager