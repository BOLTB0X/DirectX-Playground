// Graphics/Model/IndexBuffer.h
#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <vector>


class IndexBuffer {
public:
    IndexBuffer();
	IndexBuffer(const IndexBuffer&) = delete;
    ~IndexBuffer();

    bool Init(ID3D11Device*, const std::vector<unsigned int>&);
    void Bind(ID3D11DeviceContext* context);

public:
    UINT GetIndexCount() const { return m_indexCount; }

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
    UINT m_indexCount;
}; // IndexBuffer