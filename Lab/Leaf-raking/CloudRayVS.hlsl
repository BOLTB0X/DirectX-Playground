// CloudRayVS.hlsl
cbuffer CameraBuffer : register(b0)
{
    float3 cCameraPosition;
    float  cPadding;
    matrix cView;
    matrix cProjection;
}; // CameraBuffer

struct VS_INPUT
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD0;
}; // VS_INPUT

struct PS_INPUT
{
    float4 suvPos : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    float3 worldPos : POSITION;
}; // PS_INPUT

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    float3 worldPos = input.position + cCameraPosition.xyz;
    output.worldPos = worldPos;
    
    float4 viewPos = mul(float4(worldPos, 1.0f), cView);
    output.suvPos = mul(viewPos, cProjection);
    output.texCoord = input.texCoord;
    return output;
} // main