// Graphics/Renderer/DisplayInfo.h
// 그래픽 카드 및 모니터 정보 조사
#pragma once
#include <dxgi.h>
#include <string>

class DisplayInfo {
public:
    DisplayInfo();
	DisplayInfo(const DisplayInfo&) = delete;
    ~DisplayInfo();

    bool Init(int, int);

public:
    unsigned int GetRefreshRateNumerator() const { return m_numerator; }
    unsigned int GetRefreshRateDenominator() const { return m_denominator; }
    int GetVideoCardMemory() const { return m_videoCardMemory; }
    const char* GetVideoCardDescription() const { return m_videoCardDescription; }

private:
    unsigned int m_numerator;
    unsigned int m_denominator;
    int m_videoCardMemory;
    char m_videoCardDescription[128];
}; // DisplayInfo