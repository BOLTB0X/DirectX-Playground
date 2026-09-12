#pragma once

class D3D12Device;
class CommandQueue;
class D3D12SwapChain;
class DescriptorHeapAllocator;
class RenderTextureManager;
class RenderTarget;
class PSOManager;
class RendererState;
class RendererDebugger;
class RenderQueue;
class Camera;
class DirectionalLight;
class GPUMonitor;
class TextureManager;
class ImGuiManager;
class FrustumCuller;
class ShadowFrustumCuller;
class HierarchicalZBuffer;
class OcclusionCuller;
class CascadedShadowMap;
class EnvironmentProbe;
class BRDFIntegrationLUT;
class PrefilterEnvironment;
class IrradianceConvolution;
class Sponza;

struct RenderContext {
    D3D12Device*             d3d12Device = nullptr;
    CommandQueue*            commandQueue = nullptr;
    D3D12SwapChain*          swapChain = nullptr;
    DescriptorHeapAllocator* descriptorAllocator = nullptr;
    RenderTextureManager*    renderTextureManager = nullptr;
    RenderTarget*            sceneRenderTarget = nullptr;
    PSOManager*              psoManager = nullptr;

    RendererState*           rendererState = nullptr;
    RendererDebugger*        rendererDebugger = nullptr;
    RenderQueue*             renderQueue = nullptr;
    Camera*                  sceneCamera = nullptr;
    Camera*                  masterCamera = nullptr;
    DirectionalLight*        directionalLight = nullptr;
    GPUMonitor*              gpuMonitor = nullptr;
    TextureManager*          textureManager = nullptr;
    ImGuiManager*            imGuiManager = nullptr;

    FrustumCuller*           frustumCuller = nullptr;
    ShadowFrustumCuller*     shadowFrustumCuller = nullptr;
    HierarchicalZBuffer*     hierarchicalZBuffer = nullptr;
    OcclusionCuller*         occlusionCuller = nullptr;
    CascadedShadowMap*       cascadedShadowMap = nullptr;
    EnvironmentProbe*        environmentProbe = nullptr;
    BRDFIntegrationLUT*      brdfIntegrationLUT = nullptr;
    PrefilterEnvironment*    prefilterEnvironment = nullptr;
    IrradianceConvolution*   irradianceConvolution = nullptr;
    Sponza*                  sponza = nullptr;
}; // RenderContext
