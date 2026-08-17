#pragma once
#include <d3d12.h>
#include <wrl/client.h>
// Utils
#include "GPUCommons.h"

class RenderTexture;
class D3D12PipelineState;

class DepthRecorder {
public:
    struct InitParams {
        ID3D12Device*        device;
        ID3D12RootSignature* rootSignature;
        D3D12PipelineState*  solidDepthPSO;
        D3D12PipelineState*  alphaDepthPSO;

        InitParams() : device(nullptr), rootSignature(nullptr),
            solidDepthPSO(nullptr), alphaDepthPSO(nullptr) {
        }
	}; // InitParams

    struct RecordParams {
        ID3D12GraphicsCommandList*  cmdList;
        RenderTexture*              depthTexture;
        D3D12_GPU_VIRTUAL_ADDRESS   frameConstantsGPUAddress;
        D3D12_GPU_VIRTUAL_ADDRESS   lightConstantsGPUAddress;
        D3D12_GPU_DESCRIPTOR_HANDLE instanceDataGPUHandle;
        D3D12_GPU_DESCRIPTOR_HANDLE bindlessBufGPUHandle;
        D3D12_GPU_DESCRIPTOR_HANDLE bindlessTexGPUHandle;

        ID3D12Resource*             mainIndirectBuffer;
        ID3D12Resource*             mainCounterBuffer;
        UINT                        maxMainCount;
        ID3D12Resource*             vaseIndirectBuffer;
        ID3D12Resource*             vaseCounterBuffer;
        UINT                        maxVaseCount;

        RecordParams() : cmdList(nullptr), depthTexture(nullptr),
            frameConstantsGPUAddress(0), lightConstantsGPUAddress(0),
            instanceDataGPUHandle{}, bindlessBufGPUHandle{}, bindlessTexGPUHandle{},
            mainIndirectBuffer(nullptr), mainCounterBuffer(nullptr), maxMainCount(0),
            vaseIndirectBuffer(nullptr), vaseCounterBuffer(nullptr), maxVaseCount(0) {
        }
    }; // RecordParams

public:
    DepthRecorder();
    DepthRecorder(const DepthRecorder&) = delete;
    DepthRecorder& operator=(const DepthRecorder&) = delete;
    ~DepthRecorder();

    bool Init(const InitParams&);
    void RecordDepthPre(const RecordParams&);
	//void RecordShadowMap(const RecordParams&); // TODO

private:
    Microsoft::WRL::ComPtr<ID3D12CommandSignature> m_commandSignature;
    ID3D12RootSignature*                           m_rootSignature;
    D3D12PipelineState*                            m_solidDepthPSO;
    D3D12PipelineState*                            m_alphaDepthPSO;
}; // DepthRecorder