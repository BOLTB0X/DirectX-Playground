// World/Terrain/CloudTerrain.cpp
#include "CloudOcean.h"
// Graphics
#include "Model/TerrainModel.h"
#include "Model/HeightMap/HeightMap.h"
#include "Camera/Camera.h"
#include "Shader/TerrainShader.h"
#include "Shader/CloudShader.h"
#include "Renderer/BlendState.h"
#include "Renderer/DepthStencilState.h"
// Common
#include "EngineHelper.h"
#include "MathHelper.h"
// Framework
#include "Position/Position.h"

/* default */
////////////////////////////////////////////////////////////////////


CloudOcean::CloudOcean()
    : m_modelPtr(nullptr),
    m_shaderPtr(nullptr),
    m_name(""),
    m_totalTime(0.0f)
{
} // CloudOcean


CloudOcean::~CloudOcean()
{
    Shutdown();
} // ~CloudOcean


bool CloudOcean::Init(TerrainModel* model, TerrainShader* shader, const std::string& name)
{
    if (model == nullptr || shader == nullptr) return false;

    m_modelPtr = model;
    m_shaderPtr = shader;
    m_name = name;

    m_Position = std::make_unique<Position>();
    if (m_Position)
    {
        m_Position->SetPosition(0.0f, 0.0f, 0.0f);
        m_Position->SetRotation(0.0f, 0.0f, 0.0f);
        m_Position->SetScale(1.0f);
    }


    EngineHelper::DebugPrint("CloudOcean: " + m_name + " 초기화 성공");
    return true;
} // Init


void CloudOcean::Render(ID3D11DeviceContext* context, Camera* camera)
{
    if (m_modelPtr == nullptr || m_shaderPtr == nullptr || camera == nullptr) return;

    DirectX::XMMATRIX worldMatrix = m_Position->GetWorldMatrix();
    m_shaderPtr->UpdateMatrixBuffer(
        context,
        worldMatrix,
        camera->GetViewMatrix(),
        camera->GetProjectionMatrix()
    );

    m_shaderPtr->Bind(context);
    m_modelPtr->Render(context, m_shaderPtr, camera->GetFrustum(), worldMatrix);
    return;
} // Render


void CloudOcean::Shutdown()
{
    m_modelPtr = nullptr;
    m_shaderPtr = nullptr;

    if (m_Position)
    {
        m_Position.reset();
    }
} // Shutdown


void CloudOcean::Frame(float frameTime)
{
    m_totalTime += frameTime;
    m_modelPtr->SetTime(m_totalTime);
} // Frame


////////////////////////////////////////////////////////////////////


float CloudOcean::GetHeight(float x, float z) const
{
    if (m_modelPtr && m_modelPtr->GetHeightMap())
    {
        return m_modelPtr->GetHeightMap()->GetHeightAtMesh(x, z);
    }
    return 0.0f;
} // GetHeight


float CloudOcean::GetHeightAtWorld(float worldX, float worldZ) const
{
    return MathHelper::GetFBMHeight(worldX, worldZ, m_totalTime);
} // GetHeightAtWorld