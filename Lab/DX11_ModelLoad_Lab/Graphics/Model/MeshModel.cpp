// Graphics/Model/MeshModel.cpp
#include "MeshModel.h"
#include "Mesh/Mesh.h"
#include "Texture/Material.h"
#include "Shader/Shader.h"
#include "Camera/Frustum.h"
// Common
#include "EngineHelper.h"

#include <string>

/* default */
////////////////////////////////////////////////////////////////////////////

MeshModel::MeshModel() {};

MeshModel::~MeshModel() {};


void MeshModel::Render(ID3D11DeviceContext* context, Shader* shader, Frustum* frustum, DirectX::XMMATRIX worldMatrix)
{
    DirectX::XMFLOAT4X4 worldFB4;
    DirectX::XMStoreFloat4x4(&worldFB4, worldMatrix);
    float offsetX = worldFB4._41;
    float offsetY = worldFB4._42;
    float offsetZ = worldFB4._43;

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    int totalSubMeshes = (int)m_meshes.size();
    int drawnSubMeshes = 0;

    for (const auto& mesh : m_meshes)
    {
        if (frustum)
        {
            const auto& min = mesh->GetMin();
            const auto& max = mesh->GetMax();

            if (frustum->CheckBoundingBoxMinMax(
                max.x + offsetX, max.y + offsetY, max.z + offsetZ,
                min.x + offsetX, min.y + offsetY, min.z + offsetZ) == false)
            {
                continue;
            }
        }

        // 해당 메쉬의 머터리얼 인덱스 확인
        unsigned int matIdx = mesh->GetMaterialIndex();

        if (matIdx < m_materials.size())
        {
			const Material& material = m_materials[matIdx];
            //if (material.diffuse) material.diffuse->Bind(context, 0);
            //if (material.ambient) material.ambient->Bind(context, 1);
            //if (material.specular) material.specular->Bind(context, 2);

            MaterialBuffer data;
            data.type = (int)material.type;
            shader->UpdateMaterialTag(context, (int)material.type);
            if (m_materialBuffer)
            {
                m_materialBuffer->Update(context, data);
                m_materialBuffer->BindPS(context, 2);
            }

            if (material.albedo) material.albedo->Bind(context, 0);
            if (material.normal) material.normal->Bind(context, 1);
            if (material.metallic) material.metallic->Bind(context, 2);
            if (material.roughness) material.roughness->Bind(context, 3);
            if (material.alpha) material.alpha->Bind(context, 4);

        }

        // 메쉬 그리기
        mesh->Bind(context);
        context->DrawIndexed(mesh->GetIndexCount(), 0, 0);
        drawnSubMeshes++;
    }
    //EngineHelper::DebugPrint("Drawn: " + std::to_string(drawnSubMeshes) + "/" + std::to_string(totalSubMeshes) + "\n");
} // Render

void MeshModel::AddMesh(std::unique_ptr<Mesh> mesh)
{
    m_meshes.push_back(std::move(mesh));
} // AddMesh


void MeshModel::AddMaterial(const Material& material)
{
    m_materials.push_back(material);
} // AddMaterial


bool MeshModel::InitConstantBuffer(ID3D11Device* device)
{
    m_materialBuffer = std::make_unique<ConstantBuffer<MaterialBuffer>>();
    return m_materialBuffer->Init(device);
} // InitConstantBuffer


void MeshModel::GetPhysicsData(std::vector<ModelVertex>& outVertices, std::vector<unsigned int>& outIndices) const
{
    outVertices.clear();
    outIndices.clear();

    unsigned int vertexOffset = 0;
    for (const auto& mesh : m_meshes)
    {
        const auto& data = mesh->GetMeshData();

        outVertices.insert(outVertices.end(), data.vertices.begin(), data.vertices.end());

        for (auto idx : data.indices)
        {
            outIndices.push_back(idx + vertexOffset);
        }

        vertexOffset += static_cast<unsigned int>(data.vertices.size());
    }
} // GetPhysicsData


std::vector<MeshData> MeshModel::GetFullMeshData() const
{
    std::vector<MeshData> allData;
    for (const auto& mesh : m_meshes)
        allData.push_back(mesh->GetMeshData());
    
    return allData;
} // GetFullMeshData


float MeshModel::GetBottomOffset()
{
    float minY = 0.0f;

    for (const auto& mesh : m_meshes)
    {
        if (mesh->GetMin().y < minY)
            minY = mesh->GetMin().y;
    }
    
    return -minY;
} // GetBottomOffset


float MeshModel::GetMinWidth() const
{
    float minX = 0.0f;
    for (const auto& mesh : m_meshes)
        minX = (std::min)(minX, mesh->GetMin().x);
    return minX;
} // GetMinWidth


float MeshModel::GetMaxWidth() const
{
    float maxX = 0.0f;
    for (const auto& mesh : m_meshes)
        maxX = (std::max)(maxX, mesh->GetMax().x);
    return maxX;
} // GetMaxWidth


float MeshModel::GetMinHeight() const
{
    float minY = 0.0f;
    for (const auto& mesh : m_meshes)
        minY = (std::min)(minY, mesh->GetMin().y);
    return minY;
} // GetMinHeight


float MeshModel::GetMaxHeight() const
{
    float maxY = 0.0f;
    for (const auto& mesh : m_meshes)
        maxY = (std::max)(maxY, mesh->GetMax().y);
    return maxY;
} // GetMaxHeight


float MeshModel::GetMinDepth() const
{
    float minZ = 0.0f;
    for (const auto& mesh : m_meshes)
        minZ = (std::min)(minZ, mesh->GetMin().z);
    return minZ;
} // GetMinDepth


float MeshModel::GetMaxDepth() const
{
    float maxZ = 0.0f;
    for (const auto& mesh : m_meshes)
        maxZ = (std::max)(maxZ, mesh->GetMax().z);
    return maxZ;
} // GetMaxDepth


void MeshModel::GetBoundingBox(DirectX::XMFLOAT3& min, DirectX::XMFLOAT3& max) const
{
    min = { FLT_MAX, FLT_MAX, FLT_MAX };
    max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

    for (const auto& mesh : m_meshes)
    {
        const auto& mMin = mesh->GetMin();
        const auto& mMax = mesh->GetMax();

        min.x = (std::min)(min.x, mMin.x);
        min.y = (std::min)(min.y, mMin.y);
        min.z = (std::min)(min.z, mMin.z);

        max.x = (std::max)(max.x, mMax.x);
        max.y = (std::max)(max.y, mMax.y);
        max.z = (std::max)(max.z, mMax.z);
    }
} // GetBoundingBox