#include "MountPick.h"
// Graphics
#include "Model/MeshModel.h"
#include "Model/HeightMap/HeightMap.h"
#include "Shader/ActorsShader.h"
#include "Camera/Camera.h"
// Common
#include "EngineHelper.h"
// Framework
#include "Position/Position.h"

/* default */
///////////////////////////////////////////////////////////////////////////////////

MountPick::MountPick()
    : ActorObject(),
    m_heightMap(nullptr)
{
    m_heightOffset = 3.0f;
}

MountPick::~MountPick()
{
    Shutdown();
}

bool MountPick::Init(MeshModel* model, ActorsShader* shader, const std::string& name)
{
    if (ActorObject::Init(model, shader, name)
        == false) return false;

    std::vector<ModelVertex> v;
    std::vector<unsigned int> i;

    model->GetPhysicsData(v, i);
    if (v.empty())
    {
        EngineHelper::DebugPrint(" MountPick::Init: 지형 물리 데이터(정점)가 empty");
    }

    m_heightMap = std::make_unique<HeightMap>();
    if (m_heightMap->Init(v, i) == false) return false;


    return true;
} // MountPick


void MountPick::Shutdown()
{
    ActorObject::Shutdown();
} // Shutdown


void MountPick::Frame(float frameTime)
{
    ActorObject::Frame(frameTime);
} // Frame


void MountPick::Render(ID3D11DeviceContext* context, Camera* camera)
{
    ActorObject::Render(context, camera);
} // Render

///////////////////////////////////////////////////////////////////////////////////

/* public */
///////////////////////////////////////////////////////////////////////////////////


float MountPick::GetHeightAtMesh(float x, float z) const
{
    if (m_heightMap == nullptr) return 0.0f;
    return m_heightMap->GetHeightAtMesh(x, z);
} // GetHeightAtMesh


float MountPick::GetHeightAtWorld(float worldX, float worldZ) const
{
    if (m_heightMap == nullptr) return 0.0f;

    DirectX::XMFLOAT3 myPos = actor_Position->GetPosition();
    float localX = worldX - myPos.x;
    float localZ = worldZ - myPos.z;

    // 로컬 검사 후 자신의 월드 Y와 Scale 적용
    return (m_heightMap->GetHeightAtMesh(localX, localZ) * actor_Position->GetScale().x) + myPos.y;
} // GetHeightAtMesh