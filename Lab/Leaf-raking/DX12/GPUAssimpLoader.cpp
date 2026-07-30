#include "Pch.h"
#include "GPUAssimpLoader.h"
// Data
#include "Texture.h"
// Managers
#include "TextureManager.h"
// Utils
#include "DebugHelper.h"
// STL
#include <filesystem>

using namespace DebugHelper;
using namespace DirectX;

bool GPUAssimpLoader::Init(std::shared_ptr<TextureManager> texMgr, const std::string& path) {
    m_TextureManager = texMgr;

    m_globalVertices.clear();
    m_globalIndices.clear();
    m_meshDataList.clear();
    m_materials.clear();

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate | aiProcess_ConvertToLeftHanded |
        aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals);

    if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode) {
        DebugPrint(std::string("GPUAssimpLoader 로드 실패: ") + importer.GetErrorString());
        return false;
    }

    std::string directory = std::filesystem::path(path).parent_path().string();

    ProcessNode(scene->mRootNode, scene, XMMatrixIdentity());
    ProcessMaterials(scene, directory);

    return true;
} // Init

void GPUAssimpLoader::ProcessNode(aiNode* node, const aiScene* scene, const XMMATRIX& parentTransform) {
    XMMATRIX nodeTransform = ConvertMatrixToDirectX(node->mTransformation);
    XMMATRIX worldTransform = XMMatrixMultiply(nodeTransform, parentTransform);

    for (UINT i = 0; i < node->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessMesh(mesh, scene, worldTransform);
    }

    for (UINT i = 0; i < node->mNumChildren; ++i) {
        ProcessNode(node->mChildren[i], scene, worldTransform);
    }
} // ProcessNode

void GPUAssimpLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene, const XMMATRIX& worldTransform) {
    // 현재 글로벌 버퍼의 위치가 이 메시의 오프셋
    uint32_t vertexOffset = static_cast<uint32_t>(m_globalVertices.size());
    uint32_t indexOffset = static_cast<uint32_t>(m_globalIndices.size());
    uint32_t indexCount = static_cast<uint32_t>(mesh->mNumFaces * 3);

    XMVECTOR det;
    XMMATRIX worldInvTranspose = XMMatrixTranspose(XMMatrixInverse(&det, worldTransform));

    for (UINT i = 0; i < mesh->mNumVertices; ++i) {
        GPUModel::PBRVertex v;
        
        // Position
        XMVECTOR pos = XMVectorSet(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1.0f);
        pos = XMVector3Transform(pos, worldTransform);
        XMStoreFloat3(&v.position, pos);

        // Normal
        if (mesh->HasNormals()) {
            XMVECTOR normal = XMVectorSet(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z, 0.0f);
            normal = XMVector3Normalize(XMVector3TransformNormal(normal, worldInvTranspose));
            XMStoreFloat3(&v.normal, normal);
        }

        // Texture UV
        if (mesh->mTextureCoords[0]) {
            v.texture = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
        }

        // Tangent & Bitangent
        if (mesh->HasTangentsAndBitangents()) {
            XMVECTOR tangent = XMVectorSet(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z, 0.0f);
            tangent = XMVector3Normalize(XMVector3TransformNormal(tangent, worldInvTranspose));
            XMStoreFloat3(&v.tangent, tangent);

            XMVECTOR bitangent = XMVectorSet(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z, 0.0f);
            bitangent = XMVector3Normalize(XMVector3TransformNormal(bitangent, worldInvTranspose));
            XMStoreFloat3(&v.binormal, bitangent);
        }

        m_globalVertices.push_back(v);
    }

    for (UINT i = 0; i < mesh->mNumFaces; ++i) {
        const aiFace& face = mesh->mFaces[i];
        for (UINT j = 0; j < face.mNumIndices; ++j) {
            m_globalIndices.push_back(face.mIndices[j]);
        }
    }

    SharedCommons::GPUMeshData meshData;
    meshData.vertexOffset = vertexOffset;
    meshData.indexOffset = indexOffset;
    meshData.indexCount = indexCount;
    meshData.materialIndex = mesh->mMaterialIndex;

    m_meshDataList.push_back(meshData);
} // ProcessMesh

void GPUAssimpLoader::ProcessMaterials(const aiScene* scene, const std::string& directory) {
    for (UINT i = 0; i < scene->mNumMaterials; ++i) {
        aiMaterial* aiMat = scene->mMaterials[i];
        GPUMaterial material;

        aiString name;
        aiMat->Get(AI_MATKEY_NAME, name);
        material.name = name.C_Str();

        material.albedo = LoadMaterialElement(aiMat, directory, aiTextureType_DIFFUSE, SharedCommons::PBRTextureType::Albedo);
        if (!material.albedo) {
            material.albedo = m_TextureManager->GetTexture(SharedCommons::KEY_DUMMEY_WHITE);
        }
        aiColor4D baseColor(SharedCommons::ALBEDO_FACTOR.x, SharedCommons::ALBEDO_FACTOR.y,
            SharedCommons::ALBEDO_FACTOR.z, SharedCommons::ALBEDO_FACTOR.w);
        aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, baseColor);
        material.albedoFactor = { baseColor.r, baseColor.g, baseColor.b, baseColor.a };

        material.normal = LoadMaterialElement(aiMat, directory, aiTextureType_NORMALS, SharedCommons::PBRTextureType::Normal);
        if (!material.normal) {
            material.normal = m_TextureManager->GetTexture(SharedCommons::KEY_DUMMEY_NORMAL);
        }

        material.alpha = LoadMaterialElement(aiMat, directory, aiTextureType_OPACITY, SharedCommons::PBRTextureType::Alpha);
        if (!material.alpha) {
            material.alpha = m_TextureManager->GetTexture(SharedCommons::KEY_DUMMEY_WHITE);
        }

        material.roughness = LoadMaterialElement(aiMat, directory, aiTextureType_DIFFUSE_ROUGHNESS, SharedCommons::PBRTextureType::Roughness);
        if (!material.roughness) {
            float rgh = 0.8f;
            if (aiMat->Get(AI_MATKEY_ROUGHNESS_FACTOR, rgh) != AI_SUCCESS) {
                material.roughnessFactor = SharedCommons::ROUGH_FACTOR;
            }
            else {
                material.roughnessFactor = rgh;
            }
        }
        else {
            material.roughnessFactor = 1.0f;
        }

        // Metallic (MTL PBR Extension: map_Pm)
        material.metallic = LoadMaterialElement(aiMat, directory, aiTextureType_METALNESS, SharedCommons::PBRTextureType::Metallic);
        if (!material.metallic) {
            float met = 0.0f;
            if (aiMat->Get(AI_MATKEY_METALLIC_FACTOR, met) != AI_SUCCESS) {
                material.metallicFactor = 0.0f;
            }
            else {
                material.metallicFactor = met;
            }
        }
        else {
            material.metallicFactor = 1.0f;
        }

        m_materials.push_back(material);
    }
} // ProcessMaterials

std::shared_ptr<Texture> GPUAssimpLoader::LoadMaterialElement(
    aiMaterial* material, const std::string& directory, aiTextureType type,
    SharedCommons::PBRTextureType /*pbrType*/) {
    if (material->GetTextureCount(type) == 0) {
        return nullptr;
    }
    aiString texPath;
    if (material->GetTexture(type, 0, &texPath) != AI_SUCCESS) {
        return nullptr;
    }

    std::string fullPath = directory + "/" + texPath.C_Str();
    return m_TextureManager->GetTexture(fullPath);
} // LoadMaterialElement

XMMATRIX GPUAssimpLoader::ConvertMatrixToDirectX(const aiMatrix4x4& m) {
    return XMMatrixSet(
        m.a1, m.b1, m.c1, m.d1,
        m.a2, m.b2, m.c2, m.d2,
        m.a3, m.b3, m.c3, m.d3,
        m.a4, m.b4, m.c4, m.d4);
} // ConvertMatrixToDirectX

const std::vector<GPUModel::PBRVertex>&          GPUAssimpLoader::GetGlobalVertices() const { return m_globalVertices; }
const std::vector<unsigned int>&                 GPUAssimpLoader::GetGlobalIndices() const { return m_globalIndices; }
const std::vector<SharedCommons::GPUMeshData>&   GPUAssimpLoader::GetMeshDataList() const { return m_meshDataList; }
const std::vector<GPUAssimpLoader::GPUMaterial>& GPUAssimpLoader::GetMaterials() const { return m_materials; }