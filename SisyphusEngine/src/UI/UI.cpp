#include "Pch.h"
#include "UI.h"
#include "StatsWidget.h"
#include "CameraWidget.h"
#include "RenderStateWidget.h"
#include "ShaderBufferWidget.h"
#include "ShaderBufferImGuiDrawer.h"
// Framework
#include "IWidget.h"
// imgui
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"


using namespace PropertyHelper;


UI::UI()
    : m_isCameraLocked(false)
{
    m_Gui = std::make_unique<IImGUI>();
} // UI


UI::~UI() 
{
} // ~UI


bool UI::Init(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    return m_Gui->Init(hwnd, device, deviceContext);
} // Init


void UI::Shutdown()
{
    m_widgets.clear();
    if (m_Gui) m_Gui->Shutdown();
} // Shutdown


void UI::Render()
{
    if (m_Gui == nullptr) return;

    m_Gui->Begin();

    for (auto& widget : m_widgets)
    {
        if (widget->IsVisible())
        {
            widget->Frame();
        }
    }

    m_Gui->End();
    return;
} // Render


void UI::Begin()
{
    if (m_Gui) m_Gui->Begin();
} // Begin


void UI::End()
{
    if (m_Gui) m_Gui->End();
} // End

bool UI::CanControlWorld() const
{
    if (m_Gui == nullptr) { return false; }
    ImGuiIO& io = ImGui::GetIO();
    return !(io.WantCaptureMouse || io.WantCaptureKeyboard);
} // CanControlWorld


void UI::AddWidget(std::unique_ptr<IWidget> widget)
{
    m_widgets.push_back(std::move(widget));
    return;
} // 


void UI::CreateWidget(
    Property<float> timeProp, Property<int> fpsProp,
    PropertyHelper::Property<DirectX::XMFLOAT3> posProp,
    PropertyHelper::Property<DirectX::XMFLOAT3> rotProp,
    PropertyHelper::Property<float> fovProp,
    PropertyHelper::Property<bool> wire,
    PropertyHelper::Property<bool> back,
    PropertyHelper::Property<bool> depth)
{
    AddWidget(std::make_unique<StatsWidget>("Engine Stats", timeProp, fpsProp));
    AddWidget(std::make_unique<CameraWidget>("Camera Controller", posProp, rotProp, fovProp));

    AddWidget(std::make_unique<RenderStateWidget>("Render Settings", wire, back, depth));
} // CreateWidget


void UI::CreateWidget(PropertyHelper::Property<WaterBuffer> prop)
{
    AddWidget(std::make_unique<ShaderBufferWidget<WaterBuffer>>(
        "Water Settings",
        prop,
        ShaderBufferImGuiDrawer::DrawWater
    ));
} // CreateWidget (Water)


void UI::CreateWidget(PropertyHelper::Property<CloudBuffer> prop)
{
    AddWidget(std::make_unique<ShaderBufferWidget<CloudBuffer>>(
        "Cloud Settings",
        prop,
        ShaderBufferImGuiDrawer::DrawCloud
    ));
} // CreateWidget (Cloud)


void UI::CreateWidget(PropertyHelper::Property<SkyBuffer> prop)
{
    AddWidget(std::make_unique<ShaderBufferWidget<SkyBuffer>>(
        "Sky Settings",
        prop,
        ShaderBufferImGuiDrawer::DrawSky
    ));
} // CreateWidget (Sky)


void UI::CreateWidget(PropertyHelper::Property<LensFlareBuffer> prop)
{
    AddWidget(std::make_unique<ShaderBufferWidget<LensFlareBuffer>>(
        "Lens Flare Settings",
        prop,
        ShaderBufferImGuiDrawer::DrawLensFlare
    ));
} // CreateWidget (LensFlare)


void UI::CreateWidget(PropertyHelper::Property<VolumetricCloudsBuffer> prop)
{
    AddWidget(std::make_unique<ShaderBufferWidget<VolumetricCloudsBuffer>>(
        "PBR Volumetric Cloud Settings",
        prop,
        ShaderBufferImGuiDrawer::DrawVolumetricClouds
    ));
} // CreateWidget (PBR Volumetric Cloud)


void UI::ToggleWidget()
{

    for (auto& widget : m_widgets)
    {
        widget->SetVisible(!widget->IsVisible());
    }
} // ToggleWidget


bool UI::IsWorldClicked(bool mousePressed) const
{
    if (m_Gui == nullptr) return false;
    return mousePressed && !ImGui::GetIO().WantCaptureMouse;
} // IsWorldClicked