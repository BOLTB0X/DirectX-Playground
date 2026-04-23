#pragma once
// Framework
#include "IWidget.h"

class Renderer;
class World;


class ControlWidget : public IWidget {
public:
    ControlWidget(Renderer* renderer, World* world);
	ControlWidget(const ControlWidget&) = delete;
	virtual ~ControlWidget() override;

    virtual void Frame() override;

public:
    bool IsWireframe() const { return m_isWireframe; }
	bool IsCullNone() const { return m_isCullNone; }

private:
    void Shutdown();

private:
    Renderer* m_Renderer;
    World* m_World;
    bool m_isWireframe;
    bool m_isCullNone;
}; // ControlWidget