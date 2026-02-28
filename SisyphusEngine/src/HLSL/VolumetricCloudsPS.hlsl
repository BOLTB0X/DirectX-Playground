#include "Maths.hlsli"


Texture2D iNoise : register(t0); // 노이즈 텍스처
Texture2D iBlueNoise : register(t1); // 블루 노이즈
Texture2D iDepthTexture : register(t2);
SamplerState iSampler : register(s0); // 샘플러 상태


cbuffer MatrixBuffer : register(b0)
{
    matrix iWorld;
    matrix iView;
    matrix iProj;
    matrix iInvViewProj;
}; // MatrixBuffer


cbuffer GlobalBuffer : register(b1)
{
    // Row 1
    float iTime;
    float iFrame;
    float2 iResolution;
    
    // Row 2
    float3 iCameraPos;
    float iFov;
    
    // Row 3
    float3 iCamForward;
    float iAspect;
    
    // Row 4
    float3 iCamRight;
    float padding1;
    
    // Row 5
    float3 iCamUp;
    float padding2;
}; // GlobalBuffer


cbuffer LightBuffer : register(b2)
{
    float3 iLightDirection;
    float iIntensity;
    
    float4 iLightColor;
    
    float2 iLightUV;
    float2 lPadding;
}; // LightBuffer


cbuffer VolumetricCloudsBuffer : register(b3)
{
    // Row 1
    float3 iPlanetCenter;
    float iEarthRadius;
    // Row 2
    float iCloudMinHeight;
    float iCloudMaxHeight;
    float iCloudFadeDist;
    float iCloudSpeed;
    // Row 3
    float iCloudDensity;
    float iCloudCoverage;
    float iCloudAbsorption;
    float vPadding1;
    // Row 4
    int iCloudSteps;
    int iLightSteps;
    float iShadowIntensity;
    float iNoiseScale;
    // Row 5
    float iFogDensity;
    float3 vPadding2;
}; // PBRCloudBuffer


struct PixelInput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
}; // PixelInput


float getLinearDepth(float2 uv)
{
    float d = iDepthTexture.Sample(iSampler, uv).r;
    float n = 0.1f;
    float f = 1000.0f;
    return (n * f) / (f - d * (f - n));
} // GetLinearDepth


float3 getRayDir(float2 uv)
{
    float2 ndc = uv * 2.0f - 1.0f;
    float4 clip = float4(ndc.x, -ndc.y, 1.0f, 1.0f);
    float4 world = mul(clip, iInvViewProj);
    world /= world.w;
    return normalize(world.xyz - iCameraPos);
} // getRayDir


float2 intersectLayer(float3 ro, float3 rd)
{
    if (abs(rd.y) < 1e-5)
        return float2(-1.0, -1.0);
    
    float t0 = (iCloudMinHeight - ro.y) / rd.y;
    float t1 = (iCloudMaxHeight - ro.y) / rd.y;

    if (t0 > t1)
    {
        float tmp = t0;
        t0 = t1;
        t1 = tmp;
    }
    return float2(t0, t1);
} // intersectLayer


float2 intersectAtmosphere(float3 ro, float3 rd, float r)
{
    float3 oc = ro - iPlanetCenter;
    float b = dot(oc, rd);
    float c = dot(oc, oc) - r * r;
    float h = b * b - c;
    if (h < 0.0)
        return float2(-1.0, -1.0);
    h = sqrt(h);
    return float2(-b - h, -b + h);
} // intersectAtmosphere


float2 GetAtmosphereT(float3 ro, float3 rd)
{
    float r = iEarthRadius + iCloudMinHeight;
    float R = iEarthRadius + iCloudMaxHeight;
    
    float2 tInner = intersectAtmosphere(ro, rd, r);
    float2 tOuter = intersectAtmosphere(ro, rd, R);
    
    // 카메라가 지표면에 있을 때: tOuter.x (앞) ~ tOuter.y (뒤)
    // 하지만 우리는 Min과 Max 사이의 '껍질'만 계산해야 함
    float start = 0;
    float end = 0;

    float distToCenter = length(ro - iPlanetCenter);

    if (distToCenter < r)
    { // 1. 구름층 아래에 있을 때
        start = tInner.y;
        end = tOuter.y;
    }
    else if (distToCenter > R)
    { // 2. 구름층 위에 있을 때 (우주/높은 고도)
        start = tOuter.x;
        end = tInner.x;
    }
    else
    { // 3. 구름층 사이에 있을 때 (비행 중)
        start = 0;
        end = tOuter.y; // 또는 tInner.y 중 가까운 쪽
    }
    
    return float2(max(0, start), max(0, end));
}


//float GetDensity(float3 p)
//{
//    // 수직 분포 곡선 수정
//    float heightFraction = saturate((p.y - iCloudMinHeight) / (iCloudMaxHeight - iCloudMinHeight));
    
//    float verticalMask = pow(1.0 - heightFraction, 0.5);
    
//    // 2. 노이즈 스케일
//    float2 uv = p.xz * (iNoiseScale * 0.1);
//    uv += iTime * iCloudSpeed * 0.05;

//    float n = iNoise.SampleLevel(iSampler, uv, 0).r;
    
//    // [추가] 노이즈의 대비를 높여 덩어리감을 더 줍니다.
//    n = smoothstep(0.2, 0.8, n);

//    float density = saturate(n - (1.0 - iCloudCoverage)) * verticalMask;
    
//    return density * iCloudDensity;
//}


float GetDensity(float3 p)
{
    // [원근법 수정] 평면 p.y 대신 지구 중심으로부터의 거리를 사용합니다.
    float distFromCenter = length(p - iPlanetCenter);
    float height = distFromCenter - iEarthRadius; // 지표면으로부터의 높이
    
    float heightFraction = saturate((height - iCloudMinHeight) / (iCloudMaxHeight - iCloudMinHeight));
    
    // 구름의 상하단 마스크 (뭉게구름처럼 보이게 위아래를 깎음)
    float verticalMask = smoothstep(0.0, 0.1, heightFraction) * smoothstep(1.0, 0.3, heightFraction);
    
    // 노이즈 샘플링 (iNoiseScale 적용)
    // 원근감을 위해 p.xz를 사용하되 높이에 따른 약간의 왜곡을 줍니다.
    float2 uv = p.xz * iNoiseScale;
    uv += iTime * iCloudSpeed;

    float n = iNoise.SampleLevel(iSampler, uv, 0).r;
    n = smoothstep(0.2, 0.8, n);

    float density = saturate(n - (1.0 - iCloudCoverage)) * verticalMask;
    
    return density * iCloudDensity;
}


float4 main(PixelInput input) : SV_TARGET
{
    float3 ro = iCameraPos;
    float3 rd = getRayDir(input.tex);
    float2 tRange = GetAtmosphereT(ro, rd);

    if (tRange.y < 0.0)
        return float4(0, 0, 0, 0);

    float startDepth = max(tRange.x, 0.0);
    float endDepth = tRange.y;
    float sceneLinearDepth = getLinearDepth(input.tex);
    
    endDepth = min(endDepth, sceneLinearDepth);

    if (endDepth > startDepth)
    {
        float stepSize = (endDepth - startDepth) / (float) iCloudSteps;
        float dither = iBlueNoise.SampleLevel(iSampler, input.tex * (iResolution / 1024.0), 0).r;
        float currentT = startDepth + dither * stepSize;

        float transmittance = 1.0;
        float3 scatteredLight = float3(0, 0, 0);

        for (int i = 0; i < iCloudSteps; i++)
        {
            if (transmittance < 0.05)
                break;

            float3 p = ro + rd * currentT;
            float density = GetDensity(p);

            if (density > 0.001)
            {
                float3 ambientColor = float3(0.4, 0.45, 0.5);
                float3 sunColor = iLightColor.rgb * iIntensity;
    
                float h = saturate((p.y - iCloudMinHeight) / (iCloudMaxHeight - iCloudMinHeight));
                float3 lighting = lerp(ambientColor, sunColor, h);

                // Beer's Law (비어의 법칙)
                float extinction = density * 2.0;
                float stepTransmittance = exp(-extinction * stepSize);

                // 내부에서 빛이 퍼지는 양
                float distanceFade = 1.0 - saturate(currentT / iCloudFadeDist);
                //float3 stepScattering = lighting * density * stepSize * 2.5;
                float3 stepScattering = lighting * density * stepSize * 2.5 * distanceFade;
                scatteredLight += transmittance * stepScattering;
    
                transmittance *= stepTransmittance;
            }
            currentT += stepSize;
        }

        return float4(scatteredLight, (1.0 - transmittance) * 0.8);
    }

    return float4(0, 0, 0, 0);
}