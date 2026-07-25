// Graphics/Renderer/Rasterizer.cpp
#include "Rasterizer.h"
#include "Common/EngineHelper.h"


/* defualt */
/////////////////////////////////////////////////////////////////////

Rasterizer::Rasterizer() {}

Rasterizer::~Rasterizer() {}


bool Rasterizer::Init(ID3D11Device* device) {
    D3D11_RASTERIZER_DESC rasterDesc = {};
    ZeroMemory(&rasterDesc, sizeof(rasterDesc));

    // 기본 상태
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;

    if (EngineHelper::SuccessCheck(
        device->CreateRasterizerState(&rasterDesc, &m_solidState),
        "Solid RasterizerState 생성 실패")
        == false) return false;

    // 와이어프레임 상태
    rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
    if (EngineHelper::SuccessCheck(
        device->CreateRasterizerState(&rasterDesc, &m_wireframeState),
        "Wireframe RasterizerState 생성 실패")
        == false) return false;

    // 컬링 없는 상태
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.CullMode = D3D11_CULL_NONE;
    if (EngineHelper::SuccessCheck(
        device->CreateRasterizerState(&rasterDesc, &m_noCullingState),
        "NoCulling RasterizerState 생성 실패")
        == false) return false;

    return true;
} // Init


void Rasterizer::Bind(ID3D11DeviceContext* context, bool wireframe, bool cullNone) {
    if (wireframe)
        context->RSSetState(m_wireframeState.Get());
    else if (cullNone)
        context->RSSetState(m_noCullingState.Get());
    else
        context->RSSetState(m_solidState.Get());
} // Bind