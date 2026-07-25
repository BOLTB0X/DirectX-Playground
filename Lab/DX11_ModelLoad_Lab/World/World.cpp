// World/World.cpp
#include "World.h"
#include "Stone/Stone.h"
#include "Terrain/CloudOcean.h"
#include "MountPick/MountPick.h"
//#include "Physics/CollisionManager.h"
// Common
#include "EngineSettings.h"
#include "EngineHelper.h"
#include "MathHelper.h"
// Application
#include "Manager/ModelManager.h"
#include "Manager/ShaderManager.h"
#include "Manager/TexturesManager.h"
/// Framework
#include "Shader/Shader.h"
#include "Position/Position.h"
#include "Actor/ActorObject.h"
/// Graphics
#include "Renderer/Renderer.h"
#include "Camera/Camera.h"
#include "Shader/TerrainShader.h"
#include "Shader/ActorsShader.h"
#include "Shader/CloudShader.h"
// UserInterface
#include "UserInput/UserInput.h"
// etc
#include <string>


/* default */
/////////////////////////////////////////////////////////////////

World::World()
    : m_Actors(),
    m_Camera(nullptr),
    m_DeviceContext(nullptr),
    actorsShaderPtr(nullptr),
    terrainShaderPtr(nullptr),
    cloudShaderPtr(nullptr)
{
} // World


World::~World()
{
    Shutdown();
} // ~World


bool World::Init(const WorldInitParam& p)
{
    m_Camera = std::make_unique<Camera>();
    m_Camera->InitProjection(
        EngineSettings::SCREEN_WIDTH,
        EngineSettings::SCREEN_HEIGHT,
        EngineSettings::SCREEN_NEAR,
        EngineSettings::SCREEN_DEPTH
    );

    if (p.context == nullptr) return false;
    m_DeviceContext = p.context;

    TerrainModel* cloudModel = p.modelManager->GetTerrainModel(p.device, p.context, p.textureManager, EngineSettings::TERRAIN_PATH);
    terrainShaderPtr = p.shaderManager->GetShader<TerrainShader>("Terrain");

    //cloudShaderPtr = p.shaderManager->GetShader<CloudShader>("Cloud");

    if (cloudModel && terrainShaderPtr)
    {
        m_CloudOcean = std::make_unique<CloudOcean>();
        m_CloudOcean->Init(cloudModel, terrainShaderPtr, "CloudOcean");
        m_CloudOcean->GetPosition()->SetPosition(0.0f, 0.0f, 0.0f);
    }

    actorsShaderPtr = p.shaderManager->GetShader<ActorsShader>("Actors");
    MeshModel* stoneModel = p.modelManager->GetMeshModel(p.device, m_DeviceContext, p.textureManager, EngineSettings::STONE_PATH);
    MeshModel* mountainModel = p.modelManager->GetMeshModel(p.device, m_DeviceContext, p.textureManager, EngineSettings::MOUNTAIN_PATH);

    if (mountainModel && actorsShaderPtr)
    {
        auto mount = std::make_unique<MountPick>();
        mount->Init(mountainModel, actorsShaderPtr, "MountPick");
        mount->GetPosition()->SetPosition(EngineSettings::MOUNT_X, 0.0, EngineSettings::MOUNT_Z);
        mount->SetHeightOffset(0.0f);
        m_Actors.push_back(std::move(mount));
    }

    if (stoneModel && actorsShaderPtr)
    {
        auto stone = std::make_unique<Stone>();
        stone->Init(stoneModel, actorsShaderPtr, "Stone");
        stone->GetPosition()->SetPosition(EngineSettings::STONE_X, 0.0f, EngineSettings::STONE_Z);
        stone->SetHeightOffset(0.0f);
        m_Actors.push_back(std::move(stone));
    }

    if (m_CloudOcean)
    {
        for (auto& actor : m_Actors)
        {
            DirectX::XMFLOAT3 actorPos = actor->GetPosition()->GetPosition();
            float worldHeight = MathHelper::GetFBMHeight(actorPos.x, actorPos.z, m_CloudOcean->GetTotalTime());

            if (actor->GetName() == "MountPick")
            {
                actor->SetStatic(true);
                actor->SetFootingBias(10.0f);
            }
            else if (actor->GetName() == "Stone")
            {
                actor->SetStatic(false);
                actor->SetFootingBias(3.5f);
            }

            actor->PlaceOn(worldHeight);
        }

        m_Camera->GetPosition()->SetPosition(EngineSettings::STONE_X - 50.0f, 30.0f, EngineSettings::STONE_X - 50.0f);
        m_Camera->GetPosition()->SetRotation(30.0f, 45.0f, 0.0f);
    }

    //if (cloudShaderPtr)
    //{
    //    m_ParticleSystem = std::make_unique<ParticleSystem>();
    //    if (m_ParticleSystem->Init(p.device, 500) == false) return false;
    //}

    return true;
} // Init


void World::Shutdown()
{
    if (m_CloudOcean)
    {
        m_CloudOcean->Shutdown();
        m_CloudOcean.reset();
    }

    for (auto& actor : m_Actors)
    {
        if (actor) actor->Shutdown();
    }
    m_Actors.clear();

    actorsShaderPtr = nullptr;
    terrainShaderPtr = nullptr;
    m_DeviceContext = nullptr;

    m_Camera = nullptr;
} // Shutdown


void World::Frame(float frameTime, UserInput* userInput, bool canControlWorld)
{
    if (canControlWorld)
    {
        m_Camera->GetPosition()->SetFrameTime(frameTime);
        m_Camera->Render();
    }

    if (m_CloudOcean)
    {
        m_CloudOcean->Frame(frameTime);
    }


    for (auto& actor : m_Actors)
    {

        actor->Frame(frameTime);

        if (m_CloudOcean)
        {
            DirectX::XMFLOAT3 pos = actor->GetPosition()->GetPosition();
            float cloudHeight = MathHelper::GetFBMHeight(pos.x, pos.z, 0.0f);

            // 액터가 지형 높이에 맞춰 자신의 Y축 위치를 확정
            actor->PlaceOn(cloudHeight);
        }

        //UpdateCameraFollow(frameTime);
    }

} // Frame


void World::Render(ShaderManager* shaderManager, Renderer* renderer)
{
    if (m_Camera == nullptr) return;
    m_Camera->Render();

    if (m_CloudOcean)
    {

        DirectX::XMMATRIX viewMatrix = m_Camera->GetViewMatrix();
        DirectX::XMMATRIX projectionMatrix = m_Camera->GetProjectionMatrix();
        DirectX::XMMATRIX invViewProj = DirectX::XMMatrixInverse(nullptr, viewMatrix * projectionMatrix);
        DirectX::XMFLOAT3 camPos = m_Camera->GetPosition()->GetPosition();

        cloudShaderPtr->UpdateCloud(m_DeviceContext, m_CloudOcean->GetTotalTime(), camPos, invViewProj);

        m_CloudOcean->Render(m_DeviceContext, m_Camera.get());
        renderer->TurnOffAlphaBlending();
        renderer->TurnOnDepthWrite();
    }

    //renderer->TurnOffAlphaBlending();
    //renderer->TurnOnDepthWrite();

    for (auto& actor : m_Actors)
    {
        actor->Render(m_DeviceContext, m_Camera.get());
    }

} // Render

/////////////////////////////////////////////////////////////////


/* get */
/////////////////////////////////////////////////////////////////

const std::vector<std::unique_ptr<ActorObject>>& World::GetActors() const
{ 
    return m_Actors; 
} // GetActors


ActorObject* World::GetActor(size_t index) const
{
    if (index < m_Actors.size()) 
        return m_Actors[index].get();
    return nullptr;
} // GetActor


Camera* World::GetCamera() const
{
    return m_Camera.get();
} // GetCamera


Position* World::GetCameraPosition() const
{
    return m_Camera->GetPosition();
} // GetCameraPosition


/////////////////////////////////////////////////////////////////


void World::UpdateShaderMaterial(float time)
{
    if (terrainShaderPtr == nullptr) return;

    terrainShaderPtr->UpdateMaterialTag(
        m_DeviceContext,
        0,
        time,
        DirectX::XMFLOAT4(400.0f, 0.0f, 400.0f, 0.0f),
        DirectX::XMFLOAT4(300.0f, 0.0f, 300.0f, 0.0f)
    );
} // UpdateShaderMaterial


void World::UpdateCameraFollow(float frameTime)
{
    if (m_Camera == nullptr) return;

    ActorObject* stone = nullptr;
    for (auto& actor : m_Actors)
    {
        if (actor->GetName() == "Stone")
        {
            stone = actor.get();
            break;
        }
    }

    if (stone)
    {
        Position* camPos = m_Camera->GetPosition();
        Position* stonePos = stone->GetPosition();

        DirectX::XMFLOAT3 sPos = stonePos->GetPosition();
        DirectX::XMFLOAT3 sRot = stonePos->GetRotation(); // 돌의 회전이 필요할 경우

        // 3인칭 뷰 예시
        float distance = 60.0f;
        float height = 25.0f;

        // 카메라의 현재 회전값을 바탕으로 위치 계산 (마우스로 돌려볼 수 있게)
        float angleY = MathHelper::ToRadians(camPos->GetRotation().y);
        float angleX = MathHelper::ToRadians(camPos->GetRotation().x);

        DirectX::XMFLOAT3 newCamPos;
        newCamPos.x = sPos.x - sinf(angleY) * distance * cosf(angleX);
        newCamPos.y = sPos.y + height + sinf(angleX) * distance;
        newCamPos.z = sPos.z - cosf(angleY) * distance * cosf(angleX);

        camPos->SetPosition(newCamPos.x, newCamPos.y, newCamPos.z);
    }
} // UpdateCameraFollow