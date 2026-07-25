// hlsl/Terrain/TerrainVS.hlsl
cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer MaterialBuffer : register(b2)
{
    int type;
    float gTime;
    float4 gMountPos;
    float4 gStonePos;
    float3 padding;
};

struct VS_INPUT
{
    float3 position : POSITION;
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

// ------

float frac_func(float x)
{
    return x - floor(x);
} // frac_func

float hash(float2 p)
{
    float dotProduct = p.x * 12.9898f + p.y * 78.233f;
    return frac_func(sin(dotProduct) * 43758.5453f);
} // hash

float noise(float2 p)
{
    float2 i = floor(p);
    float2 f = p - i;
    float2 u = f * f * (3.0f - 2.0f * f);

    return lerp(lerp(hash(i), hash(i + float2(1, 0)), u.x),
                lerp(hash(i + float2(0, 1)), hash(i + float2(1, 1)), u.x), u.y);
} // noise

float getFBMHeight(float x, float z, float time)
{
    float2 p = float2(x, z);
    float height = 0.0f;
    float amplitude = 0.5f;
    float frequency = 0.05f;
    
    float time_scroll = time * 0.2f;

    for (int i = 0; i < 6; i++)
    {
        height += noise(p * frequency + time_scroll) * amplitude;
        
        amplitude *= 0.45f;
        frequency *= 2.0f;
    }

    return height * 15.0f;
}

// -----------------------------------------------

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    
    float displacedY = getFBMHeight(input.position.x, input.position.z, gTime);
    float3 finalPos = float3(input.position.x, displacedY, input.position.z);
    float4 worldPos = mul(float4(finalPos, 1.0f), worldMatrix);

    output.worldPos = worldPos.xyz;
    output.position = mul(worldPos, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    output.texCoord = input.texCoord;

    output.normal = normalize(mul(input.normal, (float3x3) worldMatrix));
    output.tangent = normalize(mul(input.tangent, (float3x3) worldMatrix));
    output.binormal = normalize(mul(input.binormal, (float3x3) worldMatrix));
    return output;
} // main