#pragma once
// Common
#include "MathHelper.h"
#include "ConstantHelper.h"


namespace ShaderBufferKeys {
    const std::string Global = "Global";
    const std::string Matrix = "Matrix";
    const std::string Cloud = "Cloud";
    const std::string Bicubic = "Bicubic";
    const std::string Sky = "Sky";
    const std::string LensFlare = "LensFlare";
    const std::string Water = "Water";
    const std::string Light = "Light";
    const std::string Refraction = "Refraction";
} // ShaderBufferKeys


struct GlobalBuffer {
    // Row 1
    float iTime;
    float iFrame;
    DirectX::XMFLOAT2 iResolution;

    // Row 2
    DirectX::XMFLOAT3 iCameraPos;
    float padding1;

    DirectX::XMFLOAT4 padding2;
}; // GlobalBuffer


struct MatrixBuffer {
    DirectX::XMMATRIX model;
    DirectX::XMMATRIX view;
    DirectX::XMMATRIX projection;
}; // MatrixBuffer


struct LightBuffer {
    // Row 1
    DirectX::XMFLOAT3 direction;
    float intensity;

    // Row 2
    DirectX::XMFLOAT4 color;

    // Row 3
    DirectX::XMFLOAT2 lightUV;
    DirectX::XMFLOAT2 padding;

    LightBuffer()
    {
        direction = ConstantHelper::LightPosition;
        intensity = ConstantHelper::LightIntensity;
        color = ConstantHelper::LightColor;
        lightUV = { 0.5f, 0.5f };
        padding = { 0.0f, 0.0f };
    }


    LightBuffer(DirectX::XMFLOAT3 _direction, float _intensity,
        DirectX::XMFLOAT4 _color,
        DirectX::XMFLOAT2 _lightUV)
        : direction(_direction), intensity(_intensity),
        color(_color),
        lightUV(_lightUV)
    {
        padding = { 0.0f, 0.0f };
    }
}; // LightBuffer


struct CloudBuffer {
    // Row 1
    DirectX::XMFLOAT3 baseColor;
    float iCloudType;

    // Row 2
    DirectX::XMFLOAT3 ambient;
    float maxSteps;

    // Row 3
    DirectX::XMFLOAT3 shadowColor;
    float marchSize;

    // Row 4
    float radius;
    float height;
    float thickness;
    float noiseRes;

    // Row 5
    float densityScale;
    float falloffScale;
    float mieIntensity;
    float miePower;

    // Row 6
    float diffusePower;
    float lightMultiply;
    float shadowDist;
    float maxDepth;

    // Row 7
    DirectX::XMFLOAT3 windDir;
    float cloudSpeed;

    // Row 8
    float fbmScale;
    float fbmFactor;
    float fbmIncrement;
    float fbmPersistance;

    // Row 9
    int fbmOctaves;
    DirectX::XMFLOAT3 padding;

    CloudBuffer()
    {
        baseColor = { 1.0f, 1.0f, 1.0f };
        iCloudType = 1.0f;
        ambient = { 0.25f, 0.25f, 0.25f };
        shadowColor = { 0.02f, 0.08f, 0.25f };
        maxSteps = 100.0f;
        marchSize = 0.08f;

        radius = 2.0f;
        height = 1.0f;
        thickness = 2.0f;
        noiseRes = 256.0f;

        densityScale = 0.4f;
        falloffScale = 0.1f;
        mieIntensity = 3.0f; // 전방 산란 밝기
        miePower = 15.0f; // 전방 산란 날카로움

        diffusePower = 2.0f;
        lightMultiply = 4.0f;
        shadowDist = 0.4f;
        maxDepth = 50.0f;

        windDir = { 1.0f, -0.2f, -1.0f };
        cloudSpeed = 0.5f;

        fbmScale = 0.5f;
        fbmFactor = 2.02f;
        fbmIncrement = 0.21f;
        fbmPersistance = 0.5f;
        fbmOctaves = 6;

        padding = { 0.0f, 0.0f, 0.0f };
    }

    CloudBuffer(float cloudType)
        : iCloudType(cloudType)
    {
        baseColor = { 1.0f, 1.0f, 1.0f };
        ambient = { 0.25f, 0.25f, 0.25f };
        shadowColor = { 0.02f, 0.08f, 0.25f };
        maxSteps = 100.0f;
        marchSize = 0.08f;

        radius = 2.0f;
        height = 1.0f;
        thickness = 2.0f;
        noiseRes = 256.0f;

        densityScale = 0.4f;
        falloffScale = 0.1f;
        mieIntensity = 3.0f; // 전방 산란 밝기
        miePower = 15.0f; // 전방 산란 날카로움

        diffusePower = 2.0f;
        lightMultiply = 4.0f;
        shadowDist = 0.4f;
        maxDepth = 50.0f;

        windDir = { 1.0f, -0.2f, -1.0f };
        cloudSpeed = 0.5f;

        fbmScale = 0.5f;
        fbmFactor = 2.02f;
        fbmIncrement = 0.21f;
        fbmPersistance = 0.5f;
        fbmOctaves = 6;

        padding = { 0.0f, 0.0f, 0.0f };
    }
}; // CloudBuffer


struct SkyBuffer {
    // Row 1
    DirectX::XMFLOAT3 topColor;
    float skyExponent;

    // Row 2
    DirectX::XMFLOAT3 horizonColor;
    float sunDistScale;

    // Row 3
    DirectX::XMFLOAT3 lowerColor;
    float sunSize;

    // Row 4
    DirectX::XMFLOAT3 atmosphereColor;
    float wideGlowScale;

    // Row 5
    float sunBloom;
    float sunIntensity;
    float bloomMult;
    float glowMult;

    // Row 6
    float rayFreq;
    float rayTimeScale;
    DirectX::XMFLOAT2 padding;

    SkyBuffer()
    {
        topColor = { 0.02f, 0.08f, 0.25f };
        skyExponent = 0.35f;

        horizonColor = { 0.65f, 0.32f, 0.12f };
        sunDistScale = 1.5f;

        lowerColor = { 0.05f, 0.02f, 0.1f };
        sunSize = 0.005f;

        atmosphereColor = { 1.0f, 0.6f, 0.2f };
        wideGlowScale = 10.0f;

        sunBloom = 80.0f;
        sunIntensity = 0.8f;
        bloomMult = 3.5f;
        glowMult = 0.6f;

        rayFreq = 3.0f;
        rayTimeScale = 0.15f;
        padding = { 0.0f, 0.0f };
    }
}; // SkyBuffer


struct ThresholdBuffer {
    DirectX::XMFLOAT4 scale;
    DirectX::XMFLOAT4 bias;

    ThresholdBuffer()
    {
        scale = { 1.0f, 1.0f, 1.0f, 1.0f };
        bias = { -10.0f, -10.0f, -10.0f, 0.0f };
    }
}; // ThresholdBuffer


struct LensFlareBuffer {
    // Row 1: 기본 고스트 제어
    int   count;
    float spacing;
    float threshold;
    float alpha;

    // Row 2: 태양 위치 및 기본 글로우
    DirectX::XMFLOAT2 sunUV;
    float glowSize;
    float starScale;

    // Row 3: 고스트 물리 속성 및 태양 코어
    float ghostPull;
    float ghostIntensity;
    float ghostFalloff;
    float sunCoreTightness;

    // Row 4: 왜곡 및 휘도
    DirectX::XMFLOAT3 distortion;
    float padding1;

    // Row 5: 휘도 기준
    DirectX::XMFLOAT3 luminance;
    float padding2;

    // Row 6: F2 설정 (Offset + Sharpness)
    DirectX::XMFLOAT3 f2Offset;
    float f2Sharpness;

    // Row 7: F2 색상
    DirectX::XMFLOAT3 f2ColorMult;
    float padding3;

    // Row 8: F4 설정 (Offset + Power)
    DirectX::XMFLOAT3 f4Offset;
    float f4Power;

    // Row 9: F4 색상
    DirectX::XMFLOAT3 f4ColorMult;
    float padding4;

    // Row 10: F5 설정 (Offset + Power)
    DirectX::XMFLOAT3 f5Offset;
    float f5Power;

    // Row 11: F5 색상
    DirectX::XMFLOAT3 f5ColorMult;
    float padding5;

    // Row 12: F6 설정 (Offset + Power)
    DirectX::XMFLOAT3 f6Offset;
    float f6Power;

    // Row 13: F6 색상
    DirectX::XMFLOAT3 f6ColorMult;
    float padding6;

    // Row 14~17: 행렬
    DirectX::XMMATRIX lensMatrix;

    LensFlareBuffer()
    {
        // 기본 제어
        count = 8;
        spacing = 0.25f;
        threshold = 0.9f;
        alpha = 1.0f;

        // 태양 관련
        sunUV = { 0.5f, 0.5f };
        glowSize = (float)ConstantHelper::SCREEN_WIDTH / (float)ConstantHelper::SCREEN_HEIGHT;
        starScale = 0.8f;

        // 고스트 속성
        ghostPull = 0.1f;
        ghostIntensity = 1.5f;
        ghostFalloff = 1.0f;
        sunCoreTightness = 36.0f;

        distortion = { -0.005f, 0.0f, 0.005f };
        luminance = { 0.3f, 0.59f, 0.11f };

        // F2 파라미터
        f2Offset = { 0.80f, 0.85f, 0.90f };
        f2Sharpness = 32.0f;
        f2ColorMult = { 0.25f, 0.23f, 0.21f };

        // F4 파라미터
        f4Offset = { 0.40f, 0.45f, 0.50f };
        f4Power = 2.4f;
        f4ColorMult = { 6.0f, 5.0f, 3.0f };

        // F5 파라미터
        f5Offset = { 0.20f, 0.40f, 0.60f };
        f5Power = 5.5f;
        f5ColorMult = { 2.0f, 2.0f, 2.0f };

        // F6 파라미터
        f6Offset = { -0.3f, -0.325f, -0.35f };
        f6Power = 1.6f;
        f6ColorMult = { 6.0f, 3.0f, 5.0f };

        // 패딩 초기화
        padding1 = padding2 = padding3 = padding4 = padding5 = padding6 = 0.0f;

        lensMatrix = DirectX::XMMatrixIdentity();
    }
}; // LensFlareBuffer


struct WaterBuffer {
    // Row 1: 기본 색상 및 기본 변환
    DirectX::XMFLOAT3 waterBaseColor;
    float waterTranslation;

    // Row 2: 왜곡 및 파도 설정
    float reflectRefractScale;
    float waveLength;
    float specularShininess;
    float waterAlpha;

    // Row 3: 바람 설정
    DirectX::XMFLOAT2 windDirection;
    float windForce;
    float finalAlpha;

    // Row 4:스펙큘러 상세 제어
    float highlightsSize; // 하이라이트 크기
    float sunColumnWidth; // 빛 기둥 너비
    float sunColumnInensity; // 빛 기둥 강도
    float sparkleIntensity; // 물결 자글거림 강도

    WaterBuffer() {
        waterBaseColor = { 0.1f, 0.15f, 0.2f };
        waterTranslation = 0.0f;
        reflectRefractScale = 0.03f;
        waveLength = 0.5f;
        specularShininess = 200.0f;
        waterAlpha = 0.6f;
        windDirection = { 1.0f, 0.5f };
        windForce = 0.5f;
        finalAlpha = 0.9f;

        highlightsSize = 0.05f;
        sunColumnWidth = 30.0f;
        sunColumnInensity = 0.5f;
        sparkleIntensity = 0.1f;
    }
}; // WaterBuffer


struct RefractionBuffer {
    DirectX::XMFLOAT4 clipPlane;

    RefractionBuffer() {
        clipPlane = { 0.0f, 1.0f, 0.0f, 0.0f };
    }
}; // RefractionBuffer