// Framework/ActorObject.h
#pragma once
#include <d3d11.h>
#include <memory>
#include <string>
#include <DirectXMath.h>

class Position;
class MeshModel;
class ActorsShader;
class Camera;
class Physics;

class ActorObject {
public:
    ActorObject();
    ActorObject(const ActorObject&) = delete;
    virtual ~ActorObject();

    virtual bool Init(MeshModel*, ActorsShader*, const std::string&);
    virtual void Shutdown();
    virtual void Frame(float);

    virtual void Render(ID3D11DeviceContext*, Camera*);

public:
    void PlaceOn(float);
    void AddForce(const DirectX::XMFLOAT3&);

    virtual float GetHeightAtWorld(float x, float z) const { return -10000.0f; }
    Position* GetPosition() const { return actor_Position.get(); }
    float GetHeightOffset() const { return m_heightOffset; }
    const std::string& GetName() const { return actor_name; }
    const bool getStatic() const { return m_isStatic; }

    void SetHeightOffset(float offset) { m_heightOffset = offset; }
    void SetStatic(bool isStatic) { m_isStatic = isStatic; }
    void SetFootingBias(float bias) { m_footingBias = bias; }

private:
    void ApplyDisplacement(float);
    void ApplyRotation(float frameTime);

protected: // ptr
    ActorsShader* actor_Shader;
    MeshModel* actor_Model;

    float m_footingBias;
    float m_heightOffset;
    bool m_isStatic;

public:
    std::unique_ptr<Physics> actor_Physics;
    std::unique_ptr<Position> actor_Position;

    std::string actor_name;
}; // ActorObject