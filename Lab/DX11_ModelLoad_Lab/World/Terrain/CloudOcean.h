// World/Terrain/CloudTerrain.h
#pragma once
#include <d3d11.h>
#include <memory>
#include <string>

class TerrainModel;
class CloudShader;
class Camera;
class Position;
class TerrainShader;

class CloudOcean {
public:
    CloudOcean();
    CloudOcean(const CloudOcean& other) = delete;
    ~CloudOcean();

    bool Init(TerrainModel* model, TerrainShader* shader, const std::string& name);

    void Render(ID3D11DeviceContext* context, Camera* camera);
    void Shutdown();
    void Frame(float);

public:
    float GetHeight(float x, float z) const;
    Position* GetPosition() const { return m_Position.get(); }
    float GetHeightAtWorld(float worldX, float worldZ) const;
    const float GetTotalTime() const { return m_totalTime; }

private:
    std::string m_name;
    std::unique_ptr<Position> m_Position;

    TerrainModel* m_modelPtr;
    TerrainShader* m_shaderPtr;
    float m_totalTime;
}; // CloudOcean