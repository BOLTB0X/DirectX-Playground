// hlsl/Cloud/CloudVS.hlsl
cbuffer MatrixBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
};

struct VS_INPUT
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    // 1. 월드/뷰/투영 변환 적용
    output.pos = mul(float4(input.position, 1.0f), world);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);
    
    // 2. UV 전달 (레이마칭의 기준점)
    output.uv = input.texCoord;
    
    return output;
} // main