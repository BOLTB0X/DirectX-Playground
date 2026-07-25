// Graphics/Model/IndexBuffer.cpp
#include "IndexBuffer.h"

/* defualt */
///////////////////////////////////////////////////////////////////

IndexBuffer::IndexBuffer() :m_indexCount(0) {};

IndexBuffer::~IndexBuffer() {};


bool IndexBuffer::Init(ID3D11Device* device, const std::vector<unsigned int>& indices)
{
    if (indices.empty()) return false;

    m_indexCount = (UINT)indices.size();

    D3D11_BUFFER_DESC bufDes = {};
    bufDes.Usage = D3D11_USAGE_IMMUTABLE; // 정적이니 수정
    bufDes.ByteWidth = sizeof(unsigned int) * m_indexCount;
    bufDes.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA subResourceData = {};
    subResourceData.pSysMem = indices.data();

    HRESULT hr = device->CreateBuffer(&bufDes, &subResourceData, m_buffer.GetAddressOf());
    return SUCCEEDED(hr);
} // Init


void IndexBuffer::Bind(ID3D11DeviceContext* context)
{
    context->IASetIndexBuffer(m_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
} // Bind