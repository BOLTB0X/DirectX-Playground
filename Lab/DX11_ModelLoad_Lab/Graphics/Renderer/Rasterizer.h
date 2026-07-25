// Graphics/Renderer/Rasterizer.h
// 여러 종류의 래스터라이저 상태 생성
#pragma once
#include <d3d11.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class Rasterizer {
public:
    Rasterizer();
	Rasterizer(const Rasterizer&) = delete;
    ~Rasterizer();

    bool Init(ID3D11Device* device);

    void Bind(ID3D11DeviceContext* context, bool wireframe = false, bool cullNone = false);

private:
    ComPtr<ID3D11RasterizerState> m_solidState;
    ComPtr<ID3D11RasterizerState> m_wireframeState;
    ComPtr<ID3D11RasterizerState> m_noCullingState;
}; // Rasterizer