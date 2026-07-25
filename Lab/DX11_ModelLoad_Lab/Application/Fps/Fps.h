// System/Fps.h
#pragma once
#pragma comment(lib, "winmm.lib")

#include <windows.h>
#include <mmsystem.h>


class Fps
{
public:
	Fps();
	Fps(const Fps&) = delete;
	~Fps();

	void Init();
	void Frame();

public:
	int GetFps() { return m_fps; };

private:
	int m_fps, m_count;
	unsigned long m_startTime;

}; // Fps
