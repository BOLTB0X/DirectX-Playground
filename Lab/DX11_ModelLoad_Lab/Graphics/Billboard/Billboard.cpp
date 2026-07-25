// Graphics/Billboard/Billboard.cpp
#include "Billboard.h"
// Framework
#include "Position/Position.h"
// Graphics
#include "Model/Mesh/VertexBuffer.h"
#include "Model/Mesh/IndexBuffer.h"
#include "Camera/Camera.h"
// Commom
#include "EngineHelper.h"
// etc
#include <vector>


/* default */
////////////////////////////////////////////////////////////////////////


Billboard::Billboard()
    : m_size(5.0f)
{
}

Billboard::~Billboard() {}


bool Billboard::Init(ID3D11Device* device)
{
    m_Position = std::make_unique<Position>();
    m_size = 1.0f;

    m_VertexBuffer = std::make_unique<VertexBuffer>();
    if (EngineHelper::SuccessCheck(
        m_VertexBuffer->InitDynamic<BillboardVertex>(device, 4), "버텍스버퍼 초기화 실패")
        == false) return false;

    m_IndexBuffer = std::make_unique<IndexBuffer>();
    std::vector<unsigned int>  data = { 0, 1, 2, 2, 1, 3 };
    if (EngineHelper::SuccessCheck(
        m_IndexBuffer->Init(device, data), "인덱스버퍼 초기화 실패")
        == false) return false;

    return true;
} // Init


void Billboard::Render(ID3D11DeviceContext* context, Camera* camera)
{
    if (context == nullptr || camera == nullptr
        || m_VertexBuffer == nullptr || m_IndexBuffer == nullptr) return;

    UpdateVertices(context, camera);

    m_VertexBuffer->Bind(context);
    m_IndexBuffer->Bind(context);

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->DrawIndexed(6, 0, 0);
} // Render



void Billboard::UpdateVertices(ID3D11DeviceContext* context, Camera* camera)
{
    if (!context || !camera || !m_Position) return;

    // 카메라의 View Matrix로부터 Right와 Up 벡터 추출
    DirectX::XMFLOAT3 camRight, camUp;
    MathHelper::GetCameraVectors(camera->GetViewMatrix(), camRight, camUp);

    DirectX::XMFLOAT3 center = m_Position->GetPosition();
    float halfSize = m_size * 0.5f;

    std::vector<BillboardVertex> vertices(4);

    // 좌측 하단 (v0)
    vertices[0].position = {
        center.x - (camRight.x + camUp.x) * halfSize,
        center.y - (camRight.y + camUp.y) * halfSize,
        center.z - (camRight.z + camUp.z) * halfSize
    };
    vertices[0].texture = { 0.0f, 1.0f };

    // 좌측 상단 (v1)
    vertices[1].position = {
        center.x - (camRight.x - camUp.x) * halfSize,
        center.y - (camRight.y - camUp.y) * halfSize,
        center.z - (camRight.z - camUp.z) * halfSize
    };
    vertices[1].texture = { 0.0f, 0.0f };

    // 우측 하단 (v2)
    vertices[2].position = {
        center.x + (camRight.x - camUp.x) * halfSize,
        center.y + (camRight.y - camUp.y) * halfSize,
        center.z + (camRight.z - camUp.z) * halfSize
    };
    vertices[2].texture = { 1.0f, 1.0f };

    // 우측 상단 (v3)
    vertices[3].position = {
        center.x + (camRight.x + camUp.x) * halfSize,
        center.y + (camRight.y + camUp.y) * halfSize,
        center.z + (camRight.z + camUp.z) * halfSize
    };
    vertices[3].texture = { 1.0f, 0.0f };

    m_VertexBuffer->Update<BillboardVertex>(context, vertices);
} // UpdateVertices