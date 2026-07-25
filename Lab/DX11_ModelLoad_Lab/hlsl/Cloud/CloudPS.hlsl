// hlsl/Cloud/CloudPS.hlsl
// 상수 버퍼들
cbuffer MatrixBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
}

cbuffer CloudBuffer : register(b1)
{
    float time;
    float3 padding_cloud;
}

cbuffer LightBuffer : register(b2)
{
    float3 sunDirection;
    float padding1;
    float4 sunColor;
    float4 ambientColor;
    float3 cameraPosition;
    float padding2;
    matrix inverseViewProjection;
}

// 텍스처와 샘플러
Texture2D noiseTex : register(t0); // 3D Noise 텍스처 (GLSL uNoise)
SamplerState samp : register(s0);

// Vertex Shader 출력 (Full-Screen Quad용)
struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0; // 0~1 범위 UV
};

float noise(float3 x)
{
    float3 p = floor(x);
    float3 f = frac(x);
    f = f * f * (3.0 - 2.0 * f);

    float2 uv = (p.xy + float2(37.0, 239.0) * p.z + f.xy + 0.5) / 256.0;
    return noiseTex.SampleLevel(samp, uv, 0).r * 2.0 - 1.0;
} // noise


float fbm(float3 p)
{
    float3 q = p + time * 0.5 * float3(1.0, -0.2, -1.0);
    
    float f = 0.0;
    float scale = 0.5;
    float factor = 2.02;

    for (int i = 0; i < 6; i++)
    { // GLSL과 동일한 루프 횟수
        f += scale * noise(q);
        q *= factor;
        factor += 0.21;
        scale *= 0.5;
    }
    return f;
}


float sdSphere(float3 p, float radius)
{
    return length(p) - radius;
}


float scene(float3 p)
{
    // p는 월드 공간의 점
    float distance = sdSphere(p * 0.1, 1.0); // 구름 크기 조절을 위해 p에 스케일 적용
    float f = fbm(p * 0.01 + cameraPosition * 0.001); // fbm 스케일 및 카메라 이동
    
    // 이 값을 조절하여 구름의 밀도와 분포를 결정합니다.
    return -distance + f * 5.0; // fbm의 영향력 조절
}


#define MARCH_SIZE 0.2 // 스텝 사이즈 (0.01 ~ 0.5)
#define MAX_STEPS 80   // 최대 레이마칭 스텝 수

// Ambient Occlusion
float getAO(float3 p, float3 normal)
{
    float ao = 0.0;
    float dist = 0.1;
    for (int i = 0; i < 5; i++)
    {
        ao += (dist - scene(p + normal * dist)) * (1.0 / (1.0 + dist * 10.0));
        dist *= 2.0;
    }
    return saturate(1.0 - ao * 0.2); // AO 강도 조절
}


float4 raymarch(float3 rayOrigin, float3 rayDirection)
{
    float depth = 0.0;
    float4 res = float4(0.0, 0.0, 0.0, 0.0);

    for (int i = 0; i < MAX_STEPS; i++)
    {
        float3 p = rayOrigin + depth * rayDirection;
        float density = scene(p);

        if (density > 0.0)
        {
            float3 cloudColor = float3(1.0, 1.0, 1.0); // 기본 구름 색상

            // 간단한 광원 계산
            float3 normal = normalize(float3(
                scene(p + float3(0.01, 0, 0)) - scene(p - float3(0.01, 0, 0)),
                scene(p + float3(0, 0.01, 0)) - scene(p - float3(0, 0.01, 0)),
                scene(p + float3(0, 0, 0.01)) - scene(p - float3(0, 0, 0.01))
            ));
            
            float lightDot = saturate(dot(normal, -sunDirection)); // 태양광
            cloudColor = ambientColor.rgb + sunColor.rgb * lightDot * 2.0; // 조명 영향력
            
            // AO 적용
            cloudColor *= getAO(p, normal);

            float4 color = float4(cloudColor, density * 0.1); // 밀도에 따른 투명도
            color.rgb *= color.a;
            res += color * (1.0 - res.a);

            if (res.a >= 0.95)
                break; // 불투명도가 충분하면 렌더링 중단
        }
        depth += MARCH_SIZE;
    }
    return res;
}

float4 main(PS_INPUT input) : SV_TARGET
{
    // NDC 공간의 Z=0 (Near Plane) 점을 월드 공간으로 변환
    float4 clipPos = float4(input.uv.x * 2.0 - 1.0, (1.0 - input.uv.y) * 2.0 - 1.0, 0.0, 1.0);
    float4 worldRayDir = mul(clipPos, inverseViewProjection);
    worldRayDir.xyz /= worldRayDir.w; // Perspective Divide

    // 카메라 위치 (레이 오리진)
    float3 rayOrigin = cameraPosition;
    // 월드 공간의 Ray Direction
    float3 rayDirection = normalize(worldRayDir.xyz - rayOrigin);
    
    float4 finalColor = raymarch(rayOrigin, rayDirection);
    
    // 배경색과 블렌딩 (옵션)
    float4 backgroundColor = float4(0.0, 0.0, 0.0, 1.0); 
    finalColor.rgb = lerp(backgroundColor.rgb, finalColor.rgb, finalColor.a);
    
    return finalColor;
}