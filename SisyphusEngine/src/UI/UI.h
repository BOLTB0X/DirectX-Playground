#pragma once
// STL
#include <memory>
#include <vector>
#include <string>
#include <DirectXMath.h>
// Framework
#include "IImGUI.h"
// Rendering엔진
#include "Shader/ShaderBuffers.h"
// Common
#include "PropertyHelper.h"

class IWidget;


class UI {
public:
    UI();
    UI(const UI& other) = delete;
    ~UI();

    bool Init(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
    void Shutdown();

    void Render();

    void Begin();
    void End();
    bool CanControlWorld() const;

public:
    void AddWidget(std::unique_ptr<IWidget>);
    void ToggleWidget();
    void CreateWidget(
        PropertyHelper::Property<float>,
        PropertyHelper::Property<int>,
        PropertyHelper::Property<DirectX::XMFLOAT3>,
        PropertyHelper::Property<DirectX::XMFLOAT3>,
        PropertyHelper::Property<float>,
        PropertyHelper::Property<bool>,
        PropertyHelper::Property<bool>,
        PropertyHelper::Property<bool>);
    void CreateWidget(PropertyHelper::Property<WaterBuffer>);
    void CreateWidget(PropertyHelper::Property<CloudBuffer>);
    void CreateWidget(PropertyHelper::Property<SkyBuffer>);
    void CreateWidget(PropertyHelper::Property<LensFlareBuffer>);
    void CreateWidget(PropertyHelper::Property<VolumetricCloudsBuffer>);
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
    std::unique_ptr<IImGUI> m_Gui;
    std::vector<std::unique_ptr<IWidget>> m_widgets;
    bool m_isCameraLocked;
}; // UI