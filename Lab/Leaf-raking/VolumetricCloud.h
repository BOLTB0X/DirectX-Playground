#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <memory>
#include <wrl/client.h>
#include "Resources/DefaultMesh.h"
#include "Resources/ConstantBufferType.h"

class NoiseGenerator;
class VolumeTexture;
class TextureManager;
class Texture;

class VolumetricCloud {
public:
    struct RenderParams {
        DirectX::XMMATRIX view;
        DirectX::XMMATRIX projection;
        DirectX::XMFLOAT3 camPos;

        DirectX::XMFLOAT3 planetCenter;
        float planetRadius;
        float cloudMinHeight;
        float cloudMaxHeight;

        DirectX::XMFLOAT4 diffuse;
        DirectX::XMFLOAT3 LightDir;
    }; // RenderParams

    struct alignas(16) CloudBuffer {
        // Row1
        DirectX::XMFLOAT3 planetCenter;
        float             planetRadius;
        // Row2
        float             cloudMinHeight;
        float             cloudMaxHeight;
        float             time;
        float             padding;

        CloudBuffer() {
            planetCenter = { 0.0f, 0.0f, 0.0f };
            planetRadius = 0.0f;
            cloudMinHeight = 0.0f;
            cloudMaxHeight = 0.0f;
            time = 0.0f;
            padding = 0.0f;
        }
    }; // CloudBuffer

    VolumetricCloud();
    ~VolumetricCloud();

    bool Init(ID3D11Device*, ID3D11DeviceContext*, HWND, ID3D11SamplerState*, ID3D11SamplerState*, TextureManager*);

    void Render(ID3D11DeviceContext*, const RenderParams&);

private:
    bool InitShader(ID3D11Device* device, HWND hwnd);
    bool UpdateCameraBuffer(ID3D11DeviceContext*, const DirectX::XMMATRIX&, const DirectX::XMMATRIX&, const DirectX::XMFLOAT3&);
    bool UpdateLightBuffer(ID3D11DeviceContext*, const DirectX::XMFLOAT4&, const DirectX::XMFLOAT3&);
	bool UpdateCloudBuffer(ID3D11DeviceContext*, const DirectX::XMFLOAT3&, const float&, const float&, const float&);
    bool RenderShader(ID3D11DeviceContext*, const RenderParams&);
    void GenerateCloudNoise(ID3D11DeviceContext*);

private:
    std::unique_ptr<DefaultMesh>    m_mesh;
    std::unique_ptr<VolumeTexture>  m_VolumeTexture;
    std::unique_ptr<NoiseGenerator> m_NoiseGenerator;
    std::shared_ptr<Texture>        m_WeatherMap;
    TextureManager*                 m_textureMgr;

    // shader resources
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>  m_pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>  m_layout;
    Microsoft::WRL::ComPtr<ID3D11Buffer>       m_cloudBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer>       m_cameraVSBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer>       m_cameraPSBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer>       m_lightBuffer;
    ID3D11SamplerState*                        m_wSampler;
    ID3D11SamplerState*                        m_cSampler;

    ConstantBuffer::CameraBuffer               m_CameraVSData;
    ConstantBuffer::CameraBuffer               m_CameraPSData;
    ConstantBuffer::LightBuffer                m_LightData;
	CloudBuffer                                m_CloudData;

    ConstantBuffer::CameraBuffer               m_prevCameraVSData;
    ConstantBuffer::CameraBuffer               m_prevCameraPSData;
    ConstantBuffer::LightBuffer                m_prevLightData;
    CloudBuffer                                m_prevCloudData;

    float                                      m_renderCount;
}; // VolumetricCloud