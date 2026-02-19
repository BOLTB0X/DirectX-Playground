SamplerState SampleType : register(s0);
Texture2D reflectionTexture : register(t0);
Texture2D normalTexture : register(t1);


cbuffer WaterBuffer : register(b4)
{
    float3 iWaterBaseColor;
    float wPadding1;
    
    float iWaterTranslation;
    float iReflectRefractScale;
    float2 wPadding2;
    
    float iFinalAlpha;
    float3 wPadding3;
}; // WaterBuffer


struct PixelInput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float4 reflectionPosition : TEXCOORD1;
}; // PixelInput


float4 main(PixelInput input) : SV_TARGET
{
    float2 reflectTexCoord;
    float4 normalMap;
    float3 normal;
    float4 reflectionColor;

    // 노멀맵 UV 애니메이션
    float2 movingUV = input.tex;
    movingUV.y += iWaterTranslation;

    // 투영 텍스처 좌표 계산
    reflectTexCoord.x = input.reflectionPosition.x / input.reflectionPosition.w / 2.0f + 0.5f;
    reflectTexCoord.y = -input.reflectionPosition.y / input.reflectionPosition.w / 2.0f + 0.5f;

    // 노멀맵 샘플링 및 왜곡 적용
    normalMap = normalTexture.Sample(SampleType, movingUV);
    normal = (normalMap.xyz * 2.0f) - 1.0f; // [0,1] -> [-1,1]

    // 노멀의 XY 값을 왜곡 강도(scale)와 곱해 반사 UV를 비틂
    reflectTexCoord += (normal.xy * iReflectRefractScale);
    // 왜곡된 좌표로 반사 텍스처 샘플링
    reflectionColor = reflectionTexture.Sample(SampleType, reflectTexCoord);

    float4 waterBaseColor = float4(iWaterBaseColor, 1.0f);

    return lerp(waterBaseColor, reflectionColor, iFinalAlpha);
} // main