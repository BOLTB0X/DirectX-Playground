#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <wrl/client.h>
#include <vector>
#include <memory>


class GridModelBuffer {
public:
    struct VertexType {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT2 texture;
        DirectX::XMFLOAT3 normal;
        DirectX::XMFLOAT3 tangent;
	};

public:
    GridModelBuffer();
	GridModelBuffer(const GridModelBuffer& other) = delete;
    ~GridModelBuffer();

    bool Init(ID3D11Device*, int, int, float);
    void Shutdown();
    void Render(ID3D11DeviceContext*);

public:
    int GetVertexCount();
    void CopyVertexArray(void*);


private:
    bool InitiBuffers(ID3D11Device*);

private:
    std::vector<VertexType> m_vertices;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
    int m_vertexCount, m_indexCount;
    int m_gridWidth, m_gridHeight;
    float m_gridScale;
}; // GridModelBuffer