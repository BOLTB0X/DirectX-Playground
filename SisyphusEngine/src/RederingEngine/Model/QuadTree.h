#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <wrl/client.h>
#include <vector>
#include <memory>


class GridModelBuffer;
class Frustum;
class OceanShader;

const int MAX_TRIANGLES_PER_NODE = 10000;

class QuadTree {
public:
    struct VertexType {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT2 texture;
        DirectX::XMFLOAT3 normal;
    };

    struct NodeType {
        float positionX, positionZ, width;
        int triangleCount;
        Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
        std::unique_ptr<NodeType> nodes[4];

		NodeType::NodeType()
            : positionX(0),positionZ(0), width(0),
            triangleCount(0)
        {
        }
    };

public:
    QuadTree();
	QuadTree(const QuadTree& other) = delete;
    ~QuadTree();

    bool Init(GridModelBuffer*, ID3D11Device*);
    void Shutdown();

public:
    void Render(Frustum*, ID3D11DeviceContext*, OceanShader*);
    int GetDrawCount() const;

private:
    void CalculateMeshDimensions(int, float&, float&, float&);
    void CreateTreeNode(NodeType*, float, float, float, ID3D11Device*);
    int CountTriangles(float, float, float);
    bool IsTriangleContained(int, float, float, float);
    void RenderNode(NodeType*, Frustum*, ID3D11DeviceContext*, OceanShader*);

private:
    std::vector<VertexType> m_vertexList;
    std::unique_ptr<NodeType> m_parentNode;
    int m_triangleCount, m_drawCount;
}; // QuadTree