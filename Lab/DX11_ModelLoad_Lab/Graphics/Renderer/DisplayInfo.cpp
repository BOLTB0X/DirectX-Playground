// Graphics/Renderer/DisplayInfo.cpp
#include "DisplayInfo.h"
#include "Common/EngineHelper.h"

#include <vector>


/* defualt */
/////////////////////////////////////////////////////////////////////

DisplayInfo::DisplayInfo()
    : m_numerator(0),
    m_denominator(1),
    m_videoCardMemory(0)
{
    m_videoCardDescription[0] = '\0';
} // DisplayInfo


DisplayInfo::~DisplayInfo() {} // ~DisplayInfo


bool DisplayInfo::Init(int screenWidth, int screenHeight) {
    IDXGIFactory* factory;
    IDXGIAdapter* adapter;
    IDXGIOutput* adapterOutput;
    unsigned int numModes = 0;
    DXGI_ADAPTER_DESC adapterDesc;

    // DXGI 팩토리 생성
    if (EngineHelper::SuccessCheck(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory),
        "DXGI Factory 생성 실패")
        == false) return false;

    // 기본 그래픽 카드(어댑터) 정보 가져오기
    if (EngineHelper::SuccessCheck(factory->EnumAdapters(0, &adapter),
        "기본 그래픽 어댑터 탐색 실패")
        == false) {
        factory->Release();
        return false;
    }

    // 기본 출력 장치(모니터) 정보 가져오기
    if (EngineHelper::SuccessCheck(adapter->EnumOutputs(0, &adapterOutput),
        "기본 출력 장치(모니터) 탐색 실패")
        == false) {
        adapter->Release();
        factory->Release();
        return false;
    }

    // 지원하는 디스플레이 모드 리스트 개수 파악
    if (EngineHelper::SuccessCheck(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
        DXGI_ENUM_MODES_INTERLACED, &numModes, NULL),
        "디스플레이 모드 개수 파악 실패")
        == false) return false;

    // 모드 리스트 채우기
    std::vector<DXGI_MODE_DESC> displayModeList(numModes);
    if (EngineHelper::SuccessCheck(
        adapterOutput->
            GetDisplayModeList(
                DXGI_FORMAT_R8G8B8A8_UNORM,
                DXGI_ENUM_MODES_INTERLACED,
                &numModes,
                displayModeList.data()),
        "디스플레이 모드 리스트 획득 실패") == false) return false;

    // 새로고침 빈도(Refresh Rate) 추출
    for (unsigned int i = 0; i < numModes; i++) {
        if (displayModeList[i].Width == (unsigned int)screenWidth) {
            if (displayModeList[i].Height == (unsigned int)screenHeight) {
                m_numerator = displayModeList[i].RefreshRate.Numerator;
                m_denominator = displayModeList[i].RefreshRate.Denominator;
            }
        }
    }

    // 그래픽 카드 설명 및 메모리 저장
    if (EngineHelper::SuccessCheck(adapter->GetDesc(&adapterDesc),
        "그래픽 카드 상세정보 획득 실패")
        == false) return false;

    m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

    size_t stringLength;
    if (wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128) != 0) {
        return false;
    }

    adapterOutput->Release();
    adapter->Release();
    factory->Release();

    return true;
} // Init