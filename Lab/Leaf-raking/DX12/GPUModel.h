#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <vector>
#include <DirectXMath.h>
#include <string>
// Utils
#include "Utils/SharedCommons.h"

class GPUModel {
public:
    struct PBRVertex {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT2 texture;
        DirectX::XMFLOAT3 normal;
        DirectX::XMFLOAT3 tangent;
        DirectX::XMFLOAT3 binormal;

        PBRVertex() : position(0.0f, 0.0f, 0.0f), texture(0.0f, 0.0f), normal(0.0f, 0.0f, 0.0f),
            tangent(0.0f, 0.0f, 0.0f), binormal(0.0f, 0.0f, 0.0f) {
        }
    }; // PBRVertex

    struct InitParams {
        ID3D12Device*                                  device;
        ID3D12GraphicsCommandList*                     uploadCmdList;
        ID3D12RootSignature*                           rootSignature;
        const std::vector<PBRVertex>*                  globalVertices;
        const std::vector<unsigned int>*               globalIndices;
        const std::vector<SharedCommons::GPUMeshData>* meshDataList;

        InitParams() : device(nullptr), uploadCmdList(nullptr), rootSignature(nullptr),
            globalVertices(nullptr), globalIndices(nullptr), meshDataList(nullptr) {
        }
    }; // InitParams

public:
    GPUModel();
    GPUModel(const GPUModel&) = delete;
    GPUModel& operator=(const GPUModel&) = delete;
    ~GPUModel();

    bool Init(const InitParams&,
        Microsoft::WRL::ComPtr<ID3D12Resource>&,
        Microsoft::WRL::ComPtr<ID3D12Resource>&,
        Microsoft::WRL::ComPtr<ID3D12Resource>&,
        Microsoft::WRL::ComPtr<ID3D12Resource>&);
    void RenderIndirect(ID3D12GraphicsCommandList*);
    UINT GetMeshCount() const;

    D3D12_GPU_VIRTUAL_ADDRESS GetVertexBufferAddress() const;
    D3D12_GPU_VIRTUAL_ADDRESS GetMeshDataBufferAddress() const;

private:
    Microsoft::WRL::ComPtr<ID3D12Resource>         m_globalVertexBuffer;
    Microsoft::WRL::ComPtr<ID3D12Resource>         m_globalIndexBuffer;
    Microsoft::WRL::ComPtr<ID3D12Resource>         m_indirectCommandBuffer;
    Microsoft::WRL::ComPtr<ID3D12Resource>         m_meshDataBuffer;
    Microsoft::WRL::ComPtr<ID3D12CommandSignature> m_commandSignature;
    D3D12_INDEX_BUFFER_VIEW                        m_ibView;
    UINT                                           m_meshCount;
}; // GPUModel