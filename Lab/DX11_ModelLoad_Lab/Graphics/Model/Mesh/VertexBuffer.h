// Graphics/MeshModel/VertexBuffer.h
#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <vector>
#include "Model/Mesh/VertexTypes.h"

class VertexBuffer {
public:
    VertexBuffer();
	VertexBuffer(const VertexBuffer&) = delete;
    ~VertexBuffer();

    template<typename T>
    bool Init(ID3D11Device* device, const std::vector<T>& vertices)
    {
        if (vertices.empty()) return false;

        m_stride = sizeof(T);
        D3D11_BUFFER_DESC bufDes = {};
        bufDes.Usage = D3D11_USAGE_DEFAULT;
        bufDes.ByteWidth = m_stride * (UINT)vertices.size();
        bufDes.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bufDes.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA subData = {};
        subData.pSysMem = vertices.data();

        return SUCCEEDED(device->CreateBuffer(&bufDes, &subData, m_buffer.GetAddressOf()));
    } // Init


    template<typename T>
    bool InitDynamic(ID3D11Device* device, UINT maxCount)
    {
        m_stride = sizeof(T);
        D3D11_BUFFER_DESC vbDesc = {};
        vbDesc.Usage = D3D11_USAGE_DYNAMIC;
        vbDesc.ByteWidth = m_stride * maxCount;
        vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        return SUCCEEDED(device->CreateBuffer(&vbDesc, nullptr, m_buffer.GetAddressOf()));
    } // InitDynamic


    template<typename T>
    void Update(ID3D11DeviceContext* context, const std::vector<T>& data)
    {
        if (data.empty()) return;

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        HRESULT hr = context->Map(m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

        if (SUCCEEDED(hr))
        {
            memcpy(mappedResource.pData, data.data(), sizeof(T) * data.size());
            context->Unmap(m_buffer.Get(), 0);
        }
    } // Update

public:
    void Bind(ID3D11DeviceContext*);

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
    UINT m_stride;
}; // VertexBuffer
