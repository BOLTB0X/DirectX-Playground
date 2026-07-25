// hlsl/Actors/Actors.hlsli
// 공통 구조체

struct VS_INPUT
{
    float3 pos : POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 worldPos : POSITION;
};

cbuffer MaterialBuffer : register(b2)
{
    int gMaterialType; // 0:Base, 1:Cliff, 2:Cloud, 3:Stone
    float3 gPadding;
};