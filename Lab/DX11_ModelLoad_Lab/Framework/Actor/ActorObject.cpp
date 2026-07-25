// Framework/ActorObject.cpp
#include "ActorObject.h"
#include "Shader/Shader.h"
#include "Position/Position.h"
#include "Physics/Physics.h"
// Graphics
#include "Shader/ActorsShader.h"
#include "Model/MeshModel.h"
#include "Camera/Camera.h"
// Common
#include "EngineHelper.h"

/* default */
/////////////////////////////////////////////////////////////////

ActorObject::ActorObject()
    :actor_name(""),
    m_footingBias(0.0f),
    m_heightOffset(0.0f),
    m_isStatic(false),
    actor_Shader(nullptr),
    actor_Model(nullptr)
{
    actor_Position = std::make_unique<Position>();
    actor_Physics = std::make_unique<Physics>();
} // ActorObject


ActorObject::~ActorObject()
{
    Shutdown();
} // ~ActorObject


bool ActorObject::Init(MeshModel* model, ActorsShader* shader, const std::string& name)
{
    if (model == nullptr || shader == nullptr) return false;

    actor_Model = model;
    actor_name = name;
    actor_Shader = shader;

	actor_Position->SetPosition(0.0f, 0.0f, 0.0f);
	actor_Position->SetRotation(0.0f, 0.0f, 0.0f);
    actor_Position->SetScale(1.0f);

    return true;
} // Init

void ActorObject::Shutdown()
{
    if (actor_Shader) actor_Shader = nullptr;
    if (actor_Model)
    {
        actor_Model = nullptr;
	}


    actor_Position.reset();
    return;
} // Shutdown


void ActorObject::Frame(float frameTime)
{
    if (actor_Position)
    {
        actor_Position->SetFrameTime(frameTime);
    }

    if (actor_Physics)
    {
        actor_Physics->Frame(frameTime);
        ApplyDisplacement(frameTime);
        ApplyRotation(frameTime);
    }

    return;
} // Frame


void ActorObject::Render(ID3D11DeviceContext* context, Camera* camera)
{
    if (actor_Model == nullptr || actor_Shader == nullptr) return;
    if (EngineHelper::SuccessCheck(context, "ActorObject::Render -> context nullptr")
        == false) return;
    if (EngineHelper::SuccessCheck(camera, "ActorObject::Render -> camera nullptr")
        == false) return;

    if (camera == nullptr) return;

    DirectX::XMMATRIX worldMatrix = actor_Position->GetWorldMatrix();

    actor_Shader->UpdateMatrixBuffer(
        context,
        worldMatrix,
        camera->GetViewMatrix(),
        camera->GetProjectionMatrix()
    );

    actor_Shader->Bind(context);
    auto frustum = camera->GetFrustum();
    if (frustum)
    {
        actor_Model->Render(context, actor_Shader, frustum, worldMatrix);
    }
    else
    {
        actor_Model->Render(context, actor_Shader, nullptr, worldMatrix);
    }
    return;
} // Render


void ActorObject::PlaceOn(float WorldHeight)
{
    if (actor_Model == nullptr) return;

    DirectX::XMFLOAT3 currentPos = actor_Position->GetPosition();
    float offset = actor_Model->GetBottomOffset();
    float floorY = WorldHeight + offset - m_footingBias;

    if (currentPos.y <= floorY)
    {
        currentPos.y = floorY;

        // 바닥에 닿았으니 수직 속도는 0으로
        DirectX::XMFLOAT3 vel = actor_Physics->GetVelocity();
        vel.y = 0.0f;
        actor_Physics->SetVelocity(vel);
    }

    // 최종 Y = 지형 높이 + 모델 보정값 - 개별 바이어스
    actor_Position->SetPosition(
        currentPos.x,
        floorY,
        currentPos.z
    );
} // PlaceOn


void ActorObject::AddForce(const DirectX::XMFLOAT3& force)
{
    if (actor_Physics)
        actor_Physics->AddForce(force);
} // AddForc


/* private */
//////////////////////////////////////////////////////////////////

void ActorObject::ApplyDisplacement(float frameTime)
{
    DirectX::XMFLOAT3 currentPos = actor_Position->GetPosition();
    DirectX::XMFLOAT3 velocity = actor_Physics->GetVelocity();

    // s = s0 + vt (변위 적용)
    currentPos.x += velocity.x * frameTime;
    currentPos.y += velocity.y * frameTime;
    currentPos.z += velocity.z * frameTime;

    actor_Position->SetPosition(currentPos.x, currentPos.y, currentPos.z);
} // ApplyDisplacement


void ActorObject::ApplyRotation(float frameTime)
{
    DirectX::XMFLOAT3 vel = actor_Physics->GetVelocity();

    // 이동 속도가 거의 없다면 계산 생략
    float speedSq = vel.x * vel.x + vel.z * vel.z;
    if (speedSq < 0.001f) return;

    DirectX::XMFLOAT3 currentRot = actor_Position->GetRotation();

    float rollSpeed = 2.0f;
    currentRot.z -= vel.x * frameTime * rollSpeed; // X축 이동 -> Z축 회전
    currentRot.x += vel.z * frameTime * rollSpeed; // Z축 이동 -> X축 회전

    actor_Position->SetRotation(currentRot.x, currentRot.y, currentRot.z);
} // ApplyRotation