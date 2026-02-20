#include "Pch.h"
#include "Light.h"
// Framework
#include "Position.h"
// Model
#include "DefaultModelBuffer.h"
// Common
#include "ConstantHelper.h"


Light::Light()
    : m_color(1.0f, 1.0f, 1.0f, 1.0f),
    m_intensity(1.0f)
{
    m_ModelBuffer = std::make_unique<DefaultModelBuffer>();
    m_Position = std::make_unique<Position>();
}

Light::~Light() {}


void Light::Init(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT4 color, float intensity)
{
    SetPosition(pos);
    SetColor(color.x, color.y, color.z, color.w);
    SetIntensity(intensity);

    return;
} // Init


void Light::Render(ID3D11DeviceContext* context)
{
    m_ModelBuffer->Render(context);
    context->DrawIndexed(m_ModelBuffer->GetIndexCount(), 0, 0);
} // Render


void Light::SetPosition(DirectX::XMFLOAT3 position)
{
    m_Position->SetPosition(position.x, position.y, position.z);
} // SetPosition


void Light::SetScale(float s)
{
    m_Position->SetScale(s);
} // SetScale


void Light::SetRotation(DirectX::XMFLOAT3 rot)
{
    m_Position->SetRotation(rot.x, rot.y, rot.z);
} // SetRotation


void Light::SetColor(DirectX::XMFLOAT4 color)
{
    m_color = color;
} // SetColor


void Light::SetColor(float r, float g, float b, float a)
{
    m_color = DirectX::XMFLOAT4(r, g, b, a);
} // SetColor


void Light::SetIntensity(float intensity)
{
    m_intensity = intensity;
} // SetColor


DirectX::XMFLOAT3 Light::GetPosition()
{
    return m_Position->GetPosition();
} // GetPosition


Position* Light::GetPositionPtr()
{
    return m_Position.get();
} // GetPositionPtr


DirectX::XMFLOAT4 Light::GetColor() const
{
    return m_color;
} // GetColor


float Light::GetIntensity() const
{
    return m_intensity;
} // GetIntensity


DirectX::XMMATRIX Light::GetModelMatrix()
{
    return m_Position->GetWorldMatrix();
} // GetModelMatrix


DirectX::XMFLOAT2 Light::GetModelUV(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj)
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
