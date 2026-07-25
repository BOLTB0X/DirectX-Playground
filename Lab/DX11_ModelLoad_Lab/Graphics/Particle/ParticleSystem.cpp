// Graphics/Model/Particle/ParticleSystem.cpp
#include "ParticleSystem.h"
#include "Camera/Camera.h"
#include "Model/Mesh/VertexBuffer.h"
#include "Shader/ColorShader.h"
#include "Renderer/DepthStencilState.h"
#include "Renderer/BlendState.h"
// Common
#include "MathHelper.h"
// Framework
#include "Shader/Shader.h"
// etc
#include <random>

/* default */
//////////////////////////////////////////////////////////////////////////////


ParticleSystem::ParticleSystem()
    : m_maxParticles(0)
{
}

ParticleSystem::~ParticleSystem()
{
    Shutdown();
} // ~ParticleSystem


bool ParticleSystem::Init(ID3D11Device* device, int maxParticles)
{
    m_maxParticles = maxParticles;

    // 파티클 버퍼 초기화
    m_VertexBuffer = std::make_unique<VertexBuffer>();

    if (m_VertexBuffer->InitDynamic<ParticleData>(device, m_maxParticles) == false) return false;
    
    m_DepthState = std::make_unique<DepthStencilState>();
    if (m_DepthState->InitForParticle(device) == false) return false;
    m_BlendState = std::make_unique<BlendState>();
    if (m_BlendState->InitForParticle(device) == false) return false;

    ResetParticles(); // 파티클 데이터 생성 및 배치
    return true;
} // Init


void ParticleSystem::Shutdown()
{
    m_Particles.clear();
    m_VertexBuffer.reset();
} // Shutdown


void ParticleSystem::Frame(float frameTime)
{
} // Frame


void ParticleSystem::Render(ID3D11DeviceContext* context, Camera* camera, Shader* shader)
{
    if (context == nullptr || camera == nullptr || shader == nullptr)
        return;

    m_DepthState->Bind(context, false); // DepthDisabledState 호출
    m_BlendState->Bind(context, true); // AlphaEnableState 호출

    m_VertexBuffer->Update(context, m_Particles);
    m_VertexBuffer->Bind(context);

    shader->UpdateMatrixBuffer(context, XMMatrixIdentity(), camera->GetViewMatrix(), camera->GetProjectionMatrix());
    shader->Bind(context);

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
    context->Draw(m_maxParticles, 0);

    m_DepthState->Bind(context, true);
    m_BlendState->Bind(context, false);

    context->GSSetShader(nullptr, nullptr, 0); // 복구
} // Render

//////////////////////////////////////////////////////////////////////////////


/* public */
/////////////////////////////////////////////////////////////////////////////

void ParticleSystem::ResetParticles()
{
    DirectX::XMFLOAT3 center = { 0.0f, 20.0f, 0.0f };
    DirectX::XMFLOAT3 range = { 500.0f, 15.0f, 500.0f };

    GenerateCloudVolume(center, range, 15.0f, 40.0f);
} // ResetParticles


void ParticleSystem::GenerateCloudVolume(DirectX::XMFLOAT3 center, DirectX::XMFLOAT3 range, float minSize, float maxSize)
{
    m_Particles.clear();
    m_Particles.resize(m_maxParticles);

    std::random_device rd;
    std::mt19937 gen(rd());

    // 지정된 중심점 기준 범위 내에서 랜덤 분포
    std::uniform_real_distribution<float> distX(center.x - range.x, center.x + range.x);
    std::uniform_real_distribution<float> distY(center.y - range.y, center.y + range.y);
    std::uniform_real_distribution<float> distZ(center.z - range.z, center.z + range.z);
    std::uniform_real_distribution<float> distSize(minSize, maxSize);

    for (int i = 0; i < m_maxParticles; ++i)
    {
        m_Particles[i].position = { distX(gen), distY(gen), distZ(gen) };
        m_Particles[i].size = distSize(gen);
    }
} // GenerateCloudVolume