// World/Mountain/MountPick.h
#pragma once
#include <vector>
// Framework
#include "Actor/ActorObject.h"
// Graphics
#include "Model/HeightMap/HeightMap.h"

class MeshModel;
class ActorsShader;
class Camera;
class HeightMap;

class MountPick : public ActorObject {
public:
    MountPick();
    MountPick(const MountPick& other) = delete;
    virtual ~MountPick();

    virtual bool Init(MeshModel*, ActorsShader*, const std::string&) override;
    virtual void Shutdown() override;
    virtual void Frame(float) override;
    virtual void Render(ID3D11DeviceContext*, Camera*) override;
    virtual float GetHeightAtWorld(float worldX, float worldZ) const override;

public:
    float GetHeightAtMesh(float x, float z) const;

private:
    std::unique_ptr<HeightMap> m_heightMap;
}; // MountPick