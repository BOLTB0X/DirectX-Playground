// World/World.h
#pragma once
#include <windows.h>
#include <d3d11.h>
#include <vector>
#include <memory>
// Framework


class ModelManager;
class TexturesManager;
class ShaderManager;
class ActorObject;
class Camera;
class Position;
class CloudOcean;
class ActorsShader;
class TerrainShader;
class UserInput;
//class CollisionManager;
class ParticleSystem;
class CloudShader;
class Renderer;

struct WorldInitParam {
    ID3D11Device* device;
    ID3D11DeviceContext* context;
    ModelManager* modelManager;
    ShaderManager* shaderManager;
    TexturesManager* textureManager;
    HWND hwnd;
}; // WorldInitParam


class World {
public:
    World();
	World(const World&) = delete;
    ~World();

    bool Init(const WorldInitParam& param);
    void Shutdown();
    void Frame(float, UserInput*, bool);

    void Render(ShaderManager* shaderManager, Renderer* renderer);

public:
    ActorObject* GetActor(size_t index) const;
    const std::vector<std::unique_ptr<ActorObject>>& GetActors() const;
    Camera* GetCamera() const;
    Position* GetCameraPosition() const;

private:
    void UpdateShaderMaterial(float);
    void UpdateCameraFollow(float);

private:
    std::unique_ptr<Camera> m_Camera;
    std::vector<std::unique_ptr<ActorObject>> m_Actors;
    std::unique_ptr<CloudOcean> m_CloudOcean;
    //std::unique_ptr<CollisionManager> m_CollisionManager;
    //std::unique_ptr<ParticleSystem> m_ParticleSystem;

    ID3D11DeviceContext* m_DeviceContext;
    ActorsShader* actorsShaderPtr;
    TerrainShader* terrainShaderPtr;
    CloudShader* cloudShaderPtr;

}; // World