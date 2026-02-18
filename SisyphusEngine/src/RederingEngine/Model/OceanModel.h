#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <memory>


class GridModelBuffer;
class QuadTree;
class Frustum;
class OceanShader;


class OceanModel {
public:
    OceanModel();
	OceanModel(const OceanModel& other) = delete;
    ~OceanModel();

    bool Init(ID3D11Device*, int, int, float);
    void Shutdown();
    void Render(Frustum*, ID3D11DeviceContext*, OceanShader*);

    DirectX::XMMATRIX GetWorldMatrix();

private:
    std::unique_ptr<GridModelBuffer> m_modelBuffer;
    std::unique_ptr<QuadTree> m_quadTree;

    DirectX::XMMATRIX m_worldMatrix;
}; // OceanModel