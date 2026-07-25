#include "Pch.h"
#include "AtmosphereWidget.h"
#include "ImGuiDrawer.h"
#include "Objects/SkyBox.h"

AtmosphereWidget::AtmosphereWidget(SkyBox* sky) {
	m_SkyBox = sky;
} // AtmosphereWidget

AtmosphereWidget::~AtmosphereWidget() {
	m_SkyBox = nullptr;
} // ~

void AtmosphereWidget::Frame() {
	//if (ImGuiDrawer::DrawAtmosphere(&m_SkyBox->GetAtmosphereBuffer())) {
	//}
} // Frame