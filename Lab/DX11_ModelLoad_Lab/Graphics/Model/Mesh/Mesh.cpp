// Graphics/Model/Mesh.cpp
#include "Mesh.h"
#include "Common/EngineHelper.h"
#include "Model/Texture/Material.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

/* defulat */
///////////////////////////////////////////////////////////////////

Mesh::Mesh()
    : m_materialIndex(0),
    m_min({ 0, 0, 0 }),
    m_max({ 0, 0, 0 })
{
    m_meshData = { {}, {} };
}

Mesh::~Mesh() {}


bool Mesh::Init(ID3D11Device* device, const MeshData& data, unsigned int matIndex)
{
    m_meshData = data;
    m_materialIndex = matIndex;

    m_min = { FLT_MAX, FLT_MAX, FLT_MAX };
    m_max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

    for (const auto& v : data.vertices)
    {
        m_min.x = (std::min)(m_min.x, v.position.x);
        m_min.y = (std::min)(m_min.y, v.position.y);
        m_min.z = (std::min)(m_min.z, v.position.z);

        m_max.x = (std::max)(m_max.x, v.position.x);
        m_max.y = (std::max)(m_max.y, v.position.y);
        m_max.z = (std::max)(m_max.z, v.position.z);
    }

    vertexBuffer = std::make_unique<VertexBuffer>();
    if (EngineHelper::SuccessCheck(
        vertexBuffer->Init(device, data.vertices), "버텍스버퍼 초기화 실패")
        == false) return false;

    indexBuffer = std::make_unique<IndexBuffer>();
    if (EngineHelper::SuccessCheck(
        indexBuffer->Init(device, data.indices), "인덱스버퍼 초기화 실패")
        == false) return false;

    return true;
} // Init


void Mesh::Bind(ID3D11DeviceContext* context)
{
    if (!vertexBuffer || !indexBuffer) return;
    vertexBuffer->Bind(context);
    indexBuffer->Bind(context);
} // Render

///////////////////////////////////////////////////////////////////

/* public */
///////////////////////////////////////////////////////////////////


unsigned int Mesh::GetMaterialIndex() const
{
    return m_materialIndex;
} // GetMaterialIndex


unsigned int Mesh::GetIndexCount() const
{
    return indexBuffer->GetIndexCount();
} // GetIndexCount


const MeshData& Mesh::GetMeshData() const
{
    return m_meshData;
} // GetMeshData
