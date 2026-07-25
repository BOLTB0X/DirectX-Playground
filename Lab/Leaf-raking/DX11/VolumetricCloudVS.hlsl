// VolumetricCloudVS.hlsl
struct VS_INPUT
{
    float3 Pos : POSITION;
    float2 Tex : TEXCOORD0;
}; // VS_INPUT

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
    float3 ViewRay : TEXCOORD1;
}; // PS_INPUT

cbuffer CameraBuffer : register(b0)
{
    float3 cCameraPosition;
    float  cPadding;
    matrix cViewInv;
    matrix cProjInv;
    
}; // CameraBuffer

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output;
    
    // UV를 이용해 강제로 화면을 꽉 채우는 NDC 좌표 생성 (-1 ~ 1)
    float2 ndc = input.Tex * 2.0f - 1.0f;
    ndc.y = -ndc.y;
    
    // Z값을 1.0(가장 먼 곳)으로 고정하여 화면 전체를 덮는 배경을 만듬
    output.Pos = float4(ndc, 1.0f, 1.0f);
    output.Tex = input.Tex;
    
    // NDC -> View Space 광선 계산
    float4 clipPos = float4(ndc, 1.0f, 1.0f);
    float4 viewPos = mul(clipPos, cProjInv);
    viewPos /= viewPos.w;
    
    // 월드 공간에서의 픽셀 방향 벡터
    output.ViewRay = mul(float4(viewPos.xyz, 0.0f), cViewInv).xyz;
    
    return output;
} // main