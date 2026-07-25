// Graphics/MeshModel/TerrainModelCell.cpp
#include "TerrainModelCell.h"
#include "EngineHelper.h"
#include "Mesh/VertexBuffer.h"
#include "Mesh/IndexBuffer.h"

#include <string>
/* default */
/////////////////////////////////////////////////////

TerrainModelCell::TerrainModelCell()
    : m_maxWidth(0), m_maxHeight(0), m_maxDepth(0),
    m_minWidth(0), m_minHeight(0), m_minDepth(0)
{
} // TerrainModelCell


TerrainModelCell::~TerrainModelCell()
{
} // TerrainModelCell


bool TerrainModelCell::Init(ID3D11Device* device,
    const std::vector<ModelVertex>& vertices,
    const std::vector<unsigned int>& indices)
{
    m_minWidth = m_minHeight = m_minDepth = FLT_MAX;
    m_maxWidth = m_maxHeight = m_maxDepth = -FLT_MAX;

    for (const auto& v : vertices)
    {
        m_minWidth = (std::min)(m_minWidth, v.position.x);
        m_minHeight = (std::min)(m_minHeight, v.position.y);
        m_minDepth = (std::min)(m_minDepth, v.position.z);
        m_maxWidth = (std::max)(m_maxWidth, v.position.x);
        m_maxHeight = (std::max)(m_maxHeight, v.position.y);
        m_maxDepth = (std::max)(m_maxDepth, v.position.z);
    }

    m_VertexBuffer = std::make_unique<VertexBuffer>();
    if (m_VertexBuffer->Init(device, vertices)
        == false) return false;

    m_IndexBuffer = std::make_unique<IndexBuffer>();
    if (m_IndexBuffer->Init(device, indices)
        == false) return false;

    return true;
} // Init


void TerrainModelCell::Render(ID3D11DeviceContext* context)
{ 
    m_VertexBuffer->Bind(context);
    m_IndexBuffer->Bind(context);

    context->DrawIndexed(m_IndexBuffer->GetIndexCount(), 0, 0);
} // Render