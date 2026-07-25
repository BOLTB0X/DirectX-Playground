// UserInterface/Widget/StatsWidget.cpp
#include "StatsWidget.h"
#include "imgui.h"

/* default */
////////////////////////////////////////////////////////////////////

StatsWidget::StatsWidget(Fps* fps, Timer* timer)
	: IWidget("Stats"),
	m_fpsPtr(fps),
	m_timerPtr(timer)
{
} // StatsWidget


void StatsWidget::Frame()
{
	if (i_isVisible == false) return;

    if (ImGui::BeginChild("StatsChild", ImVec2(0, 80), true))
    {
        ImGui::Text("FPS: %d", m_fpsPtr->GetFps());
        ImGui::Text("FrameTime: %.3f ms", m_timerPtr->GetTime() * 1000.0f);

        float avg = 1000.0f / ImGui::GetIO().Framerate;
        ImGui::Text("Avg: %.3f ms", avg);
    }
    ImGui::EndChild();
} // Frame
