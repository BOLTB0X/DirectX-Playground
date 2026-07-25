// Application/Timer.h
#pragma once

#include <windows.h>


class Timer
{
public:
	Timer();
	Timer(const Timer&) = delete;
	~Timer();

	bool Init();
	void Frame();

public:
	float GetTime();
	void StartTimer();
	void StopTimer();
	int GetTiming();

private:
	float m_frequency;
	INT64 m_startTime;
	float m_frameTime;
	INT64 m_beginTime, m_endTime;
}; // Timer