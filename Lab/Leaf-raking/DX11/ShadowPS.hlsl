// ShadowPS.hlsl
Texture2D depthMapTexture : register(t0);
SamplerState SampleTypeClamp : register(s0);
SamplerState SampleTypeWrap : register(s1);

cbuffer LightBuffer : register(b0)
{
    float4 lAmbientColor;
    float4 lDiffuseColor;
    float  lBias;
    float3 lPadding;
}; // LightBuffer

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float4 lightViewPosition : TEXCOORD1;
    float3 lightPos : TEXCOORD2;
}; // PS_INPUT

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 color;
    float2 projectTexCoord;
    float depthValue;
    float lightDepthValue;
    float lightIntensity;
    float4 textureColor;
	
    color = lAmbientColor;

    projectTexCoord.x = input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
    projectTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f;
    
    if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
        depthValue = depthMapTexture.Sample(SampleTypeClamp, projectTexCoord).r;
        lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w;
        lightDepthValue = lightDepthValue - lBias;

        if (lightDepthValue < depthValue)
        {
            lightIntensity = saturate(dot(input.normal, input.lightPos));

            if (lightIntensity > 0.0f)
            {
                color += (lDiffuseColor * lightIntensity);
                color = saturate(color);
            }
        }
    }


    return color;
} // main
