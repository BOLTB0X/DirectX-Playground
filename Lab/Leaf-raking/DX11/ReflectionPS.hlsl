// ReflectionPS.hlsl
Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D waveNormalTexture : register(t1);

SamplerState SampleType;

cbuffer ReflectionLightBuffer : register(b5)
{
    float4 lightDiffuseColor;
    float  rPadding;
    float3 lightDirection;
    float  colorTextureBrightness;
}; // LightBuffer

struct PS_IN
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float4 color : COLOR;
    float  clip : SV_ClipDistance0;
}; // PS_IN


float4 main(PS_IN input) : SV_TARGET
{
    float3 lightDir;
    float4 textureColor;
    float4 bumpMap;
    float3 bumpNormal;
    float lightIntensity;
    float4 color;

    lightDir = -lightDirection;

    textureColor = colorTexture.Sample(SampleType, input.tex);

    textureColor = saturate(input.color * textureColor * colorTextureBrightness);

    bumpMap = normalTexture.Sample(SampleType, input.tex);
    bumpMap = (bumpMap * 2.0f) - 1.0f;
    bumpNormal = input.normal + bumpMap.x * input.tangent + bumpMap.y * input.binormal;
    bumpNormal = normalize(bumpNormal);
    lightIntensity = saturate(dot(bumpNormal, lightDir));
    
    color = saturate(lightDiffuseColor * lightIntensity);
    color = color * textureColor;

    return color;
}