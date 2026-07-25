// Graphics/Model/ClusterModel.cpp
#include "ClusterModel.h"
#include "Model/MeshModel.h"
// 추후 ClusterModelCell.h 추가
#include "Shader/CloudShader.h"
#include "Camera/Frustum.h"
// Common
#include "MathHelper.h"
#include "EngineHelper.h"


/* default */
/////////////////////////////////////////////////////////////////

ClusterModel::ClusterModel()
	: m_ClusterWidth(0),
	m_ClusterHeight(0),
	m_ClusterScaling(0),
	m_time(0)
{}


ClusterModel::~ClusterModel() {}


bool ClusterModel::Init(ID3D11Device* device, std::unique_ptr<MeshModel> fbx, int width, int height, int scaling)
{
    if (fbx == nullptr) return false;

    m_FBX = std::move(fbx);
    m_ClusterWidth = width;
    m_ClusterHeight = height;
    m_ClusterScaling = scaling;

    if (m_FBX->InitConstantBuffer(device) == false) return false;

    if (EngineHelper::SuccessCheck(CreateCells(device, 42),
        "ClusterModel:: Init 내부에서 CreateCells 실패") 
        == false) return false;

    return true;
} // Init


void ClusterModel::Render(
    ID3D11DeviceContext* context,
    CloudShader* shader,
    Frustum* frustum,
    DirectX::XMMATRIX worldMatrix,
    DirectX::XMMATRIX view,
    DirectX::XMMATRIX projection)
{
    DirectX::XMFLOAT4X4 worldFB4;
    DirectX::XMStoreFloat4x4(&worldFB4, worldMatrix);
    float offsetX = worldFB4._41;
    float offsetY = worldFB4._42;
    float offsetZ = worldFB4._43;

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    shader->UpdateMaterialTag(context, 0, m_time);
    shader->Bind(context);

    for (const auto& inst : m_cellHeights)
    {
        DirectX::XMMATRIX finalWorld = inst.world * worldMatrix;

        float maxWidth = m_FBX->GetMaxWidth() + offsetX;
        float maxHeight = m_FBX->GetMaxHeight() + offsetY;
        float maxDepth = m_FBX->GetMaxDepth() + offsetZ;

        float minWidth = m_FBX->GetMinWidth() + offsetX;
        float minHeight = m_FBX->GetMinHeight() + offsetY;
        float minDepth = m_FBX->GetMinDepth() + offsetZ;

        if (frustum->CheckBoundingBoxMinMax(maxWidth, maxHeight, maxDepth, minWidth, minHeight, minDepth)
            == false)
            continue;
        
        shader->UpdateMatrixBuffer(context, finalWorld, view, projection);
        //m_FBX->Render(context, shader, frustum, worldMatrix);
        m_FBX->DrawOnly(context);
    } // for (const auto& inst : m_cellHeights)

} // Render


bool ClusterModel::CreateCells(ID3D11Device* device, int cellDimension)
{
    int cellsPerRow = (m_ClusterWidth - 1) / (cellDimension - 1);
    int cellsPerCol = (m_ClusterHeight - 1) / (cellDimension - 1);

    std::srand(static_cast<unsigned int>(time(nullptr)));

    // 원본 모델의 크기 정보 미리 획득
    float modelMaxY = m_FBX->GetMaxHeight();
    float modelWidth = m_FBX->GetMaxWidth() - m_FBX->GetMinWidth();
    float modelDepth = m_FBX->GetMaxDepth() - m_FBX->GetMinDepth();

    // 모델의 기본 반경 (가로, 세로 중 큰 값의 절반 정도로 가정)
    float baseRadius = (std::max)(modelWidth, modelDepth) * 0.5f;

    m_cellHeights.clear();

    for (unsigned int cz = 0; cz < cellsPerCol; cz++)
    {
        for (unsigned int cx = 0; cx < cellsPerRow; cx++)
        {
            float startX = static_cast<float>(cx * (cellDimension - 1));
            float startZ = static_cast<float>(cz * (cellDimension - 1));

            int cloudsInCell = 4 + (std::rand() % 5);

            for (int i = 0; i < cloudsInCell; i++)
            {
                CloudInstance instance;

                float rawX = startX + static_cast<float>(std::rand() % cellDimension);
                float rawZ = startZ + static_cast<float>(std::rand() % cellDimension);

                float localX = MathHelper::clamp(rawX, startX, startX + (cellDimension - 1));
                float localZ = MathHelper::clamp(rawZ, startZ, startZ + (cellDimension - 1));
                float localY = (static_cast<float>(std::rand() % 100) / 100.0f) * m_ClusterScaling;

                float rotationRad = MathHelper::ToRadians(static_cast<float>(std::rand() % 360));
                float scaleVal = 1.0f + (static_cast<float>(std::rand() % 150) / 100.0f);

                instance.position = { localX, localY, localZ };
                instance.radius = baseRadius * scaleVal;
                instance.topHeight = localY + (modelMaxY * scaleVal);

                instance.world = DirectX::XMMatrixScaling(scaleVal, scaleVal, scaleVal) *
                    DirectX::XMMatrixRotationY(rotationRad) *
                    DirectX::XMMatrixTranslation(localX, localY, localZ);

                m_cellHeights.push_back(instance);
            } // for (int i = 0; i < cloudsInCell; i++)
        } // for (int cx = 0; cx < cellsPerRow; cx++)
    }

    EngineHelper::DebugPrint("ClusterModel: " + std::to_string(m_cellHeights.size()) + " Instances Generated.");
    return true;
} // CreateCells


float ClusterModel::GetMaxHeightAt(float localX, float localZ)
{
    float maxHeight = -1.0f;

    for (const auto& inst : m_cellHeights)
    {
        float dx = localX - inst.position.x;
        float dz = localZ - inst.position.z;

        float distSq = (dx * dx) + (dz * dz);
        float radiusSq = inst.radius * inst.radius;

        if (distSq < radiusSq)
        {
            if (inst.topHeight > maxHeight)
                maxHeight = inst.topHeight;
        }
    }

    return (maxHeight < 0) ? 0.0f : maxHeight;
} // GetMaxHeightAt