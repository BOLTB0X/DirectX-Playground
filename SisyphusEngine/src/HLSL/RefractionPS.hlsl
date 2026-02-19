SamplerState SampleType : register(s0);
Texture2D shaderTexture : register(t0);

cbuffer LightBuffer : register(b2)
{
    float3 iLightDirection;
    float iIntensity;
    float4 iLightColor;
}; // LightBuffer


struct PixelInput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float clip : SV_ClipDistance0;
}; // PixelInput


float4 main(PixelInput input) : SV_TARGET
{
    float4 textureColor;
    float3 lightDir;
    float lightIntensity;
    float4 color;

    textureColor = shaderTexture.Sample(SampleType, input.tex);
    color = iLightColor * 0.2f;

    lightDir = -iLightDirection;
    lightIntensity = saturate(dot(input.normal, lightDir));

    if (lightIntensity > 0.0f)
    {
        color += (iLightColor * lightIntensity * iIntensity);
    }

    color = saturate(color) * textureColor;
    
    return color;
} // main