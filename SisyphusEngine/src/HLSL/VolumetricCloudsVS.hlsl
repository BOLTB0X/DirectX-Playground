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
}; // PixelInput



PixelInput main(VertexInput input)
{
    PixelInput output;
    
    output.position = float4(input.position.xyz, 1.0f);
    output.tex = input.tex;
    return output;
}