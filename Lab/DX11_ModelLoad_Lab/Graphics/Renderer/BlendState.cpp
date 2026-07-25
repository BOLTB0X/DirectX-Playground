// Graphics/Renderer/BlendState.cpp
//  Color = (SrcColor * SrcAlpha) + (DestColor * (1 - SrcAlpha)) 형태
#include "BlendState.h"
#include "Common/EngineHelper.h"

/* defualt */
/////////////////////////////////////////////////////////////////////

BlendState::BlendState() {}

BlendState::~BlendState() {}


bool BlendState::Init(ID3D11Device* device) {
    D3D11_BLEND_DESC blendDesc = {};
    ZeroMemory(&blendDesc, sizeof(blendDesc));

    // 알파 블렌딩 활성화 설정
    blendDesc.AlphaToCoverageEnable = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

    if (EngineHelper::SuccessCheck(device->CreateBlendState(&blendDesc, &m_alphaEnableState),
        "AlphaEnable BlendState 생성 실패")
        == false) return false;

    // 알파 블렌딩 비활성화 설정
    blendDesc.RenderTarget[0].BlendEnable = FALSE;
	blendDesc.AlphaToCoverageEnable = FALSE;
    if (EngineHelper::SuccessCheck(device->CreateBlendState(&blendDesc, &m_alphaDisableState),
        "AlphaDisable BlendState 생성 실패")
        == false) return false;

    // 알파-커버리지 블렌딩 모드에 대한 블렌딩 상태 description을 생성
    blendDesc.AlphaToCoverageEnable = TRUE;
    blendDesc.IndependentBlendEnable = FALSE;
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

    if (EngineHelper::SuccessCheck(device->CreateBlendState(&blendDesc, &m_alphaToCoverageState),
        "AlphaEnable BlendState 생성 실패")
        == false) return false;
    return true;
} // Init


bool BlendState::InitForParticle(ID3D11Device* device) {
    D3D11_BLEND_DESC blendDesc = {};
    ZeroMemory(&blendDesc, sizeof(blendDesc));

    blendDesc.AlphaToCoverageEnable = FALSE;
    blendDesc.IndependentBlendEnable = FALSE;

    blendDesc.RenderTarget[0].BlendEnable = TRUE;

    // 최종 색상 = (새 색상 * 새 알파) + (기존 색상 * (1 - 새 알파))
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

    // 알파 채널도 혼합되도록 설정
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    return EngineHelper::SuccessCheck(device->CreateBlendState(&blendDesc, &m_alphaEnableState),
        "Particle 전용 BlendState 생성 실패");
} // InitForParticle


void BlendState::Bind(ID3D11DeviceContext* context, bool alphaBlend) {
    float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    if (alphaBlend) {
        context->OMSetBlendState(m_alphaEnableState.Get(), blendFactor, 0xffffffff);
    }
    else {
        context->OMSetBlendState(m_alphaDisableState.Get(), blendFactor, 0xffffffff);
    }
} // Bind