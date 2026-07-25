// Graphics/Model/Mesh.h
#pragma once

#include <memory>
#include <d3d11.h>
#include "VertexTypes.h"

class VertexBuffer;
class IndexBuffer;

class Mesh {
public:

	Mesh();
	Mesh(const Mesh&) = delete;
	~Mesh();

    bool Init(ID3D11Device*, const MeshData&, unsigned int);
    void Bind(ID3D11DeviceContext* context);

public:
    unsigned int GetMaterialIndex() const;
    unsigned int GetIndexCount() const;
    const MeshData& GetMeshData() const;
    const DirectX::XMFLOAT3& GetMin() const { return m_min; }
    const DirectX::XMFLOAT3& GetMax() const { return m_max; }

private:
    std::unique_ptr<VertexBuffer> vertexBuffer;
    std::unique_ptr<IndexBuffer> indexBuffer;
    unsigned int m_materialIndex;
    MeshData m_meshData;

    DirectX::XMFLOAT3 m_min;
    DirectX::XMFLOAT3 m_max;
}; // Mesh