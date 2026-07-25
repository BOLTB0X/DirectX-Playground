// Application/Application.h
#pragma once
#include <windows.h>
#include <memory>

class UserInput;
class UserInterface;
class Timer;
class Fps;
class MainSideBarWidget;
class Renderer;
class ShaderManager;
class ModelManager;
class TexturesManager;
class World;
class Camera;
class Gui;
class Input;

class Application {
public:
    Application();
    Application(const Application&) = delete;
    ~Application();

    bool Init(HWND,
        std::shared_ptr<Input>,
        std::shared_ptr<Gui>);
    void Shutdown();
    bool Frame();

private:
    bool Render();

private: // 추상화 객체
    std::unique_ptr<UserInput> m_UserInput;
    std::unique_ptr<UserInterface> m_UserInterface;

private: // 유틸리티
    std::unique_ptr<Timer> m_Timer;
    std::unique_ptr<Fps> m_Fps;

private: // 엔진 컴포넌트
    std::unique_ptr<Renderer> m_Renderer;
    std::unique_ptr<ShaderManager> m_ShaderManager;
    std::unique_ptr<ModelManager> m_ModelManager;
    std::unique_ptr<TexturesManager> m_TextureManager;
    std::unique_ptr<World> m_World;
    std::unique_ptr<Camera> m_Camera;

}; // Application