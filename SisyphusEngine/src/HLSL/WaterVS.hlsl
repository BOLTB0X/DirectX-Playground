cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
}; // MatrixBuffer

cbuffer ReflectionBuffer : register(b3)
{
    matrix reflectionMatrix;
}; // ReflectionBuffer

struct VertexInput
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
}; // VertexInput


struct PixelInput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float4 reflectionPosition : TEXCOORD1;
}; // PixelInput


PixelInput main(VertexInput input)
{
    PixelInput output;

    input.position.w = 1.0f;

    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    output.tex = input.tex;

    float4 worldPos = mul(input.position, worldMatrix);
    float4 reflectViewPos = mul(worldPos, reflectionMatrix);
    output.reflectionPosition = mul(reflectViewPos, projectionMatrix);
    return output;
} // main