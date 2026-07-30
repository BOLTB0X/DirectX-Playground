#include "Pch.h"
#include "GPUModel.h"
#include "d3dx12.h"
// Utils
#include "DebugHelper.h"

using namespace Microsoft::WRL;
using namespace DebugHelper;

GPUModel::GPUModel()
    : m_ibView{}, m_meshCount(0) {
} // GPUModel

GPUModel::~GPUModel() {
} // ~GPUModel

bool GPUModel::Init(const InitParams& params,
    ComPtr<ID3D12Resource>& outVertexUpload,
    ComPtr<ID3D12Resource>& outIndexUpload,
    ComPtr<ID3D12Resource>& outIndirectUpload,
    ComPtr<ID3D12Resource>& outMeshDataUpload) {

    if (!params.device || !params.uploadCmdList || !params.rootSignature ||
        !params.globalVertices || !params.globalIndices || !params.meshDataList) {
        DebugPrint("GPUModel::Init - 잘못된 파라미터");
        return false;
    }

    m_meshCount = static_cast<UINT>(params.meshDataList->size());
    if (m_meshCount == 0) {
        return true;
    }

    CD3DX12_HEAP_PROPERTIES defaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);
    CD3DX12_HEAP_PROPERTIES uploadHeapProps(D3D12_HEAP_TYPE_UPLOAD);

    // --------------------------------------------------
    // Global Vertex Buffer 생성
    // --------------------------------------------------
    const UINT vbSize = static_cast<UINT>(sizeof(PBRVertex) * params.globalVertices->size());
    CD3DX12_RESOURCE_DESC vbDesc = CD3DX12_RESOURCE_DESC::Buffer(vbSize);

    if (FAILED(params.device->CreateCommittedResource(&defaultHeapProps, D3D12_HEAP_FLAG_NONE, &vbDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_globalVertexBuffer)))) {
        DebugPrint("글로벌 버텍스 버퍼 생성 실패");
        return false;
    }
    if (FAILED(params.device->CreateCommittedResource(&uploadHeapProps, D3D12_HEAP_FLAG_NONE, &vbDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&outVertexUpload)))) {
        return false;
    }

    D3D12_SUBRESOURCE_DATA vbData = {};
    vbData.pData = params.globalVertices->data();
    vbData.RowPitch = vbSize;
    vbData.SlicePitch = vbSize;
    UpdateSubresources(params.uploadCmdList, m_globalVertexBuffer.Get(), outVertexUpload.Get(), 0, 0, 1, &vbData);

    CD3DX12_RESOURCE_BARRIER vbBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_globalVertexBuffer.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
    );
    params.uploadCmdList->ResourceBarrier(1, &vbBarrier);

    // --------------------------------------------------
    // Global Index Buffer 생성 (유지)
    // --------------------------------------------------
    const UINT ibSize = static_cast<UINT>(sizeof(unsigned int) * params.globalIndices->size());
    CD3DX12_RESOURCE_DESC ibDesc = CD3DX12_RESOURCE_DESC::Buffer(ibSize);

    if (FAILED(params.device->CreateCommittedResource(&defaultHeapProps, D3D12_HEAP_FLAG_NONE, &ibDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_globalIndexBuffer)))) {
        DebugPrint("글로벌 인덱스 버퍼 생성 실패");
        return false;
    }
    if (FAILED(params.device->CreateCommittedResource(&uploadHeapProps, D3D12_HEAP_FLAG_NONE, &ibDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&outIndexUpload)))) {
        return false;
    }

    D3D12_SUBRESOURCE_DATA ibData = {};
    ibData.pData = params.globalIndices->data();
    ibData.RowPitch = ibSize;
    ibData.SlicePitch = ibSize;
    UpdateSubresources(params.uploadCmdList, m_globalIndexBuffer.Get(), outIndexUpload.Get(), 0, 0, 1, &ibData);

    CD3DX12_RESOURCE_BARRIER ibBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_globalIndexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER);
    params.uploadCmdList->ResourceBarrier(1, &ibBarrier);

    m_ibView.BufferLocation = m_globalIndexBuffer->GetGPUVirtualAddress();
    m_ibView.Format = DXGI_FORMAT_R32_UINT;
    m_ibView.SizeInBytes = ibSize;

    // --------------------------------------------------
    // GPU Mesh Data Buffer 생성
    // --------------------------------------------------
    const UINT meshDataSize = static_cast<UINT>(sizeof(SharedCommons::GPUMeshData) * m_meshCount);
    CD3DX12_RESOURCE_DESC meshDataDesc = CD3DX12_RESOURCE_DESC::Buffer(meshDataSize);

    if (FAILED(params.device->CreateCommittedResource(&defaultHeapProps, D3D12_HEAP_FLAG_NONE, &meshDataDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_meshDataBuffer)))) {
        DebugPrint("메시 데이터 버퍼 생성 실패");
        return false;
    }
    if (FAILED(params.device->CreateCommittedResource(&uploadHeapProps, D3D12_HEAP_FLAG_NONE, &meshDataDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&outMeshDataUpload)))) {
        return false;
    }

    D3D12_SUBRESOURCE_DATA mdData = {};
    mdData.pData = params.meshDataList->data();
    mdData.RowPitch = meshDataSize;
    mdData.SlicePitch = meshDataSize;
    UpdateSubresources(params.uploadCmdList, m_meshDataBuffer.Get(), outMeshDataUpload.Get(), 0, 0, 1, &mdData);

    CD3DX12_RESOURCE_BARRIER mdBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_meshDataBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
    params.uploadCmdList->ResourceBarrier(1, &mdBarrier);

    // --------------------------------------------------
    // Indirect Command Buffer 생성 및 데이터 구성
    // --------------------------------------------------
    std::vector<SharedCommons::IndirectCommand> commands(m_meshCount);
    for (UINT i = 0; i < m_meshCount; ++i) {
        const auto& meshInfo = (*params.meshDataList)[i];
        commands[i].IndexCountPerInstance = meshInfo.indexCount;
        commands[i].InstanceCount = 1;
        commands[i].StartIndexLocation = meshInfo.indexOffset;
        commands[i].BaseVertexLocation = meshInfo.vertexOffset;
        commands[i].StartInstanceLocation = i;
    }

    const UINT cmdSize = static_cast<UINT>(sizeof(SharedCommons::IndirectCommand) * m_meshCount);
    CD3DX12_RESOURCE_DESC cmdDesc = CD3DX12_RESOURCE_DESC::Buffer(cmdSize);

    if (FAILED(params.device->CreateCommittedResource(&defaultHeapProps, D3D12_HEAP_FLAG_NONE, &cmdDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_indirectCommandBuffer)))) {
        DebugPrint("간접 커맨드 버퍼 생성 실패");
        return false;
    }
    if (FAILED(params.device->CreateCommittedResource(&uploadHeapProps, D3D12_HEAP_FLAG_NONE, &cmdDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&outIndirectUpload)))) {
        return false;
    }

    D3D12_SUBRESOURCE_DATA cmdData = {};
    cmdData.pData = commands.data();
    cmdData.RowPitch = cmdSize;
    cmdData.SlicePitch = cmdSize;
    UpdateSubresources(params.uploadCmdList, m_indirectCommandBuffer.Get(), outIndirectUpload.Get(), 0, 0, 1, &cmdData);

    CD3DX12_RESOURCE_BARRIER cmdBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_indirectCommandBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDIRECT_ARGUMENT);
    params.uploadCmdList->ResourceBarrier(1, &cmdBarrier);

    // --------------------------------------------------
    // Command Signature 생성 (유지)
    // --------------------------------------------------
    D3D12_INDIRECT_ARGUMENT_DESC argDesc = {};
    argDesc.Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW_INDEXED;

    D3D12_COMMAND_SIGNATURE_DESC cmdSigDesc = {};
    cmdSigDesc.ByteStride = sizeof(SharedCommons::IndirectCommand);
    cmdSigDesc.NumArgumentDescs = 1;
    cmdSigDesc.pArgumentDescs = &argDesc;

    if (FAILED(params.device->CreateCommandSignature(&cmdSigDesc, nullptr, IID_PPV_ARGS(&m_commandSignature)))) {
        DebugPrint("커맨드 시그니처 생성 실패");
        return false;
    }

    return true;
} // Init

void GPUModel::RenderIndirect(ID3D12GraphicsCommandList* cmdList) {
    if (m_meshCount == 0) {
        return;
    }

    cmdList->IASetIndexBuffer(&m_ibView);
    cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    cmdList->ExecuteIndirect(
        m_commandSignature.Get(),
        m_meshCount,
        m_indirectCommandBuffer.Get(),
        0,
        nullptr,
        0
    );
} // RenderIndirect

UINT GPUModel::GetMeshCount() const {
    return m_meshCount;
} // GetMeshCount

D3D12_GPU_VIRTUAL_ADDRESS GPUModel::GetVertexBufferAddress() const {
    return m_globalVertexBuffer ? m_globalVertexBuffer->GetGPUVirtualAddress() : 0;
} // GetVertexBufferAddress

D3D12_GPU_VIRTUAL_ADDRESS GPUModel::GetMeshDataBufferAddress() const {
    return m_meshDataBuffer ? m_meshDataBuffer->GetGPUVirtualAddress() : 0;
} // GetMeshDataBufferAddress