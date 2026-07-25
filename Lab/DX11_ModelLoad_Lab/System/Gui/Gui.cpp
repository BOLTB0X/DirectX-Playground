// Base/Gui.h
#include "Gui.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "Common/EngineHelper.h"

/* default */
/////////////////////////////////////////////////////////////////////

Gui::Gui() {}


Gui::~Gui()
{
    Shutdown();
} // ~Gui


bool Gui::Init(HWND hwnd,
    ID3D11Device* device,
    ID3D11DeviceContext* deviceContext)
{

    // ImGui 컨텍스트 생성 확인
    IMGUI_CHECKVERSION();
    if (ImGui::CreateContext() == false) {
        EngineHelper::ErrorBox(hwnd, L"ImGui Context 생성 실패");
        return false;
    }

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    if (EngineHelper::SuccessCheck(ImGui_ImplWin32_Init(hwnd),
        "ImGui Win32 백엔드 초기화 실패") == false) return false;

    if (EngineHelper::SuccessCheck(ImGui_ImplDX11_Init(device, deviceContext),
        "ImGui DX11 백엔드 초기화 실패") == false) return false;

    ImGui::StyleColorsDark();

    return true;
} // Init


void Gui::Shutdown()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
} // Shutdown


void Gui::Begin()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
} // Begin


void Gui::End()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
} // End

/////////////////////////////////////////////////////////////////////

/* public */
/////////////////////////////////////////////////////////////////////

//void Gui::AddWidget(std::unique_ptr<IWidget> widget)
//{
//    m_widgets.push_back(std::move(widget));
//} // AddWidget
//
//
//void Gui::RenderWidgets()
//{
//    for (auto& widget : m_widgets)
//    {
//        widget->Frame();
//    }
//} // RenderWidgets