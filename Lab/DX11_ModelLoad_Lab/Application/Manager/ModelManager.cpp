// Application/Manager/ModelManager.cpp
#include "ModelManager.h"
#include "TexturesManager.h"
// Common
#include "EngineHelper.h"
#include "EngineSettings.h"
// Graphics
#include "Model/MeshModel.h"
#include "Model/TerrainModel.h"
#include "Model/Mesh/Mesh.h"
#include "Model/Mesh/VertexTypes.h"
#include "Model/Texture/Material.h"
#include "Model/Texture/Texture.h"
#include "Model/Loader/AssimpLoader.h"
// etc
#include <filesystem>

/* default */
///////////////////////////////////////////////////////////////////////////

ModelManager::ModelManager()
{
    m_AssimpLoader = std::make_unique<AssimpLoader>();
};

ModelManager::~ModelManager()
{
    Shutdown();
};


bool ModelManager::Init(ID3D11Device* device,
    ID3D11DeviceContext* context,
    TexturesManager* texManager)
{
    if (GetTerrainModel(device, context, texManager, EngineSettings::TERRAIN_PATH))
        EngineHelper::SuccessCheck(true, "TERRAIN 모델 로드 성공");
    else
    {
        EngineHelper::SuccessCheck(false, "TERRAIN 모델 로드 실패");
        return false;
    }

    if (GetMeshModel(device, context, texManager, EngineSettings::MOUNTAIN_PATH))
    {
        EngineHelper::SuccessCheck(true, "MountPick 모델 로드 성공");
    }
    else
    {
        EngineHelper::SuccessCheck(false, "MountPick 모델 로드 실패: " + EngineSettings::STONE_PATH);
        return false;
    }

    if (GetMeshModel(device, context, texManager, EngineSettings::STONE_PATH))
        EngineHelper::SuccessCheck(true, "Stone 모델 로드 성공");
    else
    {
        EngineHelper::SuccessCheck(false, "Stone 모델 로드 실패: " + EngineSettings::STONE_PATH);
        return false;
    }

	return true;
} // Init


void ModelManager::Shutdown()
{
	m_modelLibrary.clear();
    m_terrainLibrary.clear();
} // Shutdown

/////////////////////////////////////////////////////////////////////////

/* public */
//////////////////////////////////////////////////////////////////////////

MeshModel* ModelManager::GetMeshModel(
    ID3D11Device* device,
    ID3D11DeviceContext* context,
    TexturesManager* texManager,
    const std::string& path)
{
    if (m_modelLibrary.find(path) != m_modelLibrary.end())
        return m_modelLibrary[path].get();

    auto newModel = std::make_unique<MeshModel>();
    if (newModel->InitConstantBuffer(device) == false) return nullptr;

    if (m_AssimpLoader->LoadMeshModel(
        device, context, 
        texManager, path, 
        newModel.get()))
    {
        m_modelLibrary[path] = std::move(newModel);
        return m_modelLibrary[path].get();
    }

    return nullptr;
} // GetMeshModel


TerrainModel* ModelManager::GetTerrainModel(
    ID3D11Device* device,
    ID3D11DeviceContext* context,
    TexturesManager* texManager,
    const std::string& path)
{
    if (m_terrainLibrary.find(path) != m_terrainLibrary.end())
        return m_terrainLibrary[path].get();

    auto newTerrainModel = std::make_unique<TerrainModel>();

    if (newTerrainModel->InitConstantBuffer(device) == false) return nullptr;
    newTerrainModel->InitHeightMap();

    Material terrainMat;
    terrainMat.name = "TerrainMaterial";
    terrainMat.volumeNoise = texManager->GetTexture(device, context, "assets/Cloud/cloud_blue_noise.dds");
    //terrainMat.normal = texManager->GetTexture(device, context, EngineSettings::TERRAINNORM_PATH);
    newTerrainModel->InitMaterial(terrainMat);

    if (newTerrainModel->CreateCells(device, 42) == false)
    {
        EngineHelper::DebugPrint("ModelManager::GetTerrainModel: CreateCells 실패");
        return nullptr;
    }

    EngineHelper::DebugPrint("TerrainModel 조립 완료: " + path);
    m_terrainLibrary[path] = std::move(newTerrainModel);
    return m_terrainLibrary[path].get();
} // GetTerrainModel


//////////////////////////////////////////////////////////////////////////
