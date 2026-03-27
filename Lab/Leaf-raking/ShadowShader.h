#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <directxmath.h>

class ShadowShader {
public:
    struct InitParams {
        ID3D11Device*       device;
        HWND                hwnd;
        ID3D11SamplerState* clampSampler;
        ID3D11SamplerState* wrapSampler;
        DirectX::XMMATRIX   worldMatrix;
        DirectX::XMMATRIX   viewMatrix;
        DirectX::XMMATRIX   projectionMatrix;
        DirectX::XMMATRIX   lightViewMatrix;
        DirectX::XMMATRIX   lightProjectionMatrix;
        DirectX::XMFLOAT4   ambientColor;
        DirectX::XMFLOAT4   diffuseColor;
        DirectX::XMFLOAT3   lightPosition;
    }; // InitParams

    struct RenderParams {
        DirectX::XMMATRIX         worldMatrix;
        DirectX::XMMATRIX         viewMatrix;
        DirectX::XMMATRIX         projectionMatrix;
        DirectX::XMMATRIX         lightViewMatrix;
        DirectX::XMMATRIX         lightProjectionMatrix;
        DirectX::XMFLOAT4         ambientColor;
        DirectX::XMFLOAT4         diffuseColor;
        DirectX::XMFLOAT3         lightPosition;
        ID3D11ShaderResourceView* depthMapTexture;
    }; // RenderParams

public:
	ShadowShader();
	~ShadowShader();

	bool Init(const InitParams&);
	bool Render(ID3D11DeviceContext*, const RenderParams&);

private:
    struct MatrixBuffer {
        DirectX::XMMATRIX world;
        DirectX::XMMATRIX view;
        DirectX::XMMATRIX projection;
        DirectX::XMMATRIX lightView;
        DirectX::XMMATRIX lightProjection;
    }; // MatrixBuffer

    struct LightPositionBuffer {
        DirectX::XMFLOAT3 lightPosition;
        float             padding;
    }; // LightPositionBuffer

    struct LightBuffer {
        DirectX::XMFLOAT4 ambientColor;
        DirectX::XMFLOAT4 diffuseColor;
        float             bias;
        DirectX::XMFLOAT3 padding;
    }; // LightBuffer

private:
    bool InitShader(ID3D11Device*, HWND);
    bool UpdateMatrixBuffer(ID3D11DeviceContext*, const DirectX::XMMATRIX&, const DirectX::XMMATRIX&, const DirectX::XMMATRIX&, const DirectX::XMMATRIX&, const DirectX::XMMATRIX&);
    bool UpdateLightPositionBuffer(ID3D11DeviceContext*, const DirectX::XMFLOAT3&);
    bool UpdateLightBuffer(ID3D11DeviceContext*, const DirectX::XMFLOAT4&, const DirectX::XMFLOAT4&, const float&);

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>  m_pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>  m_layout;
    // Constant Buffers
    Microsoft::WRL::ComPtr<ID3D11Buffer>       m_matrixBuffer;        // VS b0
    Microsoft::WRL::ComPtr<ID3D11Buffer>       m_lightPositionBuffer; // VS b1
    Microsoft::WRL::ComPtr<ID3D11Buffer>       m_lightBuffer;         // PS b0

    MatrixBuffer                               m_prevMatrixBufferData;
    LightPositionBuffer                        m_prevLightPositionBufferData;
    LightBuffer                                m_prevLightBufferData;
    ID3D11SamplerState*                        m_clampSampler;
    ID3D11SamplerState*                        m_wrapSampler;
    float                                      m_bias;
}; // ShadowShader