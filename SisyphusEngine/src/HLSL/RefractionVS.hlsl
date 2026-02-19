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
    float3 normal : NORMAL;
    float clip : SV_ClipDistance0; // 이 값이 음수면 픽셀이 버려짐
}; // PixelInput


PixelInput main(VertexInput input)
{
    PixelInput output;

    input.position.w = 1.0f;

    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    output.tex = input.tex;
    output.normal = mul(input.normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);

    // Clipping 로직
    // 월드 공간의 정점 위치와 클립 평면을 내적하여 거리를 계산
    output.clip = dot(mul(input.position, worldMatrix), clipPlane);

    return output;
} // main