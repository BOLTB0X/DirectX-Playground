// 테스트용
cbuffer MatrixBuffer : register(b0) {
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct VertexInput {
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct PixelInput {
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PixelInput ColorVertexShader(VertexInput input) {
    PixelInput output;
    
    input.position.w = 1.0f;
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    float height = (input.position.y + 10.0f) / 50.0f;
    output.color = float4(0.0f, 1.0f, 0.5f, 1.0f);
    
    return output;
} // ColorVertexShader