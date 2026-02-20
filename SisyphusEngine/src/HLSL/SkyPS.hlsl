// https://www.shadertoy.com/view/4dl3zr, 배경과 태양부분 참고
#include "Common.hlsli"
#include "Maths.hlsli"


struct PixelSkyInput
{
    float4 position : SV_POSITION;
    float3 localPos : TEXCOORD0;
}; // PixelSkyInput


cbuffer SkyBuffer : register(b3)
{
    // Row 1: 하늘 기본 색상
    float3 iSkyTopColor;
    float iSkyExponent;

    // Row 2: 지평선 및 태양 거리 스케일
    float3 iSkyHorizonColor;
    float iSunDistScale;

    // Row 3: 하단 색상 및 태양 크기
    float3 iSkyLowerColor;
    float iSunSize;

    // Row 4: 태양 산란 색상 및 범위 제어
    float3 iAtmosphereColor;
    float iWideGlowScale;

    // Row 5: 태양 강도 및 감쇄 속성
    float iSunBloom;
    float iSunIntensity;
    float iBloomMult;
    float iGlowMult;

    // Row 6: 레이 및 시간 속성
    float iRayFreq;
    float iRayTimeScale;
    float2 iPadding;
}; // SkyBuffer


// 태양과의 각도 거리 계산
float getSunDistance(float3 rd, float3 sd)
{
    float dotProduct = dot(rd, sd);
    
    if (dotProduct <= 0.0f)
        return 999.0f;

    float dist = length(rd / dotProduct - sd);
    return dist * iSunDistScale;
} // getSunDistance


// 태양 본체와 주변 글로우 계산
float3 coreGlow(float sunDist, float3 lightColor)
{
    float core = smoothstep(iSunSize, iSunSize * 0.2f, sunDist);
   
    float bloomDist = sunDist;
    float bloom = exp(-bloomDist * iSunBloom) * iBloomMult;
    float wideGlow = pow(saturate(1.0 / (1.0 + sunDist * iWideGlowScale)), 2.0) * iGlowMult;

    float3 atmosphereColor = lerp(lightColor, iAtmosphereColor, 0.3);

    return (lightColor * core * iSunIntensity) + (atmosphereColor * (bloom + wideGlow));
} // coreGlow


// 배경이 되는 하늘 계산
float3 skyBackground(float3 rd)
{
    float horizonFactor = pow(abs(rd.y), iSkyExponent);
    
    if (rd.y > 0)
        return lerp(iSkyHorizonColor, iSkyTopColor, horizonFactor);
    
    // 지평선 아래는 어두운 색상으로 처리
    return lerp(iSkyHorizonColor, iSkyLowerColor, horizonFactor);
} // skyBackground


float4 main(PixelSkyInput input) : SV_TARGET
{
    float3 rd = normalize(input.localPos);
    float3 sd = normalize(iLightDirection);
    
    float3 finalColor = skyBackground(rd);
    float sunDist = getSunDistance(rd, sd);
    
    finalColor += coreGlow(sunDist, iLightColor.rgb) * iIntensity;
 
    return float4(finalColor, 0.8f);
} // main