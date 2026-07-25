cbuffer CameraBuffer : register(b0)
{
    float3 cameraPos; // World Space 카메라 위치
    float  padding1;
    float4x4 invViewProj; // ViewProjection 역행렬
};

struct PSInput
{
    float4 pos : SV_POSITION;// Screen Space
    float2 uv  : TEXCOORD0; // 0~1
};

// -------------------------------
// SDF : Sphere
// -------------------------------
float SDF_Sphere(float3 p, float radius)
{
    return length(p) - radius;
}

// -------------------------------
// Scene SDF
// -------------------------------
float SceneSDF(float3 p)
{
    return SDF_Sphere(p, 1.0f);
}

// -------------------------------
// Ray Marching
// -------------------------------
bool RayMarch(float3 rayOrigin, float3 rayDir, out float3 hitPos)
{
    float totalDist = 0.0f;

    for (int i = 0; i < 64; i++)
    {
        float3 p = rayOrigin + rayDir * totalDist;
        float d = SceneSDF(p);

        if (d < 0.001f)
        {
            hitPos = p;
            return true; // Hit
        }

        totalDist += d;

        if (totalDist > 100.0f)
            break;
    }

    return false;
}

// -------------------------------
// Pixel Shader
// -------------------------------
float4 PS_Main(PSInput input) : SV_TARGET
{
    // UV (0~1) → NDC (-1~1)
    float2 ndc;
    ndc.x = input.uv.x * 2.0f - 1.0f;
    ndc.y = 1.0f - input.uv.y * 2.0f;

    // NDC -> World Space Position
    float4 clip = float4(ndc, 0.0f, 1.0f);
    float4 world = mul(clip, invViewProj);
    world.xyz /= world.w;

    // Ray 정의
    float3 rayOrigin = cameraPos;
    float3 rayDir = normalize(world.xyz - cameraPos);

    // Ray Marching
    float3 hitPos;
    if (RayMarch(rayOrigin, rayDir, hitPos))
    {
        return float4(1, 1, 1, 1);
    }

    return float4(0, 0, 0, 1);
}
