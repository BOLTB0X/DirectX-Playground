#include "Common.hlsli"


Texture2D iInputTex : register(t0);
Texture2D iNoise : register(t1);
SamplerState iSampler : register(s0);

cbuffer MatrixBuffer : register(b0)
{
    matrix modelMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
}; // MatrixBuffer


cbuffer LensFlareBuffer : register(b3)
{
    // Row 1
    float2 iSunPos;
    float iThreshold;
    float iScale;

    // Row 2
    float iBias;
    float iGhostCount;
    float iGhostSpacing;
    float iDistortion;

    // Row 3
    float3 iTintColor;
    float iLF_Padding1;

    // Row 4
    float visibility;
    float2 iLF_Padding2;
}; // LensFlareBuffer


float noise(float2 t)
{
    return iNoise.Sample(iSampler, t).x;
} // noise


float3 ModifyGhostColor(float3 color, float factor, float factor2)
{
    float w = color.x + color.y + color.z;
    return lerp(color, float3(w, w, w) * factor, w * factor2);
} // ModifyGhostColor


float3 LensFlare(float2 uv, float2 pos)
{
    float2 main = uv - pos;
    float2 uvd = uv * (length(uv));
    
    float ang = atan2(main.x, main.y);
    float dist = length(main);
    dist = pow(dist, 0.1f);
        
    // 반대편에 맺히는 상
    float f2 = max(1.0 / (1.0 + 32.0 * pow(length(uvd + 0.8 * pos), 2.0)), 0.0) * 0.25;
    float f22 = max(1.0 / (1.0 + 32.0 * pow(length(uvd + 0.85 * pos), 2.0)), 0.0) * 0.23;
    float f23 = max(1.0 / (1.0 + 32.0 * pow(length(uvd + 0.9 * pos), 2.0)), 0.0) * 0.21;
    
    float2 uvx = lerp(uv, uvd, -0.5);
    float f4 = max(0.01 - pow(length(uvx + 0.4 * pos), 2.4), 0.0) * 6.0;
    float f42 = max(0.01 - pow(length(uvx + 0.45 * pos), 2.4), 0.0) * 5.0;
    float f43 = max(0.01 - pow(length(uvx + 0.5 * pos), 2.4), 0.0) * 3.0;

    float3 c = 0.0;
    c.r += f2 + f4;
    c.g += f22 + f42;
    c.b += f23 + f43;
    c = c * 1.3 - float3(length(uvd) * 0.05, length(uvd) * 0.05, length(uvd) * 0.05);
    c += (c * iTintColor);
    
    return c;
} // LensFlare


float4 main(PixelInput input) : SV_TARGET
{
    float2 uv = input.tex * 2.0f - 0.5f;
    float aspect = iResolution.x / iResolution.y; // 보정
    uv.x *= aspect;
    
    float2 sun = iSunPos * 2.0f - 1.0f;
    sun.x *= aspect;
    

    float3 flare = LensFlare(uv, sun);
    flare *= iIntensity;

    // 후처리
    flare -= noise(input.tex * iResolution.xy) * 0.015f;
    flare = ModifyGhostColor(flare, 0.5f, 0.1f);
    return float4(flare, 1.0f);
} // main