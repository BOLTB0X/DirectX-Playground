#pragma once
#include "DefaultModelBuffer.h"

class Position;

class DefaultModel {
public:
    DefaultModel();
    DefaultModel(const DefaultModel& other) = delete;
    ~DefaultModel();

    bool Init(ID3D11Device*, DefaultModelType);
    void Shutdown();
    void Render(ID3D11DeviceContext*);

public:
    DirectX::XMMATRIX GetModelMatrix();
    void SetPosition(DirectX::XMFLOAT3);
    void SetScale(float);
    void SetScale(float, float, float);
	void SetScale(DirectX::XMFLOAT3);
    void SetRotation(DirectX::XMFLOAT3);

private:
    std::unique_ptr<DefaultModelBuffer> m_ModelBuffer;
    std::unique_ptr<Position> m_Position;
}; // DefaultModel