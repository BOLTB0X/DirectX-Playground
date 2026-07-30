#pragma once
#include <d3d12.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
// STL
#include <string>
#include <memory>
#include <vector>
#include <DirectXMath.h>
// Data
#include "GPUModel.h"
// Utils
#include "SharedCommons.h"

class Texture;
class TextureManager;

class GPUAssimpLoader {
public:
    struct GPUMaterial {
        std::string              name;
        std::shared_ptr<Texture> albedo;
        std::shared_ptr<Texture> normal;
        std::shared_ptr<Texture> metallic;
        std::shared_ptr<Texture> roughness;
        std::shared_ptr<Texture> alpha;
        DirectX::XMFLOAT4        albedoFactor;
        float                    metallicFactor;
        float                    roughnessFactor;
        float                    emissiveFactor;
        float                    alphaFactor;

        GPUMaterial() : name(""), albedoFactor(SharedCommons::ALBEDO_FACTOR),
            metallicFactor(SharedCommons::METALLIC_FACTOR), roughnessFactor(SharedCommons::ROUGH_FACTOR),
            emissiveFactor(SharedCommons::EMISS_FACTOR), alphaFactor(SharedCommons::ALPHA_FACTOR) {
        }
    }; // GPUMaterial

public:
    GPUAssimpLoader() = default;
    GPUAssimpLoader(const GPUAssimpLoader&) = delete;
    GPUAssimpLoader& operator=(const GPUAssimpLoader&) = delete;
    ~GPUAssimpLoader() = default;

    bool Init(std::shared_ptr<TextureManager>, const std::string&);

public:
    const std::vector<GPUModel::PBRVertex>&        GetGlobalVertices() const;
    const std::vector<unsigned int>&               GetGlobalIndices() const;
    const std::vector<SharedCommons::GPUMeshData>& GetMeshDataList() const;
    const std::vector<GPUMaterial>&                GetMaterials() const;

private:
    void                     ProcessNode(aiNode*, const aiScene*, const DirectX::XMMATRIX&);
    void                     ProcessMesh(aiMesh*, const aiScene*, const DirectX::XMMATRIX&);
    void                     ProcessMaterials(const aiScene*, const std::string&);
    std::shared_ptr<Texture> LoadMaterialElement(aiMaterial*, const std::string&, aiTextureType, SharedCommons::PBRTextureType);
    DirectX::XMMATRIX        ConvertMatrixToDirectX(const aiMatrix4x4&);

private:
    std::shared_ptr<TextureManager>         m_TextureManager;

    // GPU Driven을 위한 거대한 단일 버퍼 배열들
    std::vector<GPUModel::PBRVertex>        m_globalVertices;
    std::vector<unsigned int>               m_globalIndices;
    std::vector<SharedCommons::GPUMeshData> m_meshDataList;
    std::vector<GPUMaterial>                m_materials;
}; // GPUAssimpLoader