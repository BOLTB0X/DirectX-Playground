// Graphics/MeshModel/TerrainModelCell.h
#pragma once
#include <d3d11.h>
#include <memory>
#include <vector>
#include "Model/Mesh/VertexTypes.h"

class VertexBuffer;
class IndexBuffer;

class TerrainModelCell {
public:
    TerrainModelCell();
	TerrainModelCell(const TerrainModelCell&) = delete;
    ~TerrainModelCell();

    bool Init(
        ID3D11Device* device,
        const std::vector<ModelVertex>&,
        const std::vector<unsigned int>&);

    void Render(ID3D11DeviceContext* context);

public:
    float GetMinWidth() const { return m_minWidth; }
    float GetMinHeight() const { return m_minHeight; }
    float GetMinDepth() const { return m_minDepth; }
    float GetMaxWidth() const { return m_maxWidth; }
    float GetMaxHeight() const { return m_maxHeight; }
    float GetMaxDepth() const { return m_maxDepth; }

private:
    std::unique_ptr<VertexBuffer> m_VertexBuffer;
    std::unique_ptr<IndexBuffer> m_IndexBuffer;

    // Culling용 범위 정보
    float m_maxWidth, m_maxHeight, m_maxDepth;
    float m_minWidth, m_minHeight, m_minDepth;
}; // TerrainModelCell