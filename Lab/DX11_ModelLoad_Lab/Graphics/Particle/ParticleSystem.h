// Graphics/Model/Cloud/ParticleSystem.h
#pragma once
#include "Model/Mesh/VertexTypes.h"
#include <d3d11.h>
#include <directxmath.h>
#include <vector>
#include <memory>

class Camera;
class VertexBuffer;
class Shader;
class DepthStencilState;
class BlendState;


class ParticleSystem {
public:
    ParticleSystem();
    ParticleSystem(const ParticleSystem& other) = delete;
    ~ParticleSystem();

    bool Init(ID3D11Device*, int);
    void Shutdown();

    void Frame(float);
    void Render(ID3D11DeviceContext*, Camera*, Shader*);

    void ResetParticles();
    void GenerateCloudVolume(DirectX::XMFLOAT3, DirectX::XMFLOAT3, float, float);

private:
    std::vector<ParticleData> m_Particles;
    std::unique_ptr<VertexBuffer> m_VertexBuffer;
    std::unique_ptr<DepthStencilState> m_DepthState;
    std::unique_ptr<BlendState> m_BlendState;

    int m_maxParticles;
}; // ParticleSystem