#include "Pch.h"
#include "RenderTexture.h"
#include "RenderTarget.h"
// Common
#include "DebugHelper.h"


using namespace DebugHelper;


RenderTexture::RenderTexture()
{
    m_ReflectionRT = std::make_unique<RenderTarget>();
    m_RefractionRT = std::make_unique<RenderTarget>();
    m_LowResRT = std::make_unique<RenderTarget>();
    m_CloudRT = std::make_unique<RenderTarget>();
} // RenderTexture


RenderTexture::~RenderTexture()
{
    Shutdown();
} // RenderTexture


bool RenderTexture::Init(ID3D11Device* device, int width, int height)
{
    // 반사용 타겟 초기화
    if (m_ReflectionRT->Init(device, width, height) == false)
        return false;

    // 굴절용 타겟 초기화
    if (m_RefractionRT->Init(device, width, height) == false)
        return false;

    // 저해상도용 타겟 초기화
    if (m_LowResRT->Init(device, width, height) == false)
        return false;

    // 볼류매트릭 구름
    if (m_CloudRT->Init(device, width, height) == false)
        return false;

    return true;
} // Init


void RenderTexture::Shutdown()
{
    if (m_ReflectionRT) m_ReflectionRT.reset();
    if (m_RefractionRT) m_RefractionRT.reset();
    if (m_LowResRT) m_LowResRT.reset();
    if (m_CloudRT) m_CloudRT.reset();
} // Shutdown


void RenderTexture::SetLowResolutionRenderTarget(ID3D11DeviceContext* context)
{
    ClearShaderResources(context, 0);

    m_LowResRT->Clear(context, 0, 0, 0, 1);
} // SetLowResolutionRenderTarget


void RenderTexture::SetReflectionRenderTarget(ID3D11DeviceContext* context)
{
    ClearShaderResources(context, 0);
    m_ReflectionRT->Clear(context, 0, 0, 0, 1);
} // SetReflectionRenderTarget


void RenderTexture::SetRefractionRenderTarget(ID3D11DeviceContext* context)
{
    ClearShaderResources(context, 0);
    m_RefractionRT->Clear(context, 0, 0, 0, 1);
} // SetRefractionRenderTarget


void RenderTexture::SetCloudRenderTarget(ID3D11DeviceContext* context)
{
    ClearShaderResources(context, 0);
    m_CloudRT->Clear(context, 0, 0, 0, 1);
} // SetCloudRenderTarget


void RenderTexture::SetLowResolutionSRV(ID3D11DeviceContext* context, UINT slot)
{
    ID3D11ShaderResourceView* srv = m_LowResRT->GetSRV();
    context->PSSetShaderResources(slot, 1, &srv);
} // SetLowResolutionSRV


void RenderTexture::SetReflectionSRV(ID3D11DeviceContext* context, UINT slot)
{
    ID3D11ShaderResourceView* srv = m_ReflectionRT->GetSRV();
    context->PSSetShaderResources(slot, 1, &srv);
} // SetReflectionSRV


void RenderTexture::SetRefractionSRV(ID3D11DeviceContext* context, UINT slot)
{
    ID3D11ShaderResourceView* srv = m_RefractionRT->GetSRV();

    context->PSSetShaderResources(slot, 1, &srv);
} // SetRefractionSRV


void RenderTexture::SetCloudSRV(ID3D11DeviceContext* context, UINT slot)
{
    ID3D11ShaderResourceView* srv = m_CloudRT->GetSRV();

    context->PSSetShaderResources(slot, 1, &srv);
} // SetCloudSRV


RenderTarget* RenderTexture::GetReflectionRT() const
{
    return m_ReflectionRT.get();
} // GetReflectionRT


RenderTarget* RenderTexture::GetRefractionRT() const
{
    return m_RefractionRT.get();
} // GetRefractionRT


RenderTarget* RenderTexture::GetLowResRT() const
{
    return m_LowResRT.get();
}// GetLowResRT


RenderTarget* RenderTexture::GetCloudRT() const
{
    return m_CloudRT.get();
} // GetCloudRT


void RenderTexture::ClearShaderResources(ID3D11DeviceContext* context, UINT slot)
{
    ID3D11ShaderResourceView* nullSRV = nullptr;
    context->PSSetShaderResources(slot, 1, &nullSRV);
} // ClearShaderResources