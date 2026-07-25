// hlsl/Cloud/CloudGS.hlsl

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct VS_OUTPUT
{
    float3 position : POSITION;
    float size : SIZE;
};

struct GS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 worldPos : TEXCOORD1; // PS에서 높이 기반 효과를 위해 월드 좌표 전달
};


[maxvertexcount(4)]
void main(point VS_OUTPUT input[1], inout TriangleStream<GS_OUTPUT> outStream)
{
    float3 center = input[0].position;
    float halfSize = input[0].size * 0.5f;

    // ViewMatrix에서 카메라의 Right(X), Up(Y) 축 추출
    float3 right = float3(viewMatrix._11, viewMatrix._21, viewMatrix._31);
    float3 up = float3(viewMatrix._12, viewMatrix._22, viewMatrix._32);

    // 사각형의 네 모서리 계산
    float3 vertices[4];
    vertices[0] = center + (right * -halfSize) + (up * -halfSize); // 좌하
    vertices[1] = center + (right * -halfSize) + (up * halfSize); // 좌상
    vertices[2] = center + (right * halfSize) + (up * -halfSize); // 우하
    vertices[3] = center + (right * halfSize) + (up * halfSize); // 우상

    float2 uvs[4] = { float2(0, 1), float2(0, 0), float2(1, 1), float2(1, 0) };

    GS_OUTPUT output;
    for (int i = 0; i < 4; i++)
    {
        // 월드 좌표 저장
        output.worldPos = vertices[i];
        
        // 투영 변환: World -> View -> Projection
        float4 viewPos = mul(float4(vertices[i], 1.0f), viewMatrix);
        output.position = mul(viewPos, projectionMatrix);
        
        output.uv = uvs[i];
        
        outStream.Append(output);
    }
} // main