// Graphics/Renderer/BlendState.h
// 투명도 처리를 켠 상태(AlphaBlend)와 끈 상태(Opaque)를 관리
#pragma once
#include <d3d11.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class BlendState {
public:
    BlendState();
	BlendState(const BlendState&) = delete;
    ~BlendState();

    bool Init(ID3D11Device*);
    bool InitForParticle(ID3D11Device* device);
    void Bind(ID3D11DeviceContext* context, bool alphaBlend = false);

private:
    ComPtr<ID3D11BlendState> m_alphaEnableState;
    ComPtr<ID3D11BlendState> m_alphaDisableState;
    ComPtr<ID3D11BlendState> m_alphaToCoverageState;
}; // BlendState