#pragma once
#include <d3d12.h>
#include <functional>
#include <vector>

struct RenderContext;

class RenderPipeline {
public:
    using Pass = std::function<void(RenderContext&, ID3D12GraphicsCommandList*)>;

    void AddPass(Pass);
    void Clear();
    void RenderPass(RenderContext&, ID3D12GraphicsCommandList*);

private:
    std::vector<Pass> m_passes;
}; // RenderPipeline
