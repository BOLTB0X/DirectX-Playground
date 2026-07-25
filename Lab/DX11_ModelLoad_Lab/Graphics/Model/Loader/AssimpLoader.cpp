// Graphics/Model/Loader/AssimpLoader.cpp
#include "AssimpLoader.h"
#include "Manager/TexturesManager.h"
// Common
#include "EngineHelper.h"
#include "EngineSettings.h"
// Graphics
#include "Model/MeshModel.h"
#include "Model/TerrainModel.h"
#include "Model/Mesh/Mesh.h"
#include "Model/Texture/Material.h"
#include "Model/Texture/Texture.h"
// etc
#include <filesystem>


/* default */
//////////////////////////////////////////////////////////////////////////////

AssimpLoader::AssimpLoader() {}


AssimpLoader::~AssimpLoader() {}


bool AssimpLoader::LoadMeshModel(
    ID3D11Device* device, ID3D11DeviceContext* context,
    TexturesManager* texManager, const std::string& path,
    MeshModel* outModel)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate | aiProcess_CalcTangentSpace
        | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices | aiProcess_ImproveCacheLocality
        | aiProcess_ConvertToLeftHanded);

    if (scene == nullptr)
    {
        EngineHelper::SuccessCheck(false, "Assimp Scene 로드 실패: " + path);
        return false;
    }

    if (scene->mRootNode == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
    {
        EngineHelper::SuccessCheck(false, "Assimp 데이터 불완전: " + path);
        return false;
    }

    size_t lastSlash = path.find_last_of("\\/");
    std::string directory = (lastSlash == std::string::npos) ? "." : path.substr(0, lastSlash);

    ProcessMaterials(scene, device, context, texManager, directory, outModel);
    ProcessNode(scene->mRootNode, scene, device, outModel);

    return true;
} // LoadMeshModel

//////////////////////////////////////////////////////////////////////////

/* private */
//////////////////////////////////////////////////////////////////////////

void AssimpLoader::ProcessNode(
    aiNode* node, const aiScene* scene,
    ID3D11Device* device, MeshModel* outModel)
{
    // 현재 노드의 모든 메쉬 처리
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        outModel->AddMesh(ProcessMesh(mesh, scene, device));
    }

    // 자식 노드들도 처리
    for (unsigned int i = 0; i < node->mNumChildren; i++)
        ProcessNode(node->mChildren[i], scene, device, outModel);

    return;
} // ProcessNode


std::unique_ptr<Mesh> AssimpLoader::ProcessMesh(
    aiMesh* mesh, const aiScene* scene,
    ID3D11Device* device)
{
    std::vector<ModelVertex> vertices;
    std::vector<unsigned int> indices;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        ModelVertex vertex;
        vertex.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };

        if (mesh->mTextureCoords[0])
        {
            vertex.texture.x = (float)mesh->mTextureCoords[0][i].x;
            vertex.texture.y = (float)mesh->mTextureCoords[0][i].y;
        }
        else
            vertex.texture = DirectX::XMFLOAT2(0.0f, 0.0f);

        if (mesh->HasNormals())
            vertex.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

        if (mesh->HasTangentsAndBitangents())
        {
            vertex.tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
            vertex.binormal = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
        }
        else
        {
            vertex.tangent = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
            vertex.binormal = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
        }

        vertices.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    MeshData meshData;
    meshData.vertices = vertices;
    meshData.indices = indices;

    auto newMesh = std::make_unique<Mesh>();
    if (!newMesh->Init(device, meshData, mesh->mMaterialIndex))
        return nullptr;
    return newMesh;
} // ProcessMesh


void AssimpLoader::ProcessMaterials(
    const aiScene* scene, ID3D11Device* device,
    ID3D11DeviceContext* context, TexturesManager* texManager,
    const std::string& directory, MeshModel* outModel)
{

    std::string pbrDir = directory + "/textures/";
    size_t lastSlash = directory.find_last_of("\\/");
    std::string modelName = (lastSlash == std::string::npos) ? directory : directory.substr(lastSlash + 1);


    for (unsigned int i = 0; i < scene->mNumMaterials; i++)
    {
        aiMaterial* aiMat = scene->mMaterials[i];
        Material myMaterial;
        myMaterial.name = aiMat->GetName().C_Str();

        //// Assimp 로더
        //auto loadTex = [&](aiTextureType type) -> std::shared_ptr<Texture> {
        //    aiString path;
        //    if (aiMat->GetTexture(type, 0, &path) == AI_SUCCESS) {
        //        std::string fullPath = directory + "/" + path.C_Str();
        //        auto tex = texManager->GetTexture(device, context, fullPath);

        //        EngineHelper::SuccessCheck(tex != nullptr, "일반 텍스처 로드 실패: " + fullPath);
        //        return tex;
        //    }
        //    return nullptr;
        //};

        //myMaterial.diffuse = loadTex(aiTextureType_DIFFUSE);
        //myMaterial.ambient = loadTex(aiTextureType_AMBIENT);
        //myMaterial.specular = loadTex(aiTextureType_SPECULAR);
        //myMaterial.normal = loadTex(aiTextureType_NORMALS);

        std::string matName = myMaterial.name;
        std::transform(matName.begin(), matName.end(), matName.begin(), ::tolower); // 소문자 변환

        if (matName.find("cloud") != std::string::npos)
        {
            myMaterial.type = MaterialType::CLOUD;
            myMaterial.normal = texManager->GetTexture(device, context, EngineSettings::TERRAINNORM_PATH);

            if (myMaterial.normal == nullptr)
                EngineHelper::DebugPrint("Warning: Cloud Normal Map 로드 실패 -> " + EngineSettings::TERRAINNORM_PATH);
        }
        else if (matName.find("cliff") != std::string::npos || matName.find("ciff") != std::string::npos)
            myMaterial.type = MaterialType::CLIFF;
        else if (matName.find("stone") != std::string::npos)
            myMaterial.type = MaterialType::STONE;
        else
            myMaterial.type = MaterialType::BASE;

        auto getPBR = [&](const std::string& suffix) -> std::shared_ptr<Texture> {
            std::string fullPath = pbrDir + modelName + suffix;
            auto tex = texManager->GetTexture(device, context, fullPath);

            EngineHelper::SuccessCheck(tex != nullptr, "PBR 로드 실패: " + fullPath);
            return tex;
            };

        //myMaterial.albedo = getPBR("_BaseColor.png");
        //myMaterial.normal = getPBR("_normal.png");
        //myMaterial.metallic = getPBR("_Metallic.png");
        //myMaterial.roughness = getPBR("_Roughness.png");
        //myMaterial.ao = getPBR("_ao.png");
        myMaterial.albedo = LoadPBRTexture(device, context, texManager, pbrDir, modelName, EngineSettings::PBRT_SUFFIX_BaseColor);
        //myMaterial.normal = LoadPBRTexture(device, context, texManager, pbrDir, modelName, "_normal");
        if (myMaterial.normal == nullptr) {
            myMaterial.normal = LoadPBRTexture(device, context, texManager, pbrDir, modelName, EngineSettings::PBRT_SUFFIX_NORM);
        }
        myMaterial.metallic = LoadPBRTexture(device, context, texManager, pbrDir, modelName, EngineSettings::PBRT_SUFFIX_META);
        myMaterial.roughness = LoadPBRTexture(device, context, texManager, pbrDir, modelName, EngineSettings::PBRT_SUFFIX_ROUG);
        myMaterial.ao = LoadPBRTexture(device, context, texManager, pbrDir, modelName, EngineSettings::PBRT_SUFFIX_AO);
        myMaterial.alpha = LoadPBRTexture(device, context, texManager, pbrDir, modelName, EngineSettings::PBRT_SUFFIX_ALPHA);

        outModel->AddMaterial(myMaterial);

    } // for

} // ProcessMaterials

std::shared_ptr<Texture> AssimpLoader::LoadPBRTexture(
    ID3D11Device* device,
    ID3D11DeviceContext* context,
    TexturesManager* texManager,
    const std::string& pbrDir,
    const std::string& modelName,
    const std::string& suffix)
{
    static const std::vector<std::string> extensions = { ".png", ".jpg", ".jpeg", ".tga", ".dds" };

    std::string basePath = pbrDir + modelName + suffix;

    for (const auto& ext : extensions)
    {
        std::string fullPath = basePath + ext;

        if (std::filesystem::exists(fullPath))
        {
            auto tex = texManager->GetTexture(device, context, fullPath);
            if (tex) return tex;
        }
    }

    return nullptr;
} // LoadPBRTexture
