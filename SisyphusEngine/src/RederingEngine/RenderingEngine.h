#pragma once
// STL
#include <d3d11.h>
#include <memory>
#include <windows.h>
#include <DirectXMath.h>
#include <string>
// Common
#include "PropertyHelper.h"
// Rendering
#include "Shader/ShaderBuffersManager.h"


class Renderer;
class RenderTexture;
class TexturesManager;
class ShaderManager;
class DefaultModel;
class Light;
class ShaderBuffersManager;


class RenderingEngine {
public:
	RenderingEngine();
	RenderingEngine(const RenderingEngine& other) = delete;
	~RenderingEngine();

	bool Init(HWND);
	void Shutdown();

	void BeginScene(float, float, float, float);
	void EndScene();

public:
	/// <summary>
	///  [ImGui]
	void SetMode(bool, bool);
	void SetDepthBuffer(bool);
	void SetWireframeEnable(bool);
	void SetBackCullEnable(bool);
	void SetDepthEnable(bool);

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();
	bool GetWireframeEnable() const;
	bool GetBackCullEnable() const;
	bool GetDepthEnable() const;

	template<typename T>
	PropertyHelper::Property<T> DeliveryBuffer(const std::string& key)
	{
		return m_BuffersManager->DeliveryBuffer<T>(key);
	} // DeliveryBuffer
	/// </summary>


	/// <summary>
	// [Rendering]
	void Draw(float,
		PropertyHelper::Property<DirectX::XMMATRIX>,
		PropertyHelper::Property<DirectX::XMMATRIX>,
		PropertyHelper::Property<DirectX::XMFLOAT3>,
		PropertyHelper::Property<float>);

private:
	void DrawSky(ID3D11DeviceContext*, float, DirectX::XMFLOAT3, DirectX::XMMATRIX, DirectX::XMMATRIX, float);
	void DrawOcean(ID3D11DeviceContext*, float, DirectX::XMFLOAT3, DirectX::XMMATRIX, DirectX::XMMATRIX, float);
	void DrawCloud(ID3D11DeviceContext*, float, DirectX::XMFLOAT3, DirectX::XMMATRIX, DirectX::XMMATRIX, float);

	void ApplyReflection(float, DirectX::XMFLOAT3, DirectX::XMMATRIX, DirectX::XMMATRIX, float);
	void ApplyRefraction(float, DirectX::XMFLOAT3, DirectX::XMMATRIX, DirectX::XMMATRIX);
	void ApplyBicubicUpscale(ID3D11DeviceContext*);
	void ApplyLensFlare(ID3D11DeviceContext*, const DirectX::XMMATRIX&, const DirectX::XMMATRIX&, const DirectX::XMFLOAT3&);
	/// </summary>

private:
	std::unique_ptr<Renderer> m_Renderer;
	std::unique_ptr<RenderTexture> m_RenderTexture;
	std::unique_ptr<TexturesManager> m_TexturesManager;
	std::unique_ptr<ShaderManager> m_ShaderManager;
	std::unique_ptr<DefaultModel> m_Cloud;
	std::unique_ptr<DefaultModel> m_Screen;
	std::unique_ptr<DefaultModel> m_Sky;
	std::unique_ptr<DefaultModel> m_Ocean;
	std::unique_ptr<Light> m_Sun;
	std::unique_ptr<ShaderBuffersManager> m_BuffersManager;

	bool m_isWireframe;
	bool m_backCullEnable;
	bool m_depthEnable;
	uint32_t m_frameCount;
}; // RenderingEngine