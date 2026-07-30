#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <string>
#include <memory>
// Resources
#include "GPUModel.h"
#include "GPUAssimpLoader.h"
// Utils
#include "Utils/SharedCommons.h"

class RenderQueue;
class DescriptorHeapAllocator;
class TextureManager;

class GPUSponza {
public:
    struct InitParams {
        ID3D12Device* device = nullptr;
        ID3D12GraphicsCommandList* uploadCmdList = nullptr;
        ID3D12RootSignature* rootSignature = nullptr;
        std::shared_ptr<TextureManager>  textureManager = nullptr;
        std::string                      modelPath = "";

        ID3D12PipelineState* psoSolidCull = nullptr;
        ID3D12PipelineState* psoSolidNoCull = nullptr;
        ID3D12PipelineState* psoWireCull = nullptr;
        ID3D12PipelineState* psoWireNoCull = nullptr;
        DescriptorHeapAllocator* heapAllocator = nullptr;
    }; // InitParams

public:
    GPUSponza();
    virtual ~GPUSponza();
    GPUSponza(const GPUSponza&) = delete;
    GPUSponza& operator=(const GPUSponza&) = delete;

    bool                     Init(const InitParams&);
    void                     Submit(RenderQueue*);

    const DirectX::XMMATRIX& GetWorldMatrix() const;
    void                     OnGUI();

private:
    bool CreateMaterialBuffer(ID3D12Device*, ID3D12GraphicsCommandList*);

private:
    DirectX::XMMATRIX                               m_worldMatrix;
    ID3D12PipelineState* m_psoSolidCull;
    ID3D12PipelineState* m_psoSolidNoCull;
    ID3D12PipelineState* m_psoWireCull;
    ID3D12PipelineState* m_psoWireNoCull;
    DescriptorHeapAllocator* m_heapAllocator;
    bool                                            m_enableWireframe;

    std::unique_ptr<GPUAssimpLoader>                m_loader;
    std::unique_ptr<GPUModel>                       m_gpuModel;
    Microsoft::WRL::ComPtr<ID3D12Resource>          m_materialBuffer;
    Microsoft::WRL::ComPtr<ID3D12Resource>          m_vertexUpload;
    Microsoft::WRL::ComPtr<ID3D12Resource>          m_indexUpload;
    Microsoft::WRL::ComPtr<ID3D12Resource>          m_indirectUpload;
    Microsoft::WRL::ComPtr<ID3D12Resource>          m_meshDataUpload;
    Microsoft::WRL::ComPtr<ID3D12Resource>          m_materialUpload;
}; // GPUSponza