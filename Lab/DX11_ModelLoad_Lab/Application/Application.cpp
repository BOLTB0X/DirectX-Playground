// Application/Application.h
#include "Application.h"
#include "Timer/Timer.h"
#include "Fps/Fps.h"
#include "Manager/ShaderManager.h"
#include "Manager/ModelManager.h"
#include "Manager/TexturesManager.h"
// System
#include "Input/Input.h"
#include "Gui/Gui.h"
// UserInterface
#include "UserInterface.h"
#include "UserInput/UserInput.h"
// World
#include "World.h"
// Common
#include "EngineHelper.h"
#include "EngineSettings.h"
// Graphics
#include "Renderer/Renderer.h"
#include "Camera/Camera.h"
// UserInterface
#include "Widget/MainSideBarWidget.h"
#include "Widget/StatsWidget.h"
#include "Widget/InspectorWidget.h"
#include "Widget/ControlWidget.h"
// imgui
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

/* default */
/////////////////////////////////////////////////////////////////////

Application::Application() { } // Application

Application::~Application()
{
	Shutdown();
} // ~Application


bool Application::Init(HWND hwnd,
	std::shared_ptr<Input> input,
	std::shared_ptr<Gui> gui)
{
	m_Timer = std::make_unique<Timer>();
	if (m_Timer->Init() == false)
	{
		EngineHelper::ErrorBox(hwnd, L"Timer 객체 초기화 실패");
		return false;
	}

	m_Fps = std::make_unique<Fps>();
	m_Fps->Init();

	m_UserInput = std::make_unique<UserInput>();
	if (m_UserInput->Init(input) == false) {
		EngineHelper::ErrorBox(hwnd, L"m_UserInput->Init 실패");
		return false;
	}

	m_Renderer = std::make_unique<Renderer>();
	if (m_Renderer->Init(hwnd, EngineSettings::SCREEN_WIDTH, EngineSettings::SCREEN_HEIGHT)
		== false)
	{
		EngineHelper::ErrorBox(hwnd, L"Renderer 초기화 단계에서 실패했습니다.");
		return false;
	}

	m_TextureManager = std::make_unique<TexturesManager>();
	m_ShaderManager = std::make_unique<ShaderManager>();
	if (m_ShaderManager->Init(
		m_Renderer->GetDX11Device()->GetDevice(),
		hwnd) == false)
	{
		EngineHelper::ErrorBox(hwnd, L"ShaderManager 초기화 실패");
		return false;
	}

	m_ModelManager = std::make_unique<ModelManager>();

	m_Camera = std::make_unique<Camera>();
	m_Camera->InitProjection(EngineSettings::SCREEN_WIDTH, EngineSettings::SCREEN_HEIGHT,
		EngineSettings::SCREEN_NEAR, EngineSettings::SCREEN_DEPTH);

	m_World = std::make_unique<World>();
	WorldInitParam parm = {
		m_Renderer->GetDX11Device()->GetDevice(),
		m_Renderer->GetDeviceContext(),
		m_ModelManager.get(),
		m_ShaderManager.get(),
		m_TextureManager.get(),
		hwnd
	};

	if (m_World->Init(parm) == false)
	{
		EngineHelper::ErrorBox(hwnd, L"World 초기화 실패");
		return false;
	}

	if (gui->Init(hwnd,
		m_Renderer->GetDX11Device()->GetDevice(),
		m_Renderer->GetDeviceContext())
		== false)
	{
		EngineHelper::ErrorBox(hwnd, L"Gui 초기화 실패");
		return false;
	}

	m_UserInterface = std::make_unique<UserInterface>();
	if (m_UserInterface->Init(gui) == false)
	{
		EngineHelper::ErrorBox(hwnd, L"m_UserInterface->Init 실패");
		return false;
	}
	m_UserInterface->CreateSideBar("F1:Sisyphus Editor", m_Timer.get(), m_Fps.get(), m_Renderer.get(), m_World.get());

	return true;
} // Init


void Application::Shutdown()
{

	if (m_World)
		m_World->Shutdown();
	
	if (m_ModelManager)
		m_ModelManager->Shutdown();

	if (m_ShaderManager)
		m_ShaderManager->Shutdown();

	if (m_TextureManager)
		m_TextureManager->Shutdown();

	if (m_Renderer)
	{
		m_Renderer->Shutdown();
		m_Renderer = 0;
	}

	return;
} // Shutdown


bool Application::Frame()
{
	m_Timer->Frame();
	m_Fps->Frame();
	m_UserInput->Frame();

	if (m_UserInput->IsF1Toggled())
		m_UserInterface->ToggleMainSideBar();

	if (m_UserInput->IsEscapePressed())
		return false;

	if (m_UserInterface->IsWorldClicked(m_UserInput->IsMouseLPressed()))
	{
		m_UserInterface->SetCameraLocked(!m_UserInterface->IsCameraLocked());
	}

	if (m_UserInterface->CanControlWorld() && m_UserInterface->IsCameraLocked() == false)
	{
		Position* camPos = m_World->GetCamera()->GetPosition();
		m_UserInput->ProcessInput(m_Timer->GetTime(), camPos);
	}

	m_World->Frame(m_Timer->GetTime(), m_UserInput.get(), m_UserInterface->CanControlWorld());

	return Render();
} // Frame

/////////////////////////////////////////////////////////////////////

/* private */
/////////////////////////////////////////////////////////////////////

bool Application::Render()
{
	m_Renderer->BeginScene(0.5f, 0.0f, 0.0f, 1.0f);

	m_UserInterface->ApplyRenderStates(m_Renderer.get());
	m_World->Render(m_ShaderManager.get(), m_Renderer.get());
	m_UserInterface->Render();

	m_Renderer->EndScene();

	return true;
} // Render
