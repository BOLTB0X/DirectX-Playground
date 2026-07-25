// ShadowVS.hlsl
cbuffer MatrixBuffer : register(b0)
{
    matrix mWorldMatrix;
    matrix mViewMatrix;
    matrix mProjectionMatrix;
    matrix mLightViewMatrix;
    matrix mLightProjectionMatrix;
}; // MatrixBuffer

cbuffer LightPositionBuffer : register(b1)
{
    float3 lLightPosition;
    float  lPadding;
}; // LightPositionBuffer

struct VS_INPUT
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
}; // VS_INPUT

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float4 lightViewPosition : TEXCOORD1;
    float3 lightPos : TEXCOORD2;
}; // PS_INPUT


PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    float4 worldPosition;

    input.position.w = 1.0f;

    output.position = mul(input.position, mWorldMatrix);
    output.position = mul(output.position, mViewMatrix);
    output.position = mul(output.position, mProjectionMatrix);
    output.lightViewPosition = mul(input.position, mWorldMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, mLightViewMatrix);
    output.lightViewPosition = mul(output.lightViewPosition, mLightProjectionMatrix);

    output.tex = input.tex;
    output.normal = mul(input.normal, (float3x3) mWorldMatrix);
    output.normal = normalize(output.normal);

    worldPosition = mul(input.position, mWorldMatrix);
    output.lightPos = lLightPosition.xyz - worldPosition.xyz;
    output.lightPos = normalize(output.lightPos);

    return output;
} // main