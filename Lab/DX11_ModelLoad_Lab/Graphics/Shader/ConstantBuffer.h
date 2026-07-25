// Graphics/Shader/ConstantBuffer.h
#pragma once
#include <d3d11.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

template<typename T>
class ConstantBuffer
{
public:
    ConstantBuffer() = default;
	ConstantBuffer(const ConstantBuffer&) = delete;
	~ConstantBuffer() = default;

    bool Init(ID3D11Device* device)
    {
        D3D11_BUFFER_DESC desc = {};
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.ByteWidth = sizeof(T);
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        return SUCCEEDED(device->CreateBuffer(&desc, nullptr, m_buffer.GetAddressOf()));
    } // Init


    bool Update(ID3D11DeviceContext* context, const T& data)
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        if (FAILED(context->Map(m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
            return false;

        CopyMemory(mappedResource.pData, &data, sizeof(T));
        context->Unmap(m_buffer.Get(), 0);
        return true;
    } // Update


    void BindVS(ID3D11DeviceContext* context, unsigned int slot)
    {
        context->VSSetConstantBuffers(slot, 1, m_buffer.GetAddressOf());
    } // BindVS


    void BindPS(ID3D11DeviceContext* context, unsigned int slot)
    {

        if (context == nullptr) return;

        if (m_buffer.Get() == nullptr) {
            OutputDebugStringA("Error: ConstantBuffer m_buffer is null!\n");
            return;
        }

        context->PSSetConstantBuffers(slot, 1, m_buffer.GetAddressOf());
    } // BindPS


    void BindGS(ID3D11DeviceContext* context, unsigned int slot)
    {
        if (context == nullptr || m_buffer.Get() == nullptr) return;
        context->GSSetConstantBuffers(slot, 1, m_buffer.GetAddressOf());
    } // BindGS

private:
    ComPtr<ID3D11Buffer> m_buffer;

}; // ConstantBuffer