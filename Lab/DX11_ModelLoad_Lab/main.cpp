// main.cpp
#include "System/System.h"

int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow
)
{
    System* system;
    bool result;

    system = new System;
    result = system->Init();

    if (result) system->Run();

    system->Shutdown();
    delete system;
    system = 0;

    return 0;
} // wWinMain