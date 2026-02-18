#include "Pch.h"
#include "OceanModel.h"
#include "GridModelBuffer.h"
#include "QuadTree.h"
#include "Frustum.h"
//#include "OceanShader.h"


using namespace DirectX;


OceanModel::OceanModel()
{
    m_modelBuffer = std::make_unique<GridModelBuffer>();
	m_quadTree = std::make_unique<QuadTree>();
    m_worldMatrix = XMMatrixIdentity();
} // OceanModel


OceanModel::~OceanModel()
{
    Shutdown();
} // ~OceanModel


bool OceanModel::Init(ID3D11Device* device, int gridWidth, int gridHeight, float gridScale)
{
    if (m_modelBuffer->Init(device, gridWidth, gridHeight, gridScale)
        == false)
    {
        return false;
    }

    if (m_quadTree->Init(m_modelBuffer.get(), device)
        == false)
    {
        return false;
    }

    return true;
} // Init


void OceanModel::Shutdown()
{

    if (m_quadTree)
        m_quadTree->Shutdown();

    if (m_modelBuffer)
        m_modelBuffer->Shutdown();
} // Shutdown


DirectX::XMMATRIX OceanModel::GetWorldMatrix()
{
    return m_worldMatrix;
} // GetWorldMatrix


void OceanModel::Render(Frustum* frustum, ID3D11DeviceContext* context, OceanShader* shader)
{
    if (m_quadTree)
    {
        m_quadTree->Render(frustum, context, shader);
    }
} // Render
