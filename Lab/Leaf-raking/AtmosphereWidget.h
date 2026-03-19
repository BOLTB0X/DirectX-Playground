#pragma once
#include "ImGuiWidget.h"

class SkyBox;

class AtmosphereWidget : public ImGuiWidget {
public:
    AtmosphereWidget(SkyBox* sky);
    virtual ~AtmosphereWidget();

    virtual void Frame() override;

private:
    SkyBox*     m_SkyBox;
}; // ImGuiWidget