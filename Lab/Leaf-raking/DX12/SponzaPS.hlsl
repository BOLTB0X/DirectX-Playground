// SponzaPS.hlsl
cbuffer DirectionalLightCB : register(b1) {
    float3   direction;
    float    padding1;

    float4   ambient;

    float4   diffuse;

    float3   lookAt;
    float    padding2;

    float4x4 lightViewMatrix;
    float4x4 lightProjectionMatrix;

    float    shadowMapWidth;
    float    shadowMapHeight;
    float    shadowBias;
    float    shadowSpread;
    float4   padding3;
};

struct PSInput {
    float4 positionCS : SV_POSITION;
    float3 positionWS : POSITION_WS;
    float3 normalWS   : NORMAL_WS;
    float3 tangentWS  : TANGENT_WS;
    float3 binormalWS : BINORMAL_WS;
    float2 texcoord   : TEXCOORD;
};

static const float3 FLAT_ALBEDO = float3(0.7f, 0.7f, 0.7f);

float4 main(PSInput input) : SV_TARGET {
    float3 N = normalize(input.normalWS);
    float3 L = normalize(-direction);

    float  NdotL = saturate(dot(N, L));
    float3 diffuseTerm = diffuse.rgb * NdotL;
    float3 ambientTerm = ambient.rgb;

    float3 color = FLAT_ALBEDO * (ambientTerm + diffuseTerm);
    return float4(color, 1.0f);
} // main
