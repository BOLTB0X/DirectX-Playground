#pragma once
#include <d3d11.h>
#include <memory>
#include <windows.h>
#include <DirectXMath.h>
// Common
#include "PropertyHelper.h"


class Renderer;
class TexturesManager;
class ShaderManager;
class DefaultModel;
class Light;


class RenderingEngine {
public:
	RenderingEngine();
	RenderingEngine(const RenderingEngine& other) = delete;
	~RenderingEngine();

	bool Init(HWND);
	void Shutdown();

	void Draw(float,
		PropertyHelper::Property<DirectX::XMMATRIX>,
		PropertyHelper::Property<DirectX::XMMATRIX>,
		PropertyHelper::Property<DirectX::XMFLOAT3>);

	void BeginScene(float, float, float, float);
	void EndScene();

public:
	void SetMode(bool, bool);
	void SetDepthBuffer(bool);
	void SetWireframeEnable(bool val) { m_isWireframe = val; }
	void SetBackCullEnable(bool val) { m_backCullEnable = val; }
	void SetDepthEnable(bool val) { m_depthEnable = val; }

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
	bool GetWireframeEnable() const { return m_isWireframe; }
	bool GetBackCullEnable() const { return m_backCullEnable; }
	bool GetDepthEnable() const { return m_depthEnable; }

private:
	void DrawSky(ID3D11DeviceContext*, float, DirectX::XMFLOAT3, DirectX::XMMATRIX, DirectX::XMMATRIX);
	void DrawOcean(ID3D11DeviceContext*, float, DirectX::XMFLOAT3, DirectX::XMMATRIX, DirectX::XMMATRIX);
	void DrawCloud(ID3D11DeviceContext*, float, DirectX::XMFLOAT3, DirectX::XMMATRIX, DirectX::XMMATRIX);

	void ApplyBicubicUpscale(ID3D11DeviceContext*);
	void ApplyLensFlare(ID3D11DeviceContext*, const DirectX::XMMATRIX&, const DirectX::XMMATRIX&, const DirectX::XMFLOAT3&);

	DirectX::XMFLOAT2 CalculateSunUV(const DirectX::XMMATRIX&, const DirectX::XMMATRIX&);
	DirectX::XMMATRIX CalculateLensMatrix(const DirectX::XMMATRIX&);
	DirectX::XMMATRIX CalculateReflectionMatrix(DirectX::XMFLOAT3, float);

private:
	std::unique_ptr<Renderer> m_Renderer;
	std::unique_ptr<TexturesManager> m_TexturesManager;
	std::unique_ptr<ShaderManager> m_ShaderManager;
	std::unique_ptr<DefaultModel> m_Cloud;
	std::unique_ptr<DefaultModel> m_Quad;
	std::unique_ptr<DefaultModel> m_Sky;
	std::unique_ptr<DefaultModel> m_Ocean;
	std::unique_ptr<Light> m_Sun;

	bool m_isWireframe;
	bool m_backCullEnable;
	bool m_depthEnable;
	uint32_t m_frameCount;
}; // RenderingEngine