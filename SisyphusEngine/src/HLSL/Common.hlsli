cbuffer GlobalBuffer : register(b1)
{
    // Row 1
    float iTime;
    float iFrame;
    float2 iResolution;
    
    // Row 2
    float3 iCameraPos;
    float iFov;
    
    // Row 3
    float3 iCamForward;
    float iAspect;
    
    // Row 4
    float3 iCamRight;
    float padding1;
    
    // Row 5
    float3 iCamUp;
    float padding2;
}; // GlobalBuffer


cbuffer LightBuffer : register(b2)
{
    float3 iLightDirection;
    float iIntensity;
    
    float4 iLightColor;
    
    float2 iLightUV;
    float2 lPadding;
}; // LightBuffer


struct PixelInput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 worldPos : POSITION;
    float3 worldNormal : NORMAL;
}; // PixelInput

