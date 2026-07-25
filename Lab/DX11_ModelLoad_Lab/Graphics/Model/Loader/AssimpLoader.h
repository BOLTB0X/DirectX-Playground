// Graphics/Loader/AssimpLoader.h
#pragma once
#include <d3d11.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <memory>
// Graphics
#include "Model/Mesh/VertexTypes.h"

class Mesh;
class MeshModel;
class TerrainModel;
class TexturesManager;
class Texture;

class AssimpLoader {
public:
    AssimpLoader();
    AssimpLoader(const AssimpLoader& other) = delete;
    ~AssimpLoader();

    bool LoadMeshModel(ID3D11Device*, ID3D11DeviceContext*,
        TexturesManager*, const std::string& path, MeshModel*);

private:
    void ProcessNode(aiNode*, const aiScene*, ID3D11Device*, MeshModel*);

    std::unique_ptr<Mesh> ProcessMesh(
        aiMesh*,
        const aiScene*,
        ID3D11Device*);

    void ProcessMaterials(
        const aiScene*,
        ID3D11Device*,
        ID3D11DeviceContext*,
        TexturesManager*,
        const std::string&, MeshModel*);

    std::shared_ptr<Texture> LoadPBRTexture(
        ID3D11Device*,
        ID3D11DeviceContext*,
        TexturesManager*,
        const std::string&,
        const std::string&,
        const std::string&
    );
}; // AssimpLoader