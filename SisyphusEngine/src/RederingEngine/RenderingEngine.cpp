#include "Pch.h"
#include "RenderingEngine.h"
#include "Renderer/Renderer.h"
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
    m_TexturesManager = std::make_unique<TexturesManager>();
    m_ShaderManager = std::make_unique<ShaderManager>();
    m_Cloud = std::make_unique<DefaultModel>();
    m_Quad = std::make_unique<DefaultModel>();
    m_Sky = std::make_unique<DefaultModel>();
    m_Sun = std::make_unique<Light>();
    m_Ocean = std::make_unique<DefaultModel>();
} // RenderingEngine


RenderingEngine::~RenderingEngine()
{

} // ~RenderingEngine


bool RenderingEngine::Init(HWND hwnd)
{
    if (m_Renderer->Init(hwnd, true) == false)
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

    if (m_Ocean->Init(
        m_Renderer->GetDevice(), DefaultModelType::Ocean) == false)
		return false;

    m_Sun->Init(ConstantHelper::LightPosition, ConstantHelper::LightColor, ConstantHelper::LightIntensity);

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

    if (m_Renderer)
    {
        m_Renderer->Shutdown();
        m_Renderer.reset();
    }
    return;
} // Shutdown


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

    m_Renderer->SetRenderTarget(m_Renderer->GetRefractionRT(), 0, 0, 0, 1);
    m_Renderer->SetDepthBuffer(true);

    RefractionBuffer refrData;
    refrData.clipPlane = XMFLOAT4(0.0f, -1.0f, 0.0f, -10.0f + 0.1f); // 수면 위를 Clip
    m_ShaderManager->UpdateRefractionBuffer(context, refrData);

    m_Renderer->SetRenderTarget(m_Renderer->GetReflectionRT(), 0, 0, 0, 1);
    XMMATRIX reflectView = CalculateReflectionMatrix(camPos, -10.0f) * view;
    DrawSky(context, totalTime, camPos, reflectView, proj);

    m_Renderer->SetLowResolutionRenderTarget();
    m_Renderer->SetAlphaBlending(true);
    m_Renderer->SetDepthBuffer(false);

    // 스카이
    DrawSky(context, totalTime, camPos, view, proj);

	// 오션
    // Back-face Culling
    m_Renderer->SetDepthBuffer(true);
    DrawOcean(context, totalTime, camPos, view, proj);

    m_Renderer->SetMode(false, true);
    m_Renderer->SetAlphaBlending(true);
    // 구름
    m_Renderer->SetWrapSampler(0);
    DrawCloud(context, totalTime, camPos, view, proj);

	// 포스트 프로세싱
    m_Renderer->SetBackBufferRenderTarget();
    m_Renderer->SetAlphaBlending(false);

    ApplyLensFlare(context, view, proj, camPos);
    ApplyBicubicUpscale(context);

    m_Renderer->ClearShaderResources(0);
    m_Renderer->SetDepthBuffer(true);
  
    m_frameCount++;
} // Draw


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


ID3D11Device* RenderingEngine::GetDevice()
{
    return m_Renderer->GetDevice();
} // GetDevice


ID3D11DeviceContext* RenderingEngine::GetDeviceContext()
{
    return m_Renderer->GetDeviceContext();
} // GetDeviceContext


void RenderingEngine::DrawSky(ID3D11DeviceContext* context,
    float totalTime, XMFLOAT3 camPos, XMMATRIX view, XMMATRIX proj)
{
    using namespace ConstantHelper;

    XMMATRIX skyModel = XMMatrixTranslation(camPos.x, camPos.y, camPos.z);

    m_ShaderManager->UpdateGlobalBuffer(ShaderKeys::Sky,
        context, totalTime, (float)m_frameCount, camPos);
    m_ShaderManager->UpdateMatrixBuffer(ShaderKeys::Sky, context, skyModel, view, proj);
    m_ShaderManager->UpdateLightBuffer(ShaderKeys::Sky, context, m_Sun.get());


    SkyBuffer skyData;
    m_ShaderManager->UpdateSkyBuffer(context, skyData);
    m_ShaderManager->SetShaders(ShaderKeys::Sky, context);
    m_ShaderManager->SetConstantBuffers(ShaderKeys::Sky, context);

    m_Sky->Render(context);
} // DrawSky


void RenderingEngine::DrawOcean(ID3D11DeviceContext* context,
    float totalTime, XMFLOAT3 camPos, XMMATRIX view, XMMATRIX proj)
{
    using namespace ConstantHelper;
    float waterHeight = -10.0f;

    m_Renderer->SetReflectionShaderResource(0); // t0: Reflection
    m_Renderer->SetRefractionShaderResource(1); // t1: Refraction
    m_TexturesManager->PSSetShaderResources(context, ConstantHelper::WATER_PATH, 2); // t2: Normal

    m_Renderer->SetWrapSampler(0);

    m_ShaderManager->UpdateGlobalBuffer(ShaderKeys::Water, context, totalTime, (float)m_frameCount, camPos);

    WaterBuffer waterData;
    waterData.waterTranslation = totalTime * 0.02f;
    waterData.reflectRefractScale = 0.01f;
    m_ShaderManager->UpdateWaterBuffer(context, waterData);

    // 반사 행렬 업데이트
    XMMATRIX reflectMatrix = CalculateReflectionMatrix(camPos, waterHeight);
    m_ShaderManager->UpdateWaterReflectionMatrix(context, reflectMatrix);

    m_Ocean->SetPosition(XMFLOAT3(camPos.x, waterHeight, camPos.z));
    m_Ocean->SetScale(500.0f, 1.0f, 500.0f);

    m_ShaderManager->UpdateMatrixBuffer(ShaderKeys::Water, context, m_Ocean->GetModelMatrix(), view, proj);
    m_ShaderManager->SetShaders(ShaderKeys::Water, context);
    m_ShaderManager->SetConstantBuffers(ShaderKeys::Water, context);

    m_Renderer->SetMode(false, false);
    m_Ocean->Render(context);
    m_Renderer->SetMode(false, true);

    // 리소스 정리
    m_Renderer->ClearShaderResources(0);
    m_Renderer->ClearShaderResources(1);
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

    CloudBuffer cloudData((float)ConstantHelper::cloudType);

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
        XMMATRIX cloudModel = XMMatrixTranslation(camPos.x, camPos.y, camPos.z);
        m_ShaderManager->UpdateMatrixBuffer(ShaderKeys::Cloud, context, m_Cloud->GetModelMatrix(), view, proj);
    }
    m_ShaderManager->SetShaders(ShaderKeys::Cloud, context);
    m_ShaderManager->SetConstantBuffers(ShaderKeys::Cloud, context);

    m_Cloud->Render(context);

    m_Renderer->ClearShaderResources(0);
    m_Renderer->ClearShaderResources(1);
} // DrawCloud


void RenderingEngine::ApplyBicubicUpscale(ID3D11DeviceContext* context)
{
    m_ShaderManager->SetShaders(ShaderKeys::Bicubic, context);
    m_Renderer->SetLowResolutionShaderResources(0);

    m_Renderer->SetWrapSampler(0);

    m_ShaderManager->UpdateMatrixBuffer(
        ShaderKeys::Bicubic, context,
        XMMatrixIdentity(), XMMatrixIdentity(), XMMatrixIdentity());

    m_Quad->Render(context);

    m_Renderer->ClearShaderResources(0);
} // ApplyBicubicUpscale


void RenderingEngine::ApplyLensFlare(ID3D11DeviceContext* context,
    const XMMATRIX& view, const XMMATRIX& proj, const XMFLOAT3& camPos)
{
    m_Renderer->SetAdditiveAlphaBlending();
    m_Renderer->SetDepthBuffer(false);

    m_ShaderManager->SetShaders(ShaderKeys::LensFlare, context);
    m_Renderer->SetBorderSampler(0);

    // 버퍼
    LenFlareBuffer lensFlareBuffer;
    lensFlareBuffer.sunUV = CalculateSunUV(view, proj);
    lensFlareBuffer.lensMatrix = CalculateLensMatrix(view);

    m_ShaderManager->UpdateLensFlareBuffer(context, lensFlareBuffer);
    m_ShaderManager->SetConstantBuffers(ShaderKeys::LensFlare, context);

    // 리소스
    m_Renderer->SetLowResolutionShaderResources(0);
    m_TexturesManager->PSSetShaderResources(context, ConstantHelper::NOISE_PATH, 1);
    m_Renderer->SetMainDepthShaderResource(2);

    m_Quad->Render(context);

    m_Renderer->ClearShaderResources(0);
    m_Renderer->ClearShaderResources(1);
    m_Renderer->ClearShaderResources(2);

    m_Renderer->SetAlphaBlending(true);
} // ApplyLensFlare


XMFLOAT2 RenderingEngine::CalculateSunUV(const XMMATRIX& view, const XMMATRIX& proj)
{
    using namespace ConstantHelper;

    XMVECTOR sunWorldPos = XMLoadFloat3(&m_Sun->GetPosition());

    XMVECTOR localSunPos = XMVector3TransformCoord(sunWorldPos, view);
    if (XMVectorGetZ(localSunPos) < 0.0f)
    {
        return XMFLOAT2(-1.0f, -1.0f);
    }

    XMVECTOR sunScreenPos = XMVector3Project(sunWorldPos, 0, 0,
        SCREEN_WIDTH, SCREEN_HEIGHT, 0, 1,
        proj, view, XMMatrixIdentity());

    return XMFLOAT2(
        XMVectorGetX(sunScreenPos) / (float)SCREEN_WIDTH,
        XMVectorGetY(sunScreenPos) / (float)SCREEN_HEIGHT
    );
} // CalculateSunUV


XMMATRIX RenderingEngine::CalculateLensMatrix(const XMMATRIX& view)
{
    float m00 = XMVectorGetX(view.r[0]);
    float m01 = XMVectorGetY(view.r[0]);
    float camRot = atan2(m01, m00);

    return XMMatrixTranspose(MathHelper::TransformUVRotationMatrix(camRot));
} // CalculateLensMatrix


XMMATRIX RenderingEngine::CalculateReflectionMatrix(XMFLOAT3 camPos, float waterHeight)
{
    float reflectionY = -camPos.y + (waterHeight * 2.0f);
    XMVECTOR reflectionPos = XMVectorSet(camPos.x, reflectionY, camPos.z, 1.0f);
    XMMATRIX reflectionMatrix = XMMatrixReflect(XMVectorSet(0, 1, 0, -waterHeight));

    return reflectionMatrix;
} // CalculateReflectionMatrix