// UserInterface/Widget/ControlWidget.cpp
#include "ControlWidget.h"
#include "imgui.h"

// Graphics
#include "Renderer.h"
// Framework
#include "Position.h"

/* default */
/////////////////////////////////////////////////////////////////////


ControlWidget::ControlWidget(Renderer* renderer, World* world)
	: IWidget("Control"),
	m_Renderer(renderer),
	m_isWireframe(false),
	m_isCullNone(false)
{
} // ControlWidget


ControlWidget::~ControlWidget()
{
    Shutdown();
} // ControlWidget


void ControlWidget::Frame()
{
    ImGui::TextDisabled("Rasterizer Settings");

    if (ImGui::Checkbox("Wireframe Mode", &m_isWireframe))
    {
    }

    const char* cullingLabel = m_isCullNone ? "Both Sides (Cull None)" : "Back-face Culling";
    if (ImGui::Checkbox(cullingLabel, &m_isCullNone))
    {
    }

    ImGui::Separator();

} // Frame


void ControlWidget::Shutdown()
{
    if (m_Renderer) m_Renderer = nullptr;
} // Shutdown