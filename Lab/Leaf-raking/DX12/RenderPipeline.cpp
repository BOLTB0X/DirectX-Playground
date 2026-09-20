#include "Pch.h"
#include "RenderPipeline.h"

void RenderPipeline::AddPass(Pass pass) {
    m_passes.push_back(std::move(pass));
} // AddPass

void RenderPipeline::Clear() {
    m_passes.clear();
} // Clear

void RenderPipeline::RenderPass(RenderContext& context, ID3D12GraphicsCommandList* commandList) {
    for (const Pass& pass : m_passes) {
        if (pass) {
            pass(context, commandList);
        }
    }
} // RenderPass 