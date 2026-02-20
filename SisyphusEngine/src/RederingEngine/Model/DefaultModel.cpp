#include "Pch.h"
#include "DefaultModel.h"
#include "Position.h"
// Common
#include "ConstantHelper.h"


DefaultModel::DefaultModel()
{
	m_ModelBuffer = std::make_unique<DefaultModelBuffer>();
	m_Position = std::make_unique<Position>();
} // DefaultModel

DefaultModel::~DefaultModel() { }


bool DefaultModel::Init(ID3D11Device* device, DefaultModelType type)
{
    if (m_ModelBuffer->Init(device, type) == false)
        return false;

    m_Position->SetPosition(0.0f, 0.0f, 0.0f);
    m_Position->SetScale(1.0f);

    return true;
} // Init


void DefaultModel::Shutdown()
{

} // Shutdown


void DefaultModel::Render(ID3D11DeviceContext* context)
{
    m_ModelBuffer->Render(context);
    context->DrawIndexed(m_ModelBuffer->GetIndexCount(), 0, 0);
} // Render


DirectX::XMMATRIX DefaultModel::GetModelMatrix()
{
    return m_Position->GetWorldMatrix();
} // GetModelMatrix


DirectX::XMFLOAT2 DefaultModel::GetModelUV(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj)
{
    using namespace ConstantHelper;

    XMVECTOR worldPos = XMLoadFloat3(&m_Position->GetPosition());

    XMVECTOR localSunPos = XMVector3TransformCoord(worldPos, view);
    if (XMVectorGetZ(localSunPos) < 0.0f)
    {
        return XMFLOAT2(-1.0f, -1.0f);
    }

    XMVECTOR screenPos = XMVector3Project(worldPos, 0, 0,
        SCREEN_WIDTH, SCREEN_HEIGHT, 0, 1,
        proj, view, XMMatrixIdentity());

    return XMFLOAT2(
        XMVectorGetX(screenPos) / (float)SCREEN_WIDTH,
        XMVectorGetY(screenPos) / (float)SCREEN_HEIGHT
    );
} // GetNDCModelUV


void DefaultModel::SetPosition(DirectX::XMFLOAT3 position)
{
    m_Position->SetPosition(position.x, position.y, position.z);
} // SetPosition


void DefaultModel::SetScale(float s)
{
    m_Position->SetScale(s);
} // SetScale


void DefaultModel::SetScale(float x, float y, float z)
{
    m_Position->SetScale(x, y, z);
} // SetScale


void DefaultModel::SetScale(DirectX::XMFLOAT3 scale)
{
    m_Position->SetScale(scale.x, scale.y, scale.z);
} // SetScale


void DefaultModel::SetRotation(DirectX::XMFLOAT3 position)
{
    m_Position->SetRotation(position.x, position.y, position.z);
} // SetTransform