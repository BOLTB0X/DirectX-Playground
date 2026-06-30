// ReflectionVS.hlsl
cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
}; // MatrixBuffer

cbuffer ClipPlaneBuffer : register(b5)
{
    float4 clipPlane;
}; // ClipPlaneBuffer

struct VS_IN
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float4 color : COLOR;
}; // VS_IN

struct PS_IN
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float4 color : COLOR;
    float clip : SV_ClipDistance0;
}; // PS_IN

PS_IN main(VS_IN input)
{
    PS_IN output;
    
    input.position.w = 1.0f;

    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    output.tex = input.tex;
    
    output.normal = mul(input.normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);
    output.tangent = mul(input.tangent, (float3x3) worldMatrix);
    output.tangent = normalize(output.tangent);
    output.binormal = mul(input.binormal, (float3x3) worldMatrix);
    output.binormal = normalize(output.binormal);

    output.color = input.color;

    output.clip = dot(mul(input.position, worldMatrix), clipPlane);

    return output;
} // main