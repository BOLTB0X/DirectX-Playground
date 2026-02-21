#include "Pch.h"
#include "RenderingEngine.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderTexture.h"
#include "Model/DefaultModel.h"
#include "Model/Light.h"
#include "Model/TexturesManager.h"
#include "Model/Texture.h"
// Shader
#include "Shader/ShaderManager.h"
// Framework & Base
//#include "Frustum.h"
// Common
#include "ConstantHelper.h"
#include "MathHelper.h"
#include "DebugHelper.h"


using namespace PropertyHelper;
using namespace DirectX;


RenderingEngine::RenderingEngine()
    : m_isWireframe(false),
    m_backCullEnable(false),
    m_depthEnable(true),
    m_frameCount(0)
{
    m_Renderer = std::make_unique<Renderer>();
    m_RenderTexture = std::make_unique<RenderTexture>();
    m_TexturesManager = std::make_unique<TexturesManager>();
    m_ShaderManager = std::make_unique<ShaderManager>();
    m_Cloud = std::make_unique<DefaultModel>();
    m_Quad = std::make_unique<DefaultModel>();
    m_Sky = std::make_unique<DefaultModel>();
    m_Sun = std::make_unique<Light>();
    m_Ocean = std::make_unique<DefaultModel>();
    m_BuffersManager = std::make_unique<ShaderBuffersManager>();
} // RenderingEngine


RenderingEngine::~RenderingEngine()
{
    Shutdown();
} // ~RenderingEngine


bool RenderingEngine::Init(HWND hwnd)
{
    if (m_Renderer->Init(hwnd, true) == false)
        return false;

    if (m_RenderTexture->Init(m_Renderer->GetDevice(),
        ConstantHelper::SCREEN_WIDTH,
        ConstantHelper::SCREEN_HEIGHT) == false)
        return false;

    if (m_TexturesManager->Init(
        m_Renderer->GetDevice(),
        m_Renderer->GetDeviceContext())
        == false) return false;

    if (m_ShaderManager->Init(
        m_Renderer->GetDevice(),
        hwnd) == false) return false;

    if (m_Sky->Init(
        m_Renderer->GetDevice(), DefaultModelType::Sphere) == false)
        return false;

    if (m_Quad->Init(
        m_Renderer->GetDevice(), DefaultModelType::Quad) == false)
        return false;

    if (m_Cloud->Init(
        m_Renderer->GetDevice(), DefaultModelType::Sphere) == false)
        return false;

    m_Sun->Init(ConstantHelper::LightPosition, ConstantHelper::LightColor, ConstantHelper::LightIntensity);

    if (m_Ocean->Init(
        m_Renderer->GetDevice(), DefaultModelType::Ocean) == false)
		return false;

    //m_BuffersManager->Init();
    return true;
} // Init


void RenderingEngine::Shutdown()
{
    if (m_Ocean)
        m_Ocean->Shutdown();

    if (m_ShaderManager)
        m_ShaderManager.reset();

    if (m_TexturesManager)
        m_TexturesManager.reset();

    if (m_RenderTexture)
    {
        m_RenderTexture->Shutdown();
        m_RenderTexture.reset();
    }

    if (m_Renderer)
    {
        m_Renderer->Shutdown();
        m_Renderer.reset();
    }
    return;
} // Shutdown


void RenderingEngine::BeginScene(float r, float g, float b, float a)
{
    m_Renderer->BeginScene(r, g, b, a);
} // BeginScene


void RenderingEngine::EndScene()
{
    m_Renderer->EndScene();
} // EndScene


void RenderingEngine::SetMode(bool isWireframe, bool backCullEnable)
{
    m_Renderer->SetMode(isWireframe, backCullEnable);
} // SetMode


void RenderingEngine::SetDepthBuffer(bool depthEnable)
{
    m_Renderer->SetDepthBuffer(depthEnable);
} // SetDepthBuffer


void RenderingEngine::SetWireframeEnable(bool val)
{
    m_isWireframe = val;
} // SetWireframeEnable


void RenderingEngine::SetBackCullEnable(bool val)
{
    m_backCullEnable = val;
} // SetBackCullEnable


void RenderingEngine::SetDepthEnable(bool val)
{
    m_depthEnable = val;
} // SetDepthEnable


ID3D11Device* RenderingEngine::GetDevice()
{
    return m_Renderer->GetDevice();
} // GetDevice


ID3D11DeviceContext* RenderingEngine::GetDeviceContext()
{
    return m_Renderer->GetDeviceContext();
} // GetDeviceContext


bool RenderingEngine::GetWireframeEnable() const
{
    return m_isWireframe;
} // GetWireframeEnable


bool RenderingEngine::GetBackCullEnable() const
{
    return m_backCullEnable;
} // GetBackCullEnable


bool RenderingEngine::GetDepthEnable() const
{
    return m_depthEnable;
} // GetDepthEnable


void RenderingEngine::Draw(
    float totalTime,
    Property<XMMATRIX> viewProp,
    Property<XMMATRIX> projProp,
    Property<XMFLOAT3> camPosProp)
{
    ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();
    XMFLOAT3 camPos = camPosProp.Get();
    XMMATRIX view = viewProp.Get();
    XMMATRIX proj = projProp.Get();

    // [사전패스]
    ApplyReflection(totalTime, camPos, view, proj);
    ApplyRefraction(totalTime, camPos, view, proj);

    m_Renderer->SetAlphaBlending(true);
    m_Renderer->SetDepthBuffer(false);

    m_Renderer->SetBackBufferRenderTarget();
    m_Renderer->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    // [메인패스] 메인 씬을 저해상도(혹은 오프스크린) 타겟에 먼저 그림
    m_Renderer->SetRenderTarget(m_RenderTexture->GetLowResRT(), 0.0f, 0.0f, 0.0f, 1.0f);
    m_Renderer->SetAlphaBlending(true);
    m_Renderer->SetDepthBuffer(false);
    // 스카이
    DrawSky(context, totalTime, camPos, view, proj);

    if (m_isWireframe)
        m_Renderer->SetWireframeMode();
    else
        m_Renderer->SetSolidMode();

    // 오션
    // Back-face Culling
    m_Renderer->SetDepthBuffer(true);
    DrawOcean(context, totalTime, camPos, view, proj);

    // 구름
    m_Renderer->SetAlphaBlending(true);
    m_Renderer->SetWrapSampler(0);
    DrawCloud(context, totalTime, camPos, view, proj);

    // [후처리 패스]
    m_Renderer->SetBackBufferRenderTarget();
    m_Renderer->SetAlphaBlending(false);
    ApplyLensFlare(context, view, proj, camPos);
    ApplyBicubicUpscale(context);

    m_Renderer->ClearShaderResources(0);
    m_Renderer->SetDepthBuffer(true);

    m_frameCount++;
} // Draw


void RenderingEngine::DrawSky(ID3D11DeviceContext* context,
    float totalTime, XMFLOAT3 camPos, XMMATRIX view, XMMATRIX proj)
{
    using namespace ConstantHelper;

    m_Sky->SetPosition(camPos);
    m_ShaderManager->UpdateGlobalBuffer(ShaderKeys::Sky,
        context, totalTime, (float)m_frameCount, camPos);
    m_ShaderManager->UpdateMatrixBuffer(ShaderKeys::Sky, context, m_Sky->GetModelMatrix(), view, proj);
    m_ShaderManager->UpdateLightBuffer(ShaderKeys::Sky, context, m_Sun.get(), m_Sun->GetModelUV(view, proj));

    //SkyBuffer skyData;
    auto& skyData = m_BuffersManager->GetBuffer<SkyBuffer>(ShaderBufferKeys::Sky);
    m_ShaderManager->UpdateSkyBuffer(context, skyData);
    m_ShaderManager->SetShaders(ShaderKeys::Sky, context);
    m_ShaderManager->SetConstantBuffers(ShaderKeys::Sky, context);

    m_Sky->Render(context);
} // DrawSky


void RenderingEngine::DrawOcean(ID3D11DeviceContext* context,
    float totalTime, XMFLOAT3 camPos, XMMATRIX view, XMMATRIX proj)
{
    using namespace ConstantHelper;
    float waterHeight = -2.0f;

    m_RenderTexture->SetReflectionSRV(context, 0); // t0
    m_RenderTexture->SetRefractionSRV(context, 1); // t1
    m_TexturesManager->PSSetShaderResources(context, ConstantHelper::WATER_PATH, 2); // t2

    m_Renderer->SetWrapSampler(0);

    m_ShaderManager->UpdateGlobalBuffer(ShaderKeys::Water, context, totalTime, (float)m_frameCount, camPos);

    auto& waterData = m_BuffersManager->GetBuffer<WaterBuffer>(ShaderBufferKeys::Water);
    waterData.waterTranslation = totalTime * 0.1f;
    waterData.reflectRefractScale = 0.01f;
    m_ShaderManager->UpdateWaterBuffer(context, waterData);

    XMMATRIX reflectView = MathHelper::GetReflectionMatrixFromPlane(camPos, waterHeight) * view;
    m_ShaderManager->UpdateWaterReflectionMatrix(context, reflectView);

    m_Ocean->SetPosition(XMFLOAT3(camPos.x, waterHeight, camPos.z));
    m_Ocean->SetScale(500.0f, 1.0f, 500.0f);

    m_ShaderManager->UpdateMatrixBuffer(ShaderKeys::Water, context, m_Ocean->GetModelMatrix(), view, proj);
    m_ShaderManager->SetShaders(ShaderKeys::Water, context);
    m_ShaderManager->SetConstantBuffers(ShaderKeys::Water, context);

    m_Renderer->SetCullNoneMode();
    m_Ocean->Render(context);
    m_Renderer->SetSolidMode();

    // 리소스 정리
    m_RenderTexture->ClearShaderResources(context, 0);
    m_RenderTexture->ClearShaderResources(context, 1);
    m_Renderer->ClearShaderResources(2);
} // DrawOcean


void RenderingEngine::DrawCloud(ID3D11DeviceContext* context,
    float totalTime, XMFLOAT3 camPos, XMMATRIX view, XMMATRIX proj)
{
    m_TexturesManager->PSSetShaderResources(context,
        ConstantHelper::NOISE_PATH, 0);
    m_TexturesManager->PSSetShaderResources(context,
        ConstantHelper::BLUE_NOISE_PATH, 1);

    m_ShaderManager->UpdateGlobalBuffer(ShaderKeys::Cloud,
        context, totalTime, (float)m_frameCount, camPos);


    auto& cloudData = m_BuffersManager->GetBuffer<CloudBuffer>(ShaderBufferKeys::Cloud);

    m_ShaderManager->UpdateCloudBuffer(context, cloudData);

    if (ConstantHelper::cloudType == ConstantHelper::CloudType::Default)
    {
        XMMATRIX cloudModel = XMMatrixScaling(1.5f, 1.5f, 1.5f)
            * XMMatrixTranslation(camPos.x, camPos.y, camPos.z);
        m_ShaderManager->UpdateMatrixBuffer(ShaderKeys::Cloud,
            context, cloudModel, view, proj);
    }
    else
    {
        m_Cloud->SetPosition(XMFLOAT3(camPos.x, camPos.y + 1, camPos.z));
        m_ShaderManager->UpdateMatrixBuffer(ShaderKeys::Cloud, context, m_Cloud->GetModelMatrix(), view, proj);
    }
    m_ShaderManager->SetShaders(ShaderKeys::Cloud, context);
    m_ShaderManager->SetConstantBuffers(ShaderKeys::Cloud, context);

    m_Cloud->Render(context);

    m_Renderer->ClearShaderResources(0);
    m_Renderer->ClearShaderResources(1);
} // DrawCloud


void RenderingEngine::ApplyReflection(float totalTime, XMFLOAT3 camPos, XMMATRIX view, XMMATRIX proj)
{
    ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();
    float waterHeight = -2.0f;

    m_Renderer->SetRenderTarget(m_RenderTexture->GetReflectionRT(), 0.0f, 0.0f, 0.0f, 1.0f);

    // 반사 카메라 위치 및 뷰 행렬 계산
    XMFLOAT3 reflectCamPos = camPos;
    reflectCamPos.y = -camPos.y + (waterHeight * 2.0f);
    XMMATRIX reflectView = MathHelper::GetReflectionMatrixFromPlane(camPos, waterHeight) * view;

    m_Renderer->SetAlphaBlending(true);
    m_Renderer->SetDepthBuffer(false);

    m_Renderer->SetCullNoneMode();
    DrawSky(context, totalTime, reflectCamPos, reflectView, proj);
    m_Renderer->SetSolidMode();
} // ApplyReflection


void RenderingEngine::ApplyRefraction(float totalTime, XMFLOAT3 camPos, XMMATRIX view, XMMATRIX proj)
{
    ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();

    // 굴절용 렌더 타겟 설정
    m_Renderer->SetRenderTarget(m_RenderTexture->GetRefractionRT(), 0.0f, 0.0f, 0.0f, 1.0f);

    m_Renderer->SetDepthBuffer(true);
    m_Renderer->SetAlphaBlending(false);
} // ApplyRefraction


void RenderingEngine::ApplyBicubicUpscale(ID3D11DeviceContext* context)
{
    m_ShaderManager->SetShaders(ShaderKeys::Bicubic, context);
    m_RenderTexture->SetLowResolutionSRV(context, 0);

    m_Renderer->SetWrapSampler(0);

    m_ShaderManager->UpdateMatrixBuffer(
        ShaderKeys::Bicubic, context,
        XMMatrixIdentity(), XMMatrixIdentity(), XMMatrixIdentity());

    m_Quad->Render(context);
    m_RenderTexture->ClearShaderResources(context, 0);
} // ApplyBicubicUpscale


void RenderingEngine::ApplyLensFlare(ID3D11DeviceContext* context,
    const XMMATRIX& view, const XMMATRIX& proj, const XMFLOAT3& camPos)
{
    m_Renderer->SetAdditiveAlphaBlending();
    m_Renderer->SetDepthBuffer(false);

    m_ShaderManager->SetShaders(ShaderKeys::LensFlare, context);
    m_Renderer->SetBorderSampler(0);

    // 버퍼
    //LensFlareBuffer lensFlareBuffer;
    auto& lensFlareBuffer = m_BuffersManager->GetBuffer<LensFlareBuffer>(ShaderBufferKeys::LensFlare);
    lensFlareBuffer.sunUV = m_Sun->GetModelUV(view, proj);
    lensFlareBuffer.lensMatrix = MathHelper::GetUVRotationMatrix(view);

    m_ShaderManager->UpdateLensFlareBuffer(context, lensFlareBuffer);
    m_ShaderManager->SetConstantBuffers(ShaderKeys::LensFlare, context);

    // 리소스
    m_RenderTexture->SetLowResolutionSRV(context, 0);
    m_TexturesManager->PSSetShaderResources(context, ConstantHelper::NOISE_PATH, 1);
    m_Renderer->SetMainDepthShaderResource(2);

    m_Quad->Render(context);

    m_RenderTexture->ClearShaderResources(context, 0);
    m_Renderer->ClearShaderResources(1);
    m_Renderer->ClearShaderResources(2);

    m_Renderer->SetAlphaBlending(true);
} // ApplyLensFlare

