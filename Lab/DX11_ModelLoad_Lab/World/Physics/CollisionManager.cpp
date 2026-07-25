// World/Physics/CollisionManager.cpp
#include "CollisionManager.h"
#include "MountPick/MountPick.h"
#include "Terrain/CloudOcean.h"
// Framework
#include "Actor/ActorObject.h"
#include "Position/Position.h"
#include "Physics/Physics.h"
// Common
#include "MathHelper.h"

/* default */
////////////////////////////////////////////////////////////////////////

CollisionManager::CollisionManager() {}

CollisionManager::~CollisionManager() {}


void CollisionManager::ApplySlopePhysics(ActorObject* actor, CloudOcean* ocean, float dt)
{
    Position* posComp = actor->GetPosition();
    //Physics* phyComp = actor->GetPhysics();
    DirectX::XMFLOAT3 pos = posComp->GetPosition();

    // 현재 위치 지형의 Normal을 가져옴
    DirectX::XMFLOAT3 normal = MathHelper::GetTerrainNormal(pos.x, pos.z, ocean->GetTotalTime());

    // 경사면 가속도
    float slopeIntensity = 50.0f; // 구르는 힘의 세기
    DirectX::XMFLOAT3 slopeForce = {
        normal.x * slopeIntensity,
        0.0f, // Y축 힘은 중력과 충돌이 해결하므로 0
        normal.z * slopeIntensity
    };

    // 물리 컴포넌트에 경사면 힘 추가
    phyComp->AddForce(slopeForce);
    return;
} // ApplySlopePhysics



void CollisionManager::HandleCloudCollision(ActorObject* actor, CloudOcean* ocean, float dt)
{
    Position* posComp = actor->GetPosition();
    //Physics* phyComp = actor->GetPhysics();

    DirectX::XMFLOAT3 pos = posComp->GetPosition();
    DirectX::XMFLOAT3 vel = phyComp->GetVelocity();

    // 현재 위치의 구름 높이 계산 (MathHelper FBM 사용)
    float cloudHeight = MathHelper::GetFBMHeight(pos.x, pos.z, ocean->GetTotalTime());

    // actor_Model->GetBottomOffset()을 ActorObject에 인터페이스로 만들어두면 좋습니다.
    // 여기서는 일단 이전에 설정한 높이 보정값(HeightOffset)을 사용한다고 가정합니다.
    float floorY = cloudHeight + actor->GetHeightOffset();

    // 중력 적용
    if (pos.y > floorY)
    {
        // AddForce는 가속도를 더하므로, 매 프레임 중력 가속도를 가해줍니다.
        phyComp->AddForce({ 0.0f, MathHelper::GRAVITY, 0.0f });
    }

    // 물리 엔진에 의한 이동 예측 (v = v0 + at 과정은 Physics::Frame에서 수행됨)
    // 여기서는 속도를 기반으로 위치를 미리 변경합니다.
    pos.x += vel.x * dt;
    pos.y += vel.y * dt;
    pos.z += vel.z * dt;

    // 바닥 충돌 판정 및 최종 보정 (Snap)
    if (pos.y <= floorY)
    {
        pos.y = floorY;

        // 바닥에 닿았으므로 수직 낙하 속도는 0으로
        vel.y = 0.0f;
        phyComp->SetVelocity(vel);
    }

    // 최종 계산된 위치를 Position 컴포넌트에 반영
    posComp->SetPosition(pos.x, pos.y, pos.z);
} // HandleCloudCollision


void CollisionManager::HandleMountCollision(ActorObject* actor, ActorObject* mount, float dt)
{
    if (!actor || !mount) return;

    Position* actorPosComp = actor->GetPosition();
    //Physics* actorPhyComp = actor->GetPhysics();
    DirectX::XMFLOAT3 aPos = actorPosComp->GetPosition();

    // 1. mount(절벽)의 높이 데이터를 가져옴
    // ActorObject에 GetHeightAtWorld(x, z) 인터페이스가 있다고 가정합니다.
    // MountPick은 내부적으로 메시의 하이트맵이나 정점 데이터를 참조해 높이를 반환합니다.
    float mountHeight = mount->GetHeightAtWorld(aPos.x, aPos.z);

    // 절벽의 최종 바닥 높이 (오프셋 적용)
    float mountFloorY = mountHeight + actor->GetHeightOffset();

    // 2. 현재 돌의 높이가 절벽 바닥보다 낮다면 (즉, 절벽을 밟고 있어야 하는 상황)
    // 단, 돌이 절벽 근처에 있을 때만 작동하도록 mount->IsInside(x, z) 같은 체크가 동반되면 더 좋습니다.
    if (mountHeight > -9999.0f) // GetHeightAtWorld가 유효한 높이를 반환했을 때
    {
        if (aPos.y < mountFloorY)
        {
            // 위치 보정 (절벽 위로 Snap)
            aPos.y = mountFloorY;
            actorPosComp->SetPosition(aPos.x, aPos.y, aPos.z);

            // 수직 속도 초기화
            //DirectX::XMFLOAT3 vel = actorPhyComp->GetVelocity();
            //vel.y = 0.0f;
            //actorPhyComp->SetVelocity(vel);
        }
    }

} // HandleMountCollision