#define MAX_STEPS 100
#define MARCH_SIZE 0.08
#include "Common.hlsli"
#include "Maths.hlsli"


Texture2D iNoise : register(t0); // 노이즈 텍스처
Texture2D iBlueNoise : register(t1); // 블루 노이즈
SamplerState iSampler : register(s0); // 샘플러 상태


// Texture 기반
float GetNoise(float3 x, Texture2D tex, SamplerState samp, float resolution)
{
    float3 p = floor(x);
    float3 f = frac(x);
    
    // Quintic 보간
    f = f * f * (3.0f - 2.0f * f);

    // 3D 좌표를 2D 텍스처 좌표로 투영하는 기법
    float2 uv = (p.xy + float2(37.0f, 239.0f) * p.z) + f.xy;
    
    // 인자로 받은 resolution을 사용하여 샘플링 좌표 정규화
    float2 rg = tex.SampleLevel(samp, (uv + 0.5f) / resolution, 0.0f).yx;

    // 0~1 범위를 -1~1 범위로 매핑
    return lerp(rg.x, rg.y, f.z) * 2.0f - 1.0f;
} // GetNoise


float fbm(float3 p, float time)
{
    // 시간에 따른 흐름 추가
    float3 q = p + time * 0.5f * float3(1.0f, -0.2f, -1.0f);
    
    float f = 0.0f;
    float scale = 0.5f;
    float factor = 2.02f;

    for (int i = 0; i < 6; i++)
    {
        f += scale * GetNoise(q, iNoise, iSampler, 256.0f);
        q *= factor;
        factor += 0.21f;
        scale *= 0.5f;
    }

    return f;
} // fbm



float scene(float3 p)
{
    // 1. 카메라 이동에 따라 구름 SDF도 같이 이동하게 함 (p 대신 로컬 느낌으로)
    // 혹은 특정 높이(Y)에만 구름이 존재하게 Plane SDF 스타일로 변경
    float groundHeight = 1.0f;
    float cloudThickness = 2.0f;
    
    // 구체 대신 '두께가 있는 층(Slab)' 느낌으로 변경 (무한 구름의 핵심)
    float distance = abs(p.y - groundHeight) - cloudThickness;
    
    // 2. 노이즈 샘플링 시 카메라 위치를 오프셋으로 활용 (무한 반복)
    float f = fbm(p, iTime);
    
    return -distance + f;
}

float4 rayMarch(float3 rayOrigin, float3 rayDirection, float2 uv)
{
    float4 res = float4(0, 0, 0, 0);
    
    // 블루 노이즈 디더링 (기존 코드 유지)
    float blueNoise = iBlueNoise.Sample(iSampler, uv * (iResolution / 1024.0f)).r;
    float offset = frac(blueNoise + float(iFrame % 32) / sqrt(0.5));
    
    float depth = MARCH_SIZE * offset;
    float3 sunPos = iLightPos; // 태양의 실제 위치
    float3 sunDirection = normalize(sunPos);

    for (int i = 0; i < MAX_STEPS; i++)
    {
        float3 p = rayOrigin + depth * rayDirection;
        float density = scene(p);

        if (density > 0.0f)
        {
            // --- [조명 효과 개선] ---
            float shadowDist = 0.4f;
            float densityNearSun = scene(p + shadowDist * sunDirection);
            
            // 1. 기본적인 Diffuse (은은하게 빛을 받는 면)
            float diffuse = pow(saturate((density - densityNearSun) / shadowDist), 2.0f);

            // 2. [추가] 빛의 거리에 따른 감쇄 (Inverse Square Law 느낌)
            // 태양과 현재 구름 입자(p) 사이의 거리 계산
            float distToSun = length(sunPos - p);
            float falloff = 1.0f / (1.0f + distToSun * 0.1f); // 거리에 따른 감쇄 수치

            // 3. [추가] Mie Scattering (태양을 바라볼 때 구름 경계가 밝아짐)
            float cosTheta = dot(rayDirection, sunDirection);
            float mie = 0.5f + 0.5f * pow(saturate(cosTheta), 8.0f);

            // 조명 색상 조합
            float3 cloudAmbient = float3(0.2f, 0.15f, 0.3f); // 배경 보라색 반영
            float3 lightEffect = iLightColor.rgb * iIntensity * diffuse * falloff * mie * 3.0f;
            
            float3 baseColor = lerp(float3(1.0f, 1.0f, 1.0f), float3(0.4f, 0.4f, 0.5f), density);
            float3 finalColor = baseColor * (cloudAmbient + lightEffect);

            // 알파 블렌딩
            float alpha = density * 0.4f; // 부드러운 중첩을 위해 밀도 조절
            res.rgb += (1.0f - res.a) * finalColor * alpha;
            res.a += (1.0f - res.a) * alpha;
            
            if (res.a > 0.95f)
                break;
        }

        depth += MARCH_SIZE;
        // 최적화: 일정 거리 이상 멀어지면 구름 연산 중단
        if (depth > 50.0f)
            break;
    }

    return res;
} // raymarch


float4 main(PixelInput input) : SV_TARGET
{
    float3 ro = iCameraPos;
    float3 rd = normalize(input.worldPos - iCameraPos);
    float4 res = rayMarch(ro, rd, input.tex);
    return res;
} // main