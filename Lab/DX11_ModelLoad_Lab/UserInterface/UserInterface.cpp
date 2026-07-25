// UserInterface/UserInterface.cpp
#include "UserInterface.h"
#include "Widget/MainSideBarWidget.h"
#include "Widget/ControlWidget.h"
#include "Widget/InspectorWidget.h"
#include "Widget/StatsWidget.h"
// System
#include "Input/Input.h"
#include "Gui/Gui.h"
// Graphics
#include "Renderer/Renderer.h"
// Application
#include "Timer/Timer.h"
#include "Fps/Fps.h"
// World
#include "World/World.h"
// imgui
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

/* default */
//////////////////////////////////////////////////////////

UserInterface::UserInterface()
    : m_Gui(nullptr),
    m_MainSideBar(nullptr) {}


UserInterface::~UserInterface() 
{
    m_widgets.clear();
} // ~UserInterface


bool UserInterface::Init(std::shared_ptr<Gui> gui)
{
    if (gui == nullptr) return false;

    m_Gui = gui; // 참조 cnt + 1
    return true;
} // Init


void UserInterface::Frame()
{
    //for (auto& widget : m_widgets)
    //{
    //    widget->Frame(); // 필요 시
    //}

    return;
} // Frame


void UserInterface::Render()
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

//////////////////////////////////////////////////////////////////

/* public */
/////////////////////////////////////////////////////////////////


void UserInterface::Begin()
{
    if (m_Gui) m_Gui->Begin();
} // Begin


void UserInterface::End()
{
    if (m_Gui) m_Gui->End();
} // End

bool UserInterface::CanControlWorld() const
{
    ImGuiIO& io = ImGui::GetIO();
    return !(io.WantCaptureMouse || io.WantCaptureKeyboard);
} // CanControlWorld


void UserInterface::AddWidget(std::unique_ptr<IWidget> widget)
{
    if (widget->GetName().find("MainSideBar") != std::string::npos)
        m_MainSideBar = static_cast<MainSideBarWidget*>(widget.get());
    m_widgets.push_back(std::move(widget));
    return;
} // AddWidget


void UserInterface::ToggleMainSideBar()
{
    if (m_MainSideBar) m_MainSideBar->SetVisible(!m_MainSideBar->IsVisible());
} // ToggleMainSideBar


void UserInterface::CreateSideBar(std::string name, Timer* timer, Fps* fps, Renderer* renderer, World* world)
{
    auto sideBar = std::make_unique<MainSideBarWidget>(name);

    m_MainSideBar = sideBar.get();

    sideBar->AddComponent(std::make_unique<StatsWidget>(fps, timer));
    sideBar->AddComponent(std::make_unique<ControlWidget>(renderer, world));

    auto inspector = std::make_unique<InspectorWidget>();
    if (world) {
        inspector->SetActorList(world->GetActors());
        if (!world->GetActors().empty()) {
            inspector->SetTarget(world->GetActors()[0].get());
        }
    }
    sideBar->AddComponent(std::move(inspector));
    m_widgets.push_back(std::move(sideBar));

    return;
} // CreateSideBar


void UserInterface::ApplyRenderStates(Renderer* renderer)
{
    if (m_MainSideBar && m_MainSideBar->IsVisible())
    {
        auto control = m_MainSideBar->GetComponent<ControlWidget>();
        if (control)
        {
            renderer->GetRasterizer()->Bind(
                renderer->GetDeviceContext(),
                control->IsWireframe(),
                control->IsCullNone()
            );
        }
    }

    return;
} // ApplyRenderStates

bool UserInterface::IsWorldClicked(bool mousePressed) const
{
    return mousePressed && !ImGui::GetIO().WantCaptureMouse;
} // IsWorldClicked