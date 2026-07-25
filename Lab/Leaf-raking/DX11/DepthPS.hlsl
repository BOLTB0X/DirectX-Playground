// DepthPS.hlsl
struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 depthPosition : TEXTURE0;
}; // PS_INPUT

float4 main(PS_INPUT input) : SV_TARGET
{
    float depthValue;
    float4 color;
	
    depthValue = input.depthPosition.z / input.depthPosition.w;

    color = float4(depthValue, depthValue, depthValue, 1.0f);

    return color;
} // main