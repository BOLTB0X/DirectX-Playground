// Common/EngineSeetings.h
#pragma once
#include <iostream>
#include <string>


namespace EngineSettings {
    // 화면 설정
    const bool FULL_SCREEN = false;
    const bool VSYNC_ENABLED = true;

    // 기본 해상도
    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 600;

    // 클리핑 평면
    const float SCREEN_DEPTH = 1000.0f;
    const float SCREEN_NEAR = 0.1f;

    const int terrainHeight = 1024;
    const int terrainWidth = 1024;
    const int terrainScaling = 200;

    const float MOUNT_X = 400.0f;
    const float MOUNT_Z = 400.0f;
    const float STONE_X = 300.0f;
    const float STONE_Z = 300.0f;

    // 모델
    ////////////////////////////////////////////////////////////////////////////

    const std::string ASSET_PATH = "assets/";

    const std::string STONE_PATH = ASSET_PATH + "Stone/Stone.fbx";
    const std::string MOUNTAIN_PATH = ASSET_PATH + "Mountain/Mountain.fbx";
    const std::string ClOUD_PATH = ASSET_PATH + "Cloud/Cloud2.fbx";


    /// <summary>
    /// 터레인
    /// </summary>
    const std::string TERRAIN_PATH = ASSET_PATH + "Terrain/setup.txt";
    const std::string TERRAIN_HEIGHTMAP_PATH = "assets/Terrain/heightmap.r16";
    const std::string TERRAINTEX_PATH = ASSET_PATH + "Terrain/textures/";

    const std::string TERRAINDEN_PATH = TERRAINTEX_PATH + "cloud001.dds";
    const std::string TERRAINPER_PATH = TERRAINTEX_PATH + "perturb001.dds";
    const std::string TERRAINNORM_PATH = TERRAINTEX_PATH + "Cloud_normal.jpg";
    /*const std::string TERRAINNAO_PATH = TERRAINTEX_PATH + "Cloud_ao_texture.jpeg";
    const std::string TERRAINNALPHA_PATH = TERRAINTEX_PATH + "Cloud_alpha.jpeg";
    const std::string TERRAINNROUGH_PATH = TERRAINTEX_PATH + "Cloud_Roughness.png";*/

    ///////////////////////////////////////////////////////////////////////////////////////////

    // PBRT 접미사

    const std::string PBRT_SUFFIX_BaseColor = "_BaseColor";
    const std::string PBRT_SUFFIX_NORM = "_normal";
    const std::string PBRT_SUFFIX_ALPHA = "_alpha";
    const std::string PBRT_SUFFIX_AO = "_ao";
    const std::string PBRT_SUFFIX_ROUG = "_Roughness";
    const std::string PBRT_SUFFIX_META = "_Metallic";


    // HLSL
    //////////////////////////////////////////////////////////////////////////////////////////////

    const std::wstring SHADER_PATH = L"hlsl/";

    const std::wstring COLOR_VS = L"ColorVS.hlsl";
    const std::wstring COLOR_PS = L"ColorPS.hlsl";

    const std::wstring STONE_VS = L"Stone/StoneVS.hlsl";
    const std::wstring STONE_PS = L"Stone/StonePS.hlsl";

    const std::wstring ACTORS_VS = L"Actors/ActorsVS.hlsl";
    const std::wstring ACTORS_PS = L"Actors/ActorsPS.hlsl";

    const std::wstring TERRAIN_VS = L"Terrain/TerrainVS.hlsl";
    const std::wstring TERRAIN_PS = L"Terrain/TerrainPS.hlsl";

    const std::wstring CLOUD_VS = L"Cloud/CloudVS.hlsl";
    const std::wstring CLOUD_PS = L"Cloud/CloudPS.hlsl";


    ////////////////////////////////////////////////////////////////////////////////////////////////
};