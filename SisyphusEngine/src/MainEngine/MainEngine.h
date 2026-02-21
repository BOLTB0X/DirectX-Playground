#pragma once
#include <memory>
#include <windows.h>


class Input;
class Timer;
class Camera;
class Fps;
class UI;
class InputManager;
class RenderingEngine;


class MainEngine {
public:
    MainEngine();
    MainEngine(const MainEngine& other) = delete;
    ~MainEngine();

    bool Init(HWND hwnd, std::shared_ptr<InputManager>);
    void Shutdown();
    bool Frame();

private:
    void Render();
    bool UpdateUserInput();
    void UpdateRenderStatesWidget();
    void CreateWidget();

private:
    // 공유
    std::shared_ptr<InputManager> m_InputManager;
    // 유틸
    std::unique_ptr<Timer> m_Timer;
    std::unique_ptr<Fps> m_Fps;
    // 인터페이스
    std::unique_ptr<UI> m_UI;
    // 그래픽
    std::unique_ptr<Camera> m_Camera;
    std::unique_ptr<RenderingEngine> m_RenderingEngine;
}; // MainEngine