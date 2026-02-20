#pragma once
#include <memory>
#include <directxmath.h>
#include <d3d11.h>


class DefaultModelBuffer;
class Position;


class Light {
public:
    Light();
    Light(const Light& other) = delete;
    ~Light();

    void Init(DirectX::XMFLOAT3, DirectX::XMFLOAT4, float);

    void Render(ID3D11DeviceContext*);

public:
    void SetPosition(DirectX::XMFLOAT3);
    void SetScale(float s);
    void SetRotation(DirectX::XMFLOAT3);
	void SetColor(DirectX::XMFLOAT4);
    void SetColor(float, float, float, float a = 1.0f);
    void SetIntensity(float);

    DirectX::XMFLOAT3 GetPosition();
    Position* GetPositionPtr();
    DirectX::XMFLOAT4 GetColor() const;
    float GetIntensity() const;
    DirectX::XMMATRIX GetModelMatrix();
    DirectX::XMFLOAT2 GetModelUV(const DirectX::XMMATRIX&, const DirectX::XMMATRIX&);

private:
    std::unique_ptr<DefaultModelBuffer> m_ModelBuffer;
    std::unique_ptr<Position> m_Position;
    DirectX::XMFLOAT4 m_color;
    float m_intensity;
}; // Light