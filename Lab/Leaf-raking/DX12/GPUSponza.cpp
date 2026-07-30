#include "Pch.h"
#include "GPUSponza.h"
#include "d3dx12.h"
// Core
#include "RendererState.h"
// Components
#include "RenderQueue.h"
#include "DescriptorHeapAllocator.h"
#include "TextureManager.h"
#include "Data/Texture.h"
// Utils
#include "DebugHelper.h"
#include "GPUCommons.h"

using namespace Microsoft::WRL;
using namespace DirectX;
using namespace DebugHelper;

GPUSponza::GPUSponza() : m_enableWireframe(false) {
    m_worldMatrix = XMMatrixIdentity();
    m_psoSolidCull = nullptr;
    m_psoSolidNoCull = nullptr;
    m_psoWireCull = nullptr;
    m_psoWireNoCull = nullptr;
    m_heapAllocator = nullptr;

    m_loader = std::make_unique<GPUAssimpLoader>();
    m_gpuModel = std::make_unique<GPUModel>();
} // GPUSponza

GPUSponza::~GPUSponza() {
} // ~GPUSponza

bool GPUSponza::Init(const InitParams& params) {
    if (!params.device || !params.uploadCmdList || !params.rootSignature ||
        !params.textureManager || !params.heapAllocator) {
        DebugPrint("GPUSponza::Init - 잘못된 파라미터");
        return false;
    }

    m_psoSolidCull = params.psoSolidCull;
    m_psoSolidNoCull = params.psoSolidNoCull;
    m_psoWireCull = params.psoWireCull;
    m_psoWireNoCull = params.psoWireNoCull;
    m_heapAllocator = params.heapAllocator;

    if (!m_loader->Init(params.textureManager, params.modelPath)) {
        DebugPrint("GPUSponza: 모델 로드 실패");
        return false;
    }

    GPUModel::InitParams gpuParams;
    gpuParams.device = params.device;
    gpuParams.uploadCmdList = params.uploadCmdList;
    gpuParams.rootSignature = params.rootSignature;
    gpuParams.globalVertices = &m_loader->GetGlobalVertices();
    gpuParams.globalIndices = &m_loader->GetGlobalIndices();
    gpuParams.meshDataList = &m_loader->GetMeshDataList();

    if (!m_gpuModel->Init(gpuParams, m_vertexUpload, m_indexUpload, m_indirectUpload, m_meshDataUpload)) {
        DebugPrint("GPUSponza: GPUModel 초기화 실패");
        return false;
    }

    if (!CreateMaterialBuffer(params.device, params.uploadCmdList)) {
        return false;
    }

    return true;
} // Init

bool GPUSponza::CreateMaterialBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* uploadCmdList) {
    const auto& materials = m_loader->GetMaterials();
    if (materials.empty()) return true;

    std::vector<GPUCommons::GPUMaterialData> matDataList(materials.size());
    for (size_t i = 0; i < materials.size(); ++i) {
        const auto& mat = materials[i];
        matDataList[i].albedoFactor = mat.albedoFactor;
        matDataList[i].metallicFactor = mat.metallicFactor;
        matDataList[i].roughnessFactor = mat.roughnessFactor;
        matDataList[i].emissiveFactor = mat.emissiveFactor;
        matDataList[i].alphaCutoff = mat.alphaFactor;

        // Bindless 텍스처 인덱스 획득
        matDataList[i].albedoTexIdx = mat.albedo ? mat.albedo->GetSRVIndex() : UINT_MAX;
        matDataList[i].normalTexIdx = mat.normal ? mat.normal->GetSRVIndex() : UINT_MAX;
        matDataList[i].alphaTexIdx = mat.alpha ? mat.alpha->GetSRVIndex() : UINT_MAX;
        matDataList[i].roughnessTexIdx = mat.roughness ? mat.roughness->GetSRVIndex() : UINT_MAX;
    }

    const UINT matBufferSize = static_cast<UINT>(sizeof(GPUCommons::GPUMaterialData) * matDataList.size());
    CD3DX12_HEAP_PROPERTIES defaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);
    CD3DX12_HEAP_PROPERTIES uploadHeapProps(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC matBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(matBufferSize);

    if (FAILED(device->CreateCommittedResource(&defaultHeapProps, D3D12_HEAP_FLAG_NONE, &matBufferDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_materialBuffer)))) {
        return false;
    }
    if (FAILED(device->CreateCommittedResource(&uploadHeapProps, D3D12_HEAP_FLAG_NONE, &matBufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_materialUpload)))) {
        return false;
    }

    D3D12_SUBRESOURCE_DATA matData = {};
    matData.pData = matDataList.data();
    matData.RowPitch = matBufferSize;
    matData.SlicePitch = matBufferSize;
    UpdateSubresources(uploadCmdList, m_materialBuffer.Get(), m_materialUpload.Get(), 0, 0, 1, &matData);

    CD3DX12_RESOURCE_BARRIER matBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_materialBuffer.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE
    );
    uploadCmdList->ResourceBarrier(1, &matBarrier);

    return true;
} // CreateMaterialBuffer

void GPUSponza::Submit(RenderQueue* renderQueue) {
    if (!renderQueue || m_gpuModel->GetMeshCount() == 0) {
        return;
    }

    DrawCommand cmd{};
    cmd.sortKey = 0;

    cmd.pso = m_enableWireframe ? m_psoWireCull : m_psoSolidNoCull;

    cmd.execute = [this](ID3D12GraphicsCommandList* cmdList) {
        DirectX::XMMATRIX transposedWorld = XMMatrixTranspose(m_worldMatrix);

        cmdList->SetGraphicsRoot32BitConstants(
            RendererState::WorldIndex,
            sizeof(XMMATRIX) / 4,
            &transposedWorld,
            0
        );

         cmdList->SetGraphicsRootShaderResourceView(RendererState::VertexBufferIndex, m_gpuModel->GetVertexBufferAddress());
         cmdList->SetGraphicsRootShaderResourceView(RendererState::MeshDataIndex, m_gpuModel->GetMeshDataBufferAddress());
         cmdList->SetGraphicsRootShaderResourceView(RendererState::MaterialDataIndex, m_materialBuffer->GetGPUVirtualAddress());

        m_gpuModel->RenderIndirect(cmdList);
    };

    renderQueue->Submit(cmd);
} // Submit

const DirectX::XMMATRIX& GPUSponza::GetWorldMatrix() const {
    return m_worldMatrix;
} // GetWorldMatrix

void GPUSponza::OnGUI() {
    ImGui::TextColored(ImVec4(0.8f, 1.0f, 0.6f, 1.0f), "[ GPUSponza Options ]");
    ImGui::Checkbox("Wireframe Mode", &m_enableWireframe);

    ImGui::Separator();
    ImGui::Spacing();

    ImGui::TextDisabled("Resource Info (GPU Driven)");
    ImGui::Text("Total Meshes: "); ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%u", m_gpuModel->GetMeshCount());

    ImGui::Separator();
    ImGui::Spacing();

    ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "MATERIALS STATUS");
    ImGui::Spacing();

    const auto& materials = m_loader->GetMaterials();
    for (size_t i = 0; i < materials.size(); ++i) {
        const auto& mat = materials[i];

        if (ImGui::TreeNode((void*)(intptr_t)i, "Material [%d]: %s", (int)i, mat.name.c_str())) {
            ImGui::BeginGroup();

            auto ShowStatus = [](const char* type, bool isLoaded) {
                ImGui::Text("%-12s:", type);
                ImGui::SameLine();
                if (isLoaded) {
                    ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), " [ LOADED ]");
                }
                else {
                    ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), " [ MISSING ]");
                }
                };

            ShowStatus("Albedo", mat.albedo != nullptr);
            ShowStatus("Normal", mat.normal != nullptr);
            ShowStatus("Alpha", mat.alpha != nullptr);
            ShowStatus("Metallic", mat.metallic != nullptr);
            ShowStatus("Roughness", mat.roughness != nullptr);

            ImGui::EndGroup();
            ImGui::TreePop();
        }
    }
} // OnGUI