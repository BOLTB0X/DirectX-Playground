// HLSL/Stone/StoneVS.hlsl

// 상수 버퍼 (b0)
cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct VS_INPUT
{
    float4 position : POSITION;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 worldPos : TEXCOORD1;
};

PS_INPUT STONE_VSMain(VS_INPUT input)
{
    PS_INPUT output;

    input.position.w = 1.0f;
    output.position = mul(input.position, worldMatrix);
    output.worldPos = output.position.xyz;
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    output.texCoord = input.texCoord;
    
    float3 N = normalize(mul(input.normal, (float3x3) worldMatrix));
    float3 T = normalize(mul(input.tangent, (float3x3) worldMatrix));
    T = normalize(T - dot(T, N) * N);

    output.normal = N;
    output.tangent = T;
    
    float3 B_calculated = cross(N, T);
    float3 B_original = normalize(mul(input.binormal, (float3x3) worldMatrix));
        
    if (dot(B_calculated, B_original) < 0.0f)
    {
        B_calculated = -B_calculated;
    }
    output.binormal = B_calculated;

    return output;
} // STONE_VSMain