#include "Pch.h"
#include "MainEngine.h"
#include "Timer.h"
#include "Fps.h"
#include "Camera.h"
// System
#include "Input.h"
#include "InputManager.h"
// UI
#include "UI.h"
#include "ShaderBufferWidget.h"
// Common
#include "ConstantHelper.h"
#include "DebugHelper.h"
#include "PropertyHelper.h"
// Rendering
#include "RenderingEngine.h"
#include "Shader/ShaderBuffersManager.h"

using namespace PropertyHelper;
using namespace DirectX;


MainEngine::MainEngine()
{
    m_Timer = std::make_unique<Timer>();
    m_Fps = std::make_unique<Fps>();
    m_Camera = std::make_unique<Camera>();
    m_RenderingEngine = std::make_unique<RenderingEngine>();
    m_UI = std::make_unique<UI>();
} // MainEngine


MainEngine::~MainEngine()
{

} // ~MainEngine


bool MainEngine::Init(HWND hwnd, std::shared_ptr<InputManager> inputManager)
{
    m_InputManager = inputManager;

    if (m_Timer->Init() == false) return false;
    m_Fps->Init();

    ConstantHelper::cloudType = ConstantHelper::CloudType::Plane;
    auto setup = ConstantHelper::GetCloudCameraSetup();
     m_Camera->SetPosition(setup.position);
     m_Camera->SetRotation(setup.rotation);

    if (m_RenderingEngine->Init(hwnd) == false) return false;

    if (m_UI->Init(hwnd,
        m_RenderingEngine->GetDevice(),
        m_RenderingEngine->GetDeviceContext())
        == false) return false;

    CreateWidget();
    return true;
} // Init


void MainEngine::Shutdown()
{
    if (m_UI)
    {
        m_UI->Shutdown();
        m_UI.reset();
    }

    if (m_RenderingEngine)
    {
        m_RenderingEngine->Shutdown();
        m_RenderingEngine.reset();
    }
} // Shutdown


bool MainEngine::Frame()
{
    m_Timer->Frame();
    m_Fps->Frame();

    if (UpdateUserInput() == false)
        return false;

    UpdateRenderStatesWidget();

    Render();

    return true;
} // Frame


void MainEngine::Render()
{
    m_RenderingEngine->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

    m_RenderingEngine->Draw(m_Timer->GetTotalTime(),
        Property<XMMATRIX>([this]() { return m_Camera->GetViewMatrix(); }, nullptr),
        Property<XMMATRIX>([this]() { return m_Camera->GetProjectionMatrix(); }, nullptr),
        Property<XMFLOAT3>([this]() { return m_Camera->GetPosition(); }, nullptr),
        Property<float>([this]() { return m_Camera->GetFov(); }, nullptr));

    m_UI->Render();

    m_RenderingEngine->EndScene();
} // Render


bool MainEngine::UpdateUserInput()
{
    if (m_InputManager->IsF1Toggled())
    {
        m_UI->ToggleWidget();
    }
    if (m_InputManager->IsMouseLPressed() && m_UI->CanControlWorld())
    {
        m_UI->SetCameraLocked(false);
    }
    else if (m_InputManager->IsMouseLPressed() && !m_UI->CanControlWorld())
    {
        m_UI->SetCameraLocked(true);
    }

    auto fovAddProp = PropertyHelper::Property<float>(
        nullptr,
        [this](const float& delta) {
            m_Camera->AddFOV(delta);
        }
    );

    return m_InputManager->Frame(m_Timer->GetFrameTime(),
        m_Camera->GetPositionPtr(),
        fovAddProp, m_UI->IsCameraLocked());
} // UpdateUserInput


void MainEngine::UpdateRenderStatesWidget()
{
    m_RenderingEngine->SetMode(
        m_RenderingEngine->GetWireframeEnable(), m_RenderingEngine->GetBackCullEnable());
    m_RenderingEngine->SetDepthBuffer(m_RenderingEngine->GetDepthEnable());
} // UpdateRenderStates


void MainEngine::CreateWidget()
{
    Property<float> timeProp([this]() { return m_Timer->GetTotalTime(); }, nullptr);
    Property<int> fpsProp([this]() { return m_Fps->GetFps(); }, nullptr);

    Property<XMFLOAT3> camPosProp(
        [this]() { return m_Camera->GetPosition(); },
        [this](const XMFLOAT3& p) { m_Camera->SetPosition(p); }
    );
    Property<XMFLOAT3> camRotProp(
        [this]() { return m_Camera->GetRotation(); },
        [this](const XMFLOAT3& r) { m_Camera->SetRotation(r); }
    );

    auto camFovProp = Property<float>(
        [this]() { return m_Camera->GetFov(); },
        [this](const float& f) { m_Camera->SetFOV(f); }
    );

    auto wireProp = Property<bool>(
        [this]() { return m_RenderingEngine->GetWireframeEnable(); },
        [this](const bool& v) {m_RenderingEngine->SetWireframeEnable(v); }
    );
    auto backProp = Property<bool>(
        [this]() { return m_RenderingEngine->GetBackCullEnable(); },
        [this](const bool& v) { m_RenderingEngine->SetBackCullEnable(v); }
    );
    auto depthProp = Property<bool>(
        [this]() { return m_RenderingEngine->GetDepthEnable(); },
        [this](const bool& v) { m_RenderingEngine->SetDepthEnable(v); }
    );

    m_UI->CreateWidget(timeProp, fpsProp,
        camPosProp, camRotProp, camFovProp,
        wireProp, backProp, depthProp);

    //m_UI->CreateWidget(m_RenderingEngine->DeliveryBuffer<WaterBuffer>(ShaderBufferKeys::Water));
    m_UI->CreateWidget(m_RenderingEngine->DeliveryBuffer<VolumetricCloudsBuffer>(ShaderBufferKeys::VolumetricClouds));
    //m_UI->CreateWidget(m_RenderingEngine->DeliveryBuffer<CloudBuffer>(ShaderBufferKeys::Cloud));
    //m_UI->CreateWidget(m_RenderingEngine->DeliveryBuffer<SkyBuffer>(ShaderBufferKeys::Sky));
    //m_UI->CreateWidget(m_RenderingEngine->DeliveryBuffer<LensFlareBuffer>(ShaderBufferKeys::LensFlare));

} // CreateWidget