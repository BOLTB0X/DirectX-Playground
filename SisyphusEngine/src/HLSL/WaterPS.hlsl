// 레스터택 코드 참고
#include "Common.hlsli"


SamplerState SampleType : register(s0);
Texture2D reflectionTexture : register(t0);
Texture2D refractionTexture : register(t1);
Texture2D normalTexture : register(t2);


cbuffer WaterBuffer : register(b4)
{
    // Row 1
    float3 iWaterBaseColor;
    float iWaterTranslation;
    
    // Row 2
    float iReflectRefractScale;
    float iWaveLength;
    float iSpecularShininess;
    float iWaterAlpha;
    
    // Row 3
    float2 iWindDirection;
    float iWindForce;
    float iFinalAlpha;
    
    // Row 4
    float iHighlightsSize;
    float iSunColumnWidth;
    float iSunColumnIntensity;
    float iSparkleIntensity;
}; // WaterBuffer


struct WaterPixelInput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float4 reflectionPosition : TEXCOORD1;
    float4 refractionPosition : TEXCOORD2;
    float4 worldPosition : TEXCOORD3;
}; // PixelInput


float2 getScreenUV(float4 clipPosition)
{
    float2 uv;
    
    uv.x = clipPosition.x / clipPosition.w / 2.0f + 0.5f;
    uv.y = -clipPosition.y / clipPosition.w / 2.0f + 0.5f;
    return uv;
} // getScreenUV


float3 waterNormal(float2 texCoord)
{
    // 바람 적용
    float2 scaledUV = texCoord / iWaveLength;
    float2 moveOffset = normalize(iWindDirection) * (iWaterTranslation * iWindForce);

    float2 uv1 = scaledUV + moveOffset;
    float2 uv2 = scaledUV - (moveOffset * 0.5f) + float2(0.1f, 0.1f);
    
    float3 n1 = normalTexture.Sample(SampleType, uv1).xyz * 2.0f - 1.0f;
    float3 n2 = normalTexture.Sample(SampleType, uv2).xyz * 2.0f - 1.0f;
    
    return normalize(n1 + n2);
} // waterNormal


float4 visualSpecular(float2 screenUV, float3 normal)
{
    float2 distToSunVec = screenUV - iLightUV;
    float2 distortedDist = distToSunVec + (normal.xy * iSparkleIntensity);

    // 태양 중심부 하이라이트
    float sunSpot = exp(-length(distortedDist) * (iSpecularShininess * iHighlightsSize));
    
    // 수직으로 늘어지는 빛 기둥
    float verticalDist = saturate(screenUV.y - iLightUV.y);
    float distanceFade = pow(verticalDist, 0.5f);
    float sunColumn = exp(-abs(distortedDist.x) * iSunColumnWidth) * verticalDist * distanceFade;
    
    float finalSpecular = saturate(sunSpot + sunColumn * iSunColumnIntensity);
    
    return iLightColor * iIntensity * finalSpecular;
} // visualSpecular


float4 main(WaterPixelInput input) : SV_TARGET
{
    // UV
    float2 screenUV = getScreenUV(input.refractionPosition);
    float2 reflectUV = getScreenUV(input.reflectionPosition);
    float2 refractUV = screenUV;

    // 노멀 및 왜곡
    float3 normal = waterNormal(input.tex);
    float2 distortion = normal.xy * iReflectRefractScale;

    reflectUV = clamp(reflectUV + distortion, 0.001f, 0.999f);
    refractUV = clamp(refractUV + distortion, 0.001f, 0.999f);

    // 텍스처 샘플링
    float4 reflectionColor = reflectionTexture.Sample(SampleType, reflectUV);
    float4 refractionColor = refractionTexture.Sample(SampleType, refractUV);

    // 스펙큘러 하이라이트 보정
    float4 specularHighlight = visualSpecular(screenUV, normal);

    float4 waterColor = lerp(refractionColor, reflectionColor, iWaterAlpha);
    float4 finalBase = float4(iWaterBaseColor, iWaterAlpha);
    float4 finalColor = lerp(finalBase, waterColor, iFinalAlpha);

    return saturate(finalColor + specularHighlight);
} // main