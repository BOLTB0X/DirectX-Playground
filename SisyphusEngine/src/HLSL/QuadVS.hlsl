cbuffer MatrixBuffer : register(b0)
{
    matrix modelMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix iInvViewProj;
}; // MatrixBuffer


struct QuadInput
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
}; // VertexInput


struct PixelInput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
}; // PixelInput


PixelInput main(QuadInput input)
{
    PixelInput output;
    
    output.position = float4(input.position.xy, 0.0f, 1.0f);
    output.tex = input.tex;
    
    return output;
} // main