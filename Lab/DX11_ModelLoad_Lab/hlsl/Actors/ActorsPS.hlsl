#include "Actors.hlsli"

Texture2D AlbedoMap : register(t0);
Texture2D NormalMap : register(t1);
Texture2D MetallicMap : register(t2);
Texture2D RoughnessMap : register(t3);
Texture2D AlphaMap : register(t4);
SamplerState DefaultSampler : register(s0);

// 머티리얼 타입 정의
#define TYPE_BASE  0
#define TYPE_CLIFF 1
#define TYPE_CLOUD 2
#define TYPE_STONE 3

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 albedo = AlbedoMap.Sample(DefaultSampler, input.tex);
    float roughness = RoughnessMap.Sample(DefaultSampler, input.tex).g;
    float3 lightDir = normalize(float3(1.0f, 1.0f, -1.0f));
    float alpha = AlphaMap.Sample(DefaultSampler, input.tex).r;

    float3 N = normalize(input.normal);
    float3 T = normalize(input.tangent);
    float3 B = normalize(input.binormal);
    float3x3 TBN = float3x3(T, B, N);
    
    if (gMaterialType == TYPE_CLOUD) // 구름
    {
        float3 cloudNormalSample = NormalMap.Sample(DefaultSampler, input.tex).rgb;
        float3 localNormal = cloudNormalSample * 2.0f - 1.0f;
        float3 worldNormal = normalize(mul(localNormal, TBN));

        // 구름 조명 연산
        float diff = max(dot(worldNormal, lightDir), 0.7f);
        //  TODO 반절반사 또는 Wrap 조명 적용
        
        // 구름 색상 보간
        float3 cloudGray = lerp(float3(1.0f, 1.0f, 1.0f), float3(1.0f, 1.0f, 1.0f), roughness);
        
        // 조명과 AO를 결합한 최종 색상
        float3 finalCloudColor = cloudGray * diff;

        // 알파 마스크 처리
        float alphaMask = albedo.r;
        if (alphaMask < 0.1f)
            discard;

        return float4(finalCloudColor, alpha);
    }

    float3 normalSample = NormalMap.Sample(DefaultSampler, input.tex).rgb;
    float3 localNormal = normalSample * 2.0f - 1.0f;
    float3 worldNormal = normalize(mul(localNormal, TBN));
    
    if (gMaterialType == TYPE_STONE)
    {
        roughness *= 1.2f;
    }

    float diff = max(dot(worldNormal, lightDir), 0.2f);
    
    return float4(albedo.rgb * diff, 1.0f);
} // PS_Main