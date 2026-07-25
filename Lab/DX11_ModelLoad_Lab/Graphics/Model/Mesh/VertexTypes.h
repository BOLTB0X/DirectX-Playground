// Graphics/Model/Mesh/VertexTypes.h
#pragma once
#include <directxmath.h>
#include <vector>

struct ModelVertex {
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT2 texture;
    DirectX::XMFLOAT3 normal;
    DirectX::XMFLOAT3 tangent;
    DirectX::XMFLOAT3 binormal;
}; // ModelVertex

struct BillboardVertex {
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT2 texture;
}; // BillboardVertex


struct MeshData {
    std::vector<ModelVertex> vertices;
    std::vector<unsigned int> indices;
}; // MeshData


namespace CellConfig {
    const int CELLS_PER_ROW = 4;
    const int VERTICES_PER_CELL = 0;
} // CellConfig


struct ParticleData {
    DirectX::XMFLOAT3 position;
    float size;
};