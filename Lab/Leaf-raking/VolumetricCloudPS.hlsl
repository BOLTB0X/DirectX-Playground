// VolumetricCloudPS.hlsl
#include "FBM.hlsl"
#include "Remap.hlsl"

cbuffer CameraBuffer : register(b0)
{
    float3 cCameraPosition;
    float cPadding1;
    matrix cViewInv;
    matrix cProjInv;
}; // CameraBuffer

cbuffer LightBuffer : register(b1)
{
    float4 cLightColor;
    float3 cLightDir;
    float cLightIntensity;
}; // LightBuffer

cbuffer CloudBuffer : register(b2)
{
    float3 cPlanetCenter;
    float cPlanetRadius;
    float cCloudMinHeight;
    float cCloudMaxHeight;
    float cTime;
    float cPadding;
}; // CloudBuffer



Texture3D<float4> VolumeTexture : register(t0);
SamplerState WrapSampler : register(s0);

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD0;
    float3 ViewRay : TEXCOORD1; // VS에서 넘어온 월드 공간 광선 방향
}; // PS_INPUT

//float2 ray_sphere_intersection(float3 r0, float3 rd, float sr)
//{
//    float a = dot(rd, rd);
//    float b = 2.0 * dot(rd, r0);
//    float c = dot(r0, r0) - (sr * sr);
//    float d = (b * b) - 4.0 * a * c;
//    if (d < 0.0)
//        return float2(1e5, -1e5);
//    return float2((-b - sqrt(d)) / (2.0 * a), (-b + sqrt(d)) / (2.0 * a));
//} // ray_sphere_intersection

float2 ray_sphere_intersect(float3 start, float3 dir, float radius)
{
    float a = dot(dir, dir);
    float b = 2.0 * dot(dir, start);
    float c = dot(start, start) - (radius * radius);
    float d = (b * b) - 4.0 * a * c;
    
    if (d < 0.0)
        return float2(1e5, -1e5);
    
    return float2(
        (-b - sqrt(d)) / (2.0 * a),
        (-b + sqrt(d)) / (2.0 * a)
    );
} // ray_sphere_intersect


// --- 이식된 유틸리티 ---

// Valentin의 핵심 조명 함수 단순화
float illuminate_volume(float height)
{
    return exp(height) / 1.95;
}

// 텍스처 기반 밀도 함수 (snoise 제거)
float get_density(float3 pos, float h)
{
    float3 uvw = pos * 0.0001 + (float3(0, 0, -1) * cTime * 0.02);
    
    // [최적화] 루프 안에서 가장 가벼운 연산: 텍스처 샘플링
    float4 noise = VolumeTexture.SampleLevel(WrapSampler, uvw, 0);
    
    float baseShape = noise.r;
    
    // GBA 채널을 조합하여 디테일 노이즈 생성 (침식용)
    float detailErosion = (noise.g * 0.625) + (noise.b * 0.25) + (noise.a * 0.125);
    
    // Valentin Galea 스타일의 Coverage 적용
    // baseShape에서 detailErosion을 살짝 빼주어 외곽을 깎아냅니다 (Erosion)
    float dens = remap(baseShape, detailErosion * 0.2, 1.0, 0.0, 1.0);
    
    // Coverage 및 높이 마스크 적용
    dens = RemapClamp(dens, 0.3125f, 1.0, 0.0, 1.0);
    float heightMask = smoothstep(0.0, 0.15, h) * smoothstep(1.0, 0.8, h);
    
    return dens * heightMask;
} // get_density

#define PI 3.14159265359

// Henyey-Greenstein 페이즈 함수: 빛이 진행 방향으로 얼마나 강하게 산란되는가
float henyey_greenstein(float mu, float g)
{
    float g2 = g * g;
    return (1.0 - g2) / (4.0 * PI * pow(1.0 + g2 - 2.0 * g * mu, 1.5));
}

// 조명 계산: 높이와 태양 각도에 따른 산란광
float illuminate_volume(float h, float cosTheta)
{
    // 1. 기본 높이 기반 밝기 (Overcast 특유의 밑바닥은 어둡고 위는 밝은 느낌)
    float baseLight = exp(h) / 1.95;
    
    // 2. 태양 방향 산란 (Forward Scattering)
    // g값이 0.76 정도면 태양 근처 구름이 아주 밝게 빛납니다.
    float phase = henyey_greenstein(cosTheta, 0.76);
    
    return baseLight * phase;
}

// --- 메인 레이마칭 ---

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 aZenithColor = { 0.0f, 0.2f, 0.6f, 1.0f };
    float4 aHorizonColor = { 0.81f, 0.38f, 0.66f, 1.0f };
    
    float3 ro = cCameraPosition;
    float3 rd = normalize(input.ViewRay);
    float3 L = normalize(-cLightDir); // 태양 방향
    float cosTheta = dot(rd, L); // 시선과 태양광의 각도

    float cutoff = rd.y;
    if (cutoff < 0.01)
        discard;

   // 3. 레이마칭 구간 계산 (구체 교차 활용)
    // 행성 중심 기준 상대 좌표
    float3 relativeRo = ro - cPlanetCenter;
    
    // 구름의 하단(Min)과 상단(Max) 구체와의 교차점 탐색
    float2 innerHit = ray_sphere_intersect(relativeRo, rd, cPlanetRadius + cCloudMinHeight);
    float2 outerHit = ray_sphere_intersect(relativeRo, rd, cPlanetRadius + cCloudMaxHeight);

    // 실제 레이마칭을 수행할 시작/끝 거리 결정
    // 카메라가 구름 아래에 있다면 innerHit.y(하단 뚫고 나가는 점)부터 시작
    float startDist = max(0.0, innerHit.y);
    float endDist = outerHit.y;

    // 레이가 구름층을 아예 지나치지 않는 경우 처리
    if (endDist <= 0 || startDist >= endDist)
    {
        float factor = pow(saturate(rd.y), 0.5);
        return lerp(aHorizonColor, aZenithColor, factor);
    }

    // 4. 레이마칭 파라미터 설정
    const int steps = 48; // 프레임에 따라 32~64 조절
    float stepSize = (endDist - startDist) / float(steps);
    float3 currentPos = ro + rd * startDist;
    
    float transmittance = 1.0;
    float3 accumulatedColor = 0;
    float alpha = 0;

    [loop]
    for (int i = 0; i < steps; i++)
    {
        // 현재 위치의 고도 비율 (0.0 ~ 1.0)
        float currentHeight = length(currentPos - cPlanetCenter) - cPlanetRadius;
        float h = saturate((currentHeight - cCloudMinHeight) / (cCloudMaxHeight - cCloudMinHeight));
        
        float density = get_density(currentPos, h);

        if (density > 0.01)
        {
            // [Light Marching] 태양 방향 산란광 계산
            // 구름의 '입체감'과 '검은 부분 해결'의 핵심
            float shadowDensity = 0;
            float3 lightPos = currentPos;
            for (int j = 0; j < 4; j++) // 4단계만 체크해도 충분
            {
                lightPos += L * (stepSize * 1.5);
                float lh = saturate((length(lightPos - cPlanetCenter) - cPlanetRadius - cCloudMinHeight) / (cCloudMaxHeight - cCloudMinHeight));
                shadowDensity += get_density(lightPos, lh);
            }
            float lightTrans = exp(-shadowDensity * 0.7); // 태양빛 도달율

            // 5. 조명 통합 (Henyey-Greenstein 페이즈 적용)
            float phase = henyey_greenstein(cosTheta, 0.76);
            float3 lightInput = cLightColor.rgb * cLightIntensity * lightTrans * phase;
            
            // 투과율 법칙(Beer-Lambert) 적용
            float T_i = exp(-1.0 * density * stepSize);
            accumulatedColor += transmittance * lightInput * density * stepSize;
            
            transmittance *= T_i;
            alpha += (1.0 - T_i) * (1.0 - alpha);

            if (alpha > 0.99)
                break;
        }
        currentPos += rd * stepSize;
    }

    // 6. 최종 배경 합성 (하늘색 그라데이션)
    float skyFactor = pow(saturate(rd.y), 0.5);
    float3 skyColor = lerp(aHorizonColor.rgb, aZenithColor.rgb, skyFactor);
    
    // 구름 색상 + (하늘색 * 남은 투과율)
    float3 finalColor = accumulatedColor + (skyColor * transmittance);

    return float4(finalColor, alpha);
    //return float4(1.0f, 0.0f, 1.0f, 1.0f);
}