// System/Gui/Gui.h
#pragma once

#include <d3d11.h>
#include <vector>
#include <memory>

#include "Framework/Widget/IWidget.h"

class Gui {
public:
    Gui();
	Gui(const Gui&) = delete;
    ~Gui();

    bool Init(HWND, ID3D11Device*, ID3D11DeviceContext*);
    void Shutdown();

    void Begin();
    void End();
}; // Gui