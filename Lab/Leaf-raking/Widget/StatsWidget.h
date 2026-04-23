// UserInterface/Widget/StatsWidget.h
#pragma once
// Framework
#include "IWidget.h"
// main
#include "Fps.h"
#include "Timer.h"

class StatsWidget : public IWidget {
public:
    StatsWidget(Fps*, Timer*);
	StatsWidget(const StatsWidget&) = delete;
	virtual ~StatsWidget() = default;

    virtual void Frame() override;

private:
    Fps* m_fpsPtr;
    Timer* m_timerPtr;
}; // StatsWidget