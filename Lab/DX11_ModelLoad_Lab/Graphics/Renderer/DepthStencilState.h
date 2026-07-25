// Graphics/Renderer/DepthStencilState.h
// 깊이 판정을 켜고 끄거나, 2D UI 렌더링을 위해 깊이 쓰기를 제한하는 등의 상태를 관리
#pragma once
#include <d3d11.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class DepthStencilState {
public:
    DepthStencilState();
	DepthStencilState(const DepthStencilState&) = delete;
    ~DepthStencilState();

    bool Init(ID3D11Device*);
    bool InitForParticle(ID3D11Device* device);
    void Bind(ID3D11DeviceContext*, bool depthEnable = true);

private:
    ComPtr<ID3D11DepthStencilState> m_depthEnabledState;
    ComPtr<ID3D11DepthStencilState> m_depthDisabledState;
}; // DepthStencilState
