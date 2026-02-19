#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <wrl/client.h>
#include <vector>
#include <memory>


enum class DefaultModelType {
    Quad,
    Cube,
    Sphere,
    Ocean
}; // DefaultModelType


class DefaultModelBuffer {
public:
    struct VertexType {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT2 texture;
        DirectX::XMFLOAT3 normal;
    };

public:
    DefaultModelBuffer();
    DefaultModelBuffer(const DefaultModelBuffer& other) = delete;
    ~DefaultModelBuffer();

    bool Init(ID3D11Device*, DefaultModelType);
	bool Init(ID3D11Device*, const std::vector<VertexType>&, const std::vector<unsigned long>&);
    void Shutdown();

    void Render(ID3D11DeviceContext*);

public:
    int GetVertexCount() const;
    int GetIndexCount() const;
    void CreateCube(std::vector<VertexType>&, std::vector<unsigned long>&, float, float, float, float, float, float);

private:
    bool InitBuffers(ID3D11Device*, std::vector<VertexType>, std::vector<unsigned long>);

    void CreateQuad(std::vector<VertexType>&, std::vector<unsigned long>&, float width = 2.0f, float height = 2.0f);
    void CreateCube(std::vector<VertexType>&, std::vector<unsigned long>&, float width = 2.0f, float height = 1.0f, float depth = 1.0f);
    void CreateSphere(std::vector<VertexType>&, std::vector<unsigned long>&, float radius = 2.0f, int sliceCount = 20, int stackCount = 20);
    void CreateOcean(std::vector<VertexType>&, std::vector<unsigned long>&);

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

    int m_vertexCount;
    int m_indexCount;
}; // DefaultModelBuffer