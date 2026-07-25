// Graphics/Model/ClusterModel.h
#pragma once
#include <d3d11.h>
#include <vector>
#include <memory>
// Graphics
#include "MeshModel.h"
#include "HeightMap/HeightMap.h"
#include "Model/Mesh/VertexTypes.h"
#include "Shader/ConstantBuffer.h"

class Frustum;
class MeshModel;
class CloudShader;

class ClusterModel {
public:
    ClusterModel();
    ClusterModel(const ClusterModel&) = delete;
    ~ClusterModel();

    bool Init(ID3D11Device* device, std::unique_ptr<MeshModel> fbx, int w, int h, int s);

    void Render(ID3D11DeviceContext* context, CloudShader* shader, Frustum* frustuml, DirectX::XMMATRIX worldMatrix, DirectX::XMMATRIX view, DirectX::XMMATRIX projection);

public:
    bool CreateCells(ID3D11Device* device, int cellDimension = 42);

public:
    float GetMaxHeightAt(float x, float z);
    const std::vector<std::unique_ptr<MeshModel>>& GetCell() const { return m_cells; }
    const int GetClusterWidth() const { return m_ClusterWidth; }
    const int GetClusterHeight() const { return m_ClusterHeight; }
    const int GetClusterScaling() const { return m_ClusterScaling; }

    void SetTime(float time) { m_time = time; }

private:
    struct CloudInstance {
        DirectX::XMMATRIX world;
        DirectX::XMFLOAT3 position; // 위치 캐싱
        float radius; // 충돌 반경 캐싱
        float topHeight; // 최고 높이
    };

private:
    std::vector<std::unique_ptr<MeshModel>> m_cells;
    std::unique_ptr<ConstantBuffer<MaterialBuffer>> m_MaterialBuffer;
    std::unique_ptr<MeshModel> m_FBX;
    std::vector<CloudInstance> m_cellHeights;
    std::vector<std::vector<std::vector<int>>> m_gridIndices;

    int m_ClusterWidth, m_ClusterHeight, m_ClusterScaling;
    int m_gridRows, m_gridCols;
    float m_time;
}; // ClusterModel