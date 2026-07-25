// Graphics/Billboard/Billboard.h
#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <memory>

class Camera;
class Position;
class VertexBuffer;
class IndexBuffer;


class Billboard {
public:
    Billboard();
    Billboard(const Billboard& other) = delete;
    ~Billboard();

    bool Init(ID3D11Device* device);
    void Render(ID3D11DeviceContext* context, Camera* camera);

    //void SetPosition(const DirectX::XMFLOAT3& pos);
    void SetSize(float size) { m_size = size; }

private:
    void UpdateVertices(ID3D11DeviceContext*, Camera*);

private:
    std::unique_ptr<VertexBuffer> m_VertexBuffer;
    std::unique_ptr<IndexBuffer> m_IndexBuffer;
    std::unique_ptr<Position> m_Position;
    float m_size;
}; // Billboard
