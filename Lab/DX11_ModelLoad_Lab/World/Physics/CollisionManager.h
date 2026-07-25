// World/Physics/CollisionManager.h
#pragma once
// Framework
#include "Physics/Physics.h"

class ActorObject;
class CloudOcean;
class MountPick;

class CollisionManager {
public:
    CollisionManager();
    CollisionManager(const CollisionManager& other) = delete;
    ~CollisionManager();

    void ApplySlopePhysics(ActorObject*, CloudOcean*, float);
    void HandleCloudCollision(ActorObject*, CloudOcean*, float);
    void HandleMountCollision(ActorObject*, ActorObject*, float);

private:
    // 내부 보조 계산 함수들
}; // CollisionManager