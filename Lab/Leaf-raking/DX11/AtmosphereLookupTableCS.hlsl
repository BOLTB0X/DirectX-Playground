// AtmosphereLookupTableCS.hlsl
#include "Common.hlsli"
#include "Atmosphere.hlsli"

cbuffer AtmosphereBuffer : register(b3)
{
    // [Row 1] 단순 그라데이션
    float4 aZenithColor;
    // [Row 2] 단순 그라데이션
    float4 aHorizonColor;
    // [Row 3] 행성 물리 데이터
    float3 aPlanetCenter;
    float  aPlanetRadius;
    // [Row 4] 대기권 물리 데이터
    float  aAtmoRadius;
    float3 aPadding;
    // [Row 5] 산란 계수 (Rayleigh)
    float3 aRayleighBeta;
    float  aMieBeta;
    // [Row 6] 흡수 및 주변광
    float3 aAbsorptionBeta;
    float  aAmbientBeta;
    // [Row 7] 고도 상수 (Density Falloff)
    float  aRayleighHeight;
    float  aMieHeight;
    float  aAbsorptionHeight;
    float  aAbsorptionFalloff;
    // [Row 8] Mie 위상 함수 및 샘플링 설정
    float  aG;
    int    aPrimarySteps;
    int    aLightSteps;
    float  aIntensity;
    // [Row 9] 지표면 색상
    float3 aGroundColor;
    float  aPadding2;
    // [Row 10] 지표면 레이마칭 설정
    int    aGroundPrimarySteps;
    int    agroundLightSteps;
    float2 aPadding3;
}; // AtmosphereBuffer

RWTexture2D<float4> OutLUT : register(u0);

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    float width, height;
    OutLUT.GetDimensions(width, height);

    if (DTid.x >= width || DTid.y >= height)
    {
        return;
    }

    // UV를 각도로 매핑
    // x축: 시선 벡터와 천정(Up) 사이의 각도 cos(theta)
    // y축: 태양 벡터와 천정(Up) 사이의 각도 cos(phi)
    float2 uv = float2(DTid.xy) / float2(width, height);

    // X축: 시선 방향의 높이 (천정 각도)
    float cosTheta = uv.x * 2.0f - 1.0f;
    // Y축: 시선과 태양 사이의 각도 (산란의 핵심)
    float cosGamma = uv.y * 2.0f - 1.0f;

    float3 up = float3(0, 1, 0);
    
    // 1. 임의의 시선 방향 rd 생성 (높이만 cosTheta에 맞춤)
    float sinTheta = sqrt(max(0.0f, 1.0f - cosTheta * cosTheta));
    float3 rd = float3(sinTheta, cosTheta, 0.0f);

    // 2. rd와 cosGamma만큼의 각도를 가진 가상의 태양 방향 sd 생성
    // (이게 포인트입니다! rd와의 상대 각도를 직접 고정합니다)
    float sinGamma = sqrt(max(0.0f, 1.0f - cosGamma * cosGamma));
    float3 sd = float3(sinTheta * cosGamma + cosTheta * sinGamma,
                       cosTheta * cosGamma - sinTheta * sinGamma,
                       0.0f); // 수식적으로 rd와 sd의 dot 결과가 cosGamma가 되게 설정
    
    // 3. 레이마칭 위치 (행성 표면)
    float3 ro_km = float3(0, aPlanetRadius + 0.01, 0);

    // 4. 산란 계산 (sd를 직접 넣으세요)
    float3 col = calculate_atmosphere_scattering(
        ro_km, rd, MAX_DIST,
        float3(0, 0, 0), // scene_color는 0으로 (순수 하늘용)
        -sd, // 태양 방향
        float3(aIntensity, aIntensity, aIntensity),
        aPlanetCenter, aPlanetRadius, aAtmoRadius,
        aRayleighBeta, aMieBeta, aAbsorptionBeta,
        float3(aAmbientBeta, aAmbientBeta, aAmbientBeta),
        aG, aRayleighHeight, aMieHeight, aAbsorptionHeight, aAbsorptionFalloff,
        aPrimarySteps, aLightSteps
    );
    
    OutLUT[DTid.xy] = float4(col, 1.0f);
} // main