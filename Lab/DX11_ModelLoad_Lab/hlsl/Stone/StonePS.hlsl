// HLSL/Stone/StonePS.hlsl

// 텍스처 슬롯 (t0 ~ t4)
Texture2D albedoTex : register(t0);
Texture2D normalTex : register(t1);
Texture2D metallicTex : register(t2);
Texture2D roughnessTex : register(t3);
Texture2D aoTex : register(t4);

SamplerState sampleState : register(s0);

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 worldPos : TEXCOORD1;
};

float4 STONE_PSMain(PS_INPUT input) : SV_TARGET
{
    float4 albedo = albedoTex.Sample(sampleState, input.texCoord);
    float3 normalMap = normalTex.Sample(sampleState, input.texCoord).rgb;
    float metallic = metallicTex.Sample(sampleState, input.texCoord).r;
    float roughness = roughnessTex.Sample(sampleState, input.texCoord).r;
    float ao = aoTex.Sample(sampleState, input.texCoord).r;

    normalMap = (normalMap * 2.0f) - 1.0f;
   
    float3x3 TBN = float3x3(normalize(input.tangent),
                            normalize(input.binormal),
                            normalize(input.normal));
    
    float3 worldNormal = normalize(mul(normalMap, TBN));

    float3 lightDir = normalize(float3(-1.0f, -1.0f, 1.0f));
    float diff = max(dot(worldNormal, -lightDir), 0.5f);

    float3 finalColor = albedo.rgb * diff * ao;
    
    return float4(finalColor, albedo.a);
} // STONE_PSMain