// hlsl/Actors/ActorsVS.hlsl
#include "Actors.hlsli"

cbuffer TransformBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    
    float4 worldPos = mul(float4(input.pos, 1.0f), world);
    output.worldPos = worldPos.xyz;
    output.pos = mul(worldPos, view);
    output.pos = mul(output.pos, projection);
    
    output.tex = input.tex;
    output.normal = mul(input.normal, (float3x3) world);
    output.tangent = mul(input.tangent, (float3x3) world);
    output.binormal = mul(input.binormal, (float3x3) world);
    
    return output;
} // VS_Main