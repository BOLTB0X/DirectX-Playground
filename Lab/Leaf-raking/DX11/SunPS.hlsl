// https://www.shadertoy.com/view/4dl3zr
#include "Common.hlsli"


float4 main(PixelInput input) : SV_TARGET
{
    // 1. 좌표 및 방향 벡터 준비
    float2 uv = input.tex * 2.0f - 1.0f; // -1 ~ 1 범위
    float dist = length(uv);
    float3 normal = normalize(input.worldNormal);
    float3 viewDir = normalize(iCameraPos - input.worldPos);

    // 2. 시간대별 태양 색상 및 강도 제어 (셰이더토이 로직 응용)
    // iTime을 이용해 낮/밤의 전이를 부드럽게 시뮬레이션 합니다.
    float midday = saturate(sin(iTime * 0.1f) * 0.5f + 0.5f);
    float3 sunColor = lerp(float3(1.5f, 0.6f, 0.2f), float3(1.0f, 0.95f, 0.8f), midday);
    float3 finalBaseColor = sunColor * iIntensity;

    // 3. 태양 핵 (Sun Core)
    // 매우 날카롭고 밝은 중심부
    float core = smoothstep(0.45f, 0.02f, dist);

    // 4. 가우시안 블룸 및 글로우 (Bloom & Glow)
    // 화면 전체로 부드럽게 퍼지는 빛의 산란
    float bloom = exp(-dist * dist * 3.5f) * 2.2f;
    float wideGlow = exp(-dist * 1.2f) * 0.8f;

    // 5. [핵심] Simplex Noise 기반 역동적 광선 (Dynamic RayGlow)
    // atan2로 각도를 구하여 방사형 노이즈를 생성합니다.
    float angle = atan2(uv.y, uv.x);
    
    // 두 개의 노이즈 층을 서로 다른 속도로 섞어 겹침 효과(fBM)를 줍니다.
    float n1 = snoise(float2(angle * 2.5f, iTime * 0.4f));
    float n2 = snoise(float2(angle * 5.0f - iTime * 0.8f, iTime * 0.2f));
    
    // 노이즈를 0~1 범위로 가공하고 pow를 써서 빛줄기를 날카롭게 만듭니다.
    float dynamicRays = pow(saturate((n1 + n2 * 0.5f) + 0.4f), 5.0f);
    // 태양 본체 근처에서만 강하게 나타나도록 거리 마스킹
    float rayGlow = dynamicRays * saturate(1.1f - dist) * 1.8f;

    // 6. 프레넬 림 라이트 (Rim Light)
    // 구체 모델의 가장자리를 따라 에너지가 분출되는 입체 효과
    float rim = pow(1.0f - saturate(dot(normal, viewDir)), 3.5f) * 2.5f;

    // 7. 최종 결과 조합
    // 각 레이어를 합산하여 강렬한 HDR 느낌을 구현합니다.
    float3 combinedColor = finalBaseColor * (core * 2.0f + bloom + wideGlow + rayGlow + rim);

    // 8. 알파 채널 (투명도) 제어
    // 메쉬의 사각형 경계선이 보이지 않도록 외곽으로 갈수록 부드럽게 감쇄시킵니다.
    float alpha = saturate(core + bloom * 0.5f + wideGlow * 0.4f + rayGlow * 0.3f + rim * 0.5f);

    return float4(combinedColor, alpha);
} // main