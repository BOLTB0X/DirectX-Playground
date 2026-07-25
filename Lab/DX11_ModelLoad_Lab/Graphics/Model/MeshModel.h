// Graphics/Model/MeshModel.h
#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include <memory>
#include <vector>
#include "Model/Mesh/VertexTypes.h"
#include "Model/Texture/Material.h"
#include "Shader/ConstantBuffer.h"

class Mesh;
class Shader;
class Frustum;

class MeshModel {
public:
    MeshModel();
	MeshModel(const MeshModel&) = delete;
	~MeshModel();

    void Render(ID3D11DeviceContext* context, Shader* shader, Frustum* frustum, DirectX::XMMATRIX worldMatrix);

public:
    void AddMesh(std::unique_ptr<Mesh>);
    void AddMaterial(const Material&);

    bool InitConstantBuffer(ID3D11Device*);
    void GetPhysicsData(std::vector<ModelVertex>& outVertices, std::vector<unsigned int>& outIndices) const;
    float GetBottomOffset();
    float GetMinWidth() const;
    float GetMaxWidth() const;
    float GetMinHeight() const;
    float GetMaxHeight() const;
    float GetMinDepth() const;
    float GetMaxDepth() const;

private:
    void GetBoundingBox(DirectX::XMFLOAT3&, DirectX::XMFLOAT3&) const;

public:
    std::vector<MeshData> GetFullMeshData() const;

private:
    std::vector<std::unique_ptr<Mesh>> m_meshes;
    std::vector<Material> m_materials;
    std::unique_ptr<ConstantBuffer<MaterialBuffer>> m_materialBuffer;
}; // MeshModel