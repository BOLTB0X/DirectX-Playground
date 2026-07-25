// UserInterface/UserInterface.h
#pragma once
#include <memory>
#include <vector>
#include <string>
// Framework
#include "Widget/IWidget.h"

class Gui;
class Fps;
class Timer;
class Renderer;
class MainSideBarWidget;
class World;

class UserInterface {
public:
    UserInterface();
    UserInterface(const UserInterface& other) = delete;
    ~UserInterface();

    bool Init(std::shared_ptr<Gui> gui);
    void Frame();
    void Render();

    void Begin();
    void End();
    bool CanControlWorld() const;


public:
    void AddWidget(std::unique_ptr<IWidget>);
    void ToggleMainSideBar();
    void CreateSideBar(std::string, Timer*, Fps*, Renderer*, World*);
    void ApplyRenderStates(Renderer*);
    bool IsWorldClicked(bool mousePressed) const;

public:
    template<typename T>
    T* GetWidget(const std::string& name)
    {
        for (auto& widget : m_widgets) {
            if (widget->GetName() == name)
                return dynamic_cast<T*>(widget.get());
        }
        return nullptr;
    }

    bool IsCameraLocked() const { return m_isCameraLocked; }
    void SetCameraLocked(bool lock) { m_isCameraLocked = lock; }

private:
    std::shared_ptr<Gui> m_Gui;
    std::vector<std::unique_ptr<IWidget>> m_widgets;
    MainSideBarWidget* m_MainSideBar;
    bool m_isCameraLocked = false;
}; // UserInterface