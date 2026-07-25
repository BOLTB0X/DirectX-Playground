// VolumetricCloudCS.hlsl
// https://www.shadertoy.com/view/XslGRr
// https://www.shadertoy.com/view/Xttcz2
// https://www.shadertoy.com/view/MstBWs
// https://github.com/chihirobelmo/volumetric-cloud-for-directx11/blob/main/VolumetricCloud/shaders/RayMarch.hlsl
// https://momentsingraphics.de/BlueNoise.html
// https://wallisc.github.io/rendering/2020/05/02/Volumetric-Rendering-Part-1.html
// https://www.jpgrenier.org/clouds.html
// https://erk.fe.uni-lj.si/2025/papers/loboda%28real_time_volumetric%29.pdf
// https://forums.unrealengine.com/t/distorting-textures-with-flow-maps/68111
// https://forums.odforce.net/topic/23724-flow-map-from-fluid-velocity-for-game-application/
#include "Atmosphere.hlsli"
#include "Ray.hlsli"
#include "Remap.hlsli"
#include "Maths.hlsli"
#define DIV                2

SamplerState LinearWrapSampler : register(s0);
Texture2D<float> SceneDepthTexture : register(t1);
Texture2D WeatherMapTexture : register(t2);
Texture3D VolmeNoiseTexture : register(t3);
Texture3D DetailNoiseTexture : register(t4);
Texture2D BlueNoiseTexture : register(t5);
RWTexture2D<float4> OutCloudResult : register(u0);

cbuffer CloudBuffer : register(b0)
{
    // [Row 1] 고도 및 노이즈 스케일
    float cCloudMinHeight;
    float cCloudMaxHeight;
    float cShapeScale; // 베이스 구름 덩어리 크기 (예: 0.0001)
    float cDetailScale; // 디테일 노이즈 크기 (예: 0.002)

    // [Row 2] 바람 애니메이션
    float3 cWindDirection;
    float cWindSpeed;

    // [Row 3] 형태 제어 (Shaping)
    float cGlobalCoverage; // 전체 구름 양 (0.0 ~ 1.0)
    float cCloudDensityScale; // 구름 밀도 스케일
    float cCrispiness; // 디테일 노이즈 강도 (구름을 깎아내는 정도)
    float cPowderScale; // 파우더 효과(Silver Lining) 강도

    // [Row 4] 레이마칭 및 라이팅 제어
    int cMaxSteps; // 메인 레이마칭 최대 스텝 수
    int cLightSteps; // 빛(그림자) 레이마칭 스텝 수
    float cLightAbsorption; // 빛이 구름을 통과할 때 흡수되는 정도
    float cPadding;
};

cbuffer RayBuffer : register(b1)
{
    // [Row 1 ~ 2]
    matrix rViewInv;
    // [Row 3 ~ 4]
    matrix rProjInv;
    // [Row 5]
    float3 rCameraPosition;
    float rPadding1;
    // [Row 6]
    float3 rLightDir;
    float rPadding2;
    // [Row 7]
    float4 rLightAmbient;
    // [Row 8]
    float4 rLightDiffuse;
    // [Row 9]
    float3 rPlanetCenter;
    float rPlanetRadius;
    // [Row 10]
    float2 rResolution;
    float2 rPadding3;
}; // RayBuffer

float GetHeightGradient(float heightFrac, float cloudType)
{
    // Stratus(층운), Stratocumulus(층적운), Cumulus(적란운)의 형태 프로파일 보간
    float stratus = 1.0 - clamp(heightFrac * 2.0, 0.0, 1.0);
    float cumulus = 1.0 - smoothstep(0.7, 1.0, heightFrac);
    return lerp(stratus, cumulus, cloudType);
}

// 벤치마킹된 구름 밀도 함수
float GetCloudDensityPorted(float3 pos, float3 windOffset, float3 planetCenter, float innerRadius, float outerRadius, bool isLightMarch)
{
    float distFromCenter = length(pos - planetCenter);
    float heightFrac = saturate((distFromCenter - innerRadius) / (outerRadius - innerRadius));
    
    if (heightFrac <= 0.0 || heightFrac >= 1.0)
        return 0.0;

    // 웨더맵 샘플링 (스케일은 맵 크기에 맞게 조절)
    float2 weatherUV = (pos.xz + windOffset.xz) * 0.00005;
    float4 weatherMap = WeatherMapTexture.SampleLevel(LinearWrapSampler, weatherUV, 0);
    
    float coverage = weatherMap.r * cGlobalCoverage; // 구름 분포양
    float cloudType = weatherMap.g; // 구름 종류

    // 빈 공간이면 조기 종료 (성능 최적화의 핵심)
    if (coverage < 0.01)
        return 0.0;

    //  고도별 그라디언트 뼈대 생성
    float heightGradient = GetHeightGradient(heightFrac, cloudType);
    heightGradient *= smoothstep(0.0, 0.05, heightFrac); // 바닥 면을 살짝 부드럽게 둥글림

    // Base Noise로 덩어리 만들기
    float3 baseUV = (pos + windOffset) * cShapeScale;
    float4 baseNoise = VolmeNoiseTexture.SampleLevel(LinearWrapSampler, baseUV, 0);
    float baseFBM = baseNoise.r * 0.625 + baseNoise.g * 0.25 + baseNoise.b * 0.125;

    // 웨더맵(Coverage)을 기준으로 밀도 재배치(Remap)
    float baseDensity = saturate(remap_clamp(baseFBM, 1.0 - coverage, 1.0, 0.0, 1.0));
    baseDensity *= heightGradient;

    // Detail Noise로 가장자리 깎아내기 (Erosion)
    // 조명 계산(isLightMarch) 중일 때는 디테일을 계산하지 않아 프레임을 폭발적으로 아낍니다.
    if (baseDensity > 0.0 && !isLightMarch)
    {
        float3 detailUV = (pos + windOffset) * cDetailScale;
        float4 detailNoise = DetailNoiseTexture.SampleLevel(LinearWrapSampler, detailUV, 0);
        float detailFBM = detailNoise.r * 0.625 + detailNoise.g * 0.25 + detailNoise.b * 0.125;
        
        // 구름의 가장자리 위주로 침식(Erosion) 시킴
        float erosion = 1.0 - detailFBM;
        baseDensity = saturate(remap_clamp(baseDensity, erosion * cCrispiness, 1.0, 0.0, 1.0));
    }

    return baseDensity * cCloudDensityScale;
}

float LightMarchPorted(float3 pos, float3 lightDir, float3 windOffset, float3 planetCenter, float innerRadius, float outerRadius, float currentDensity)
{
    float totalDensity = 0.0;
    float stepSize = 100.0; // 태양 방향 스텝 보폭 (미터 단위)
    float3 rayPos = pos;
    
    // Cone Marching: 태양으로 향할수록 보폭을 넓게 뛰어서 소프트 섀도우 연출
    for (int i = 0; i < cLightSteps; ++i)
    {
        rayPos += lightDir * stepSize;
        float density = GetCloudDensityPorted(rayPos, windOffset, planetCenter, innerRadius, outerRadius, true);
        totalDensity += density * stepSize;
        stepSize *= 1.5; // 거리가 멀어질수록 보폭을 1.5배씩 넓힘
    }
    
    // 다중 산란 근사(Multiple Scattering Approximation)
    float d = totalDensity * cLightAbsorption;
    float transmittance = exp(-d) + 0.5 * exp(-d * 0.25) + 0.25 * exp(-d * 0.1);
    
    // 은빛 테두리를 만드는 파우더 효과(Powder Effect)
    float powder = 1.0 - exp(-currentDensity * cPowderScale);
    
    return transmittance * powder;
}

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    if (DTid.x >= (uint) rResolution.x || DTid.y >= (uint) rResolution.y)
        return;

    float2 uv = (float2(DTid.xy) + 0.5f) / rResolution;
    float3 rd = ray_direction_restore(uv, rProjInv, rViewInv);
    float3 ro = rCameraPosition / KM_UNIT;
    float3 planetToCam = ro - rPlanetCenter;
    float currentHeight = length(planetToCam);

    // 구름 레이마칭 영역 설정 (고도)
    float r_inner = rPlanetRadius + cCloudMinHeight;
    float r_outer = rPlanetRadius + cCloudMaxHeight;
    
    float2 inner_isect = ray_sphere_intersect(planetToCam, rd, r_inner);
    float2 outer_isect = ray_sphere_intersect(planetToCam, rd, r_outer);
    
    if (outer_isect.y < 0.0)
    {
        OutCloudResult[DTid.xy] = float4(0, 0, 0, 0);
        return;
    }

    // 레이 시작/끝점 결정 (tMin, tMax)
    float tMin, tMax;
    if (currentHeight < r_inner)
    {
        if (rd.y < 0.0)
        {
            OutCloudResult[DTid.xy] = 0;
            return;
        }
        tMin = max(0.0, inner_isect.y);
        tMax = outer_isect.y;
    }
    else if (currentHeight < r_outer)
    {
        tMin = 0.0;
        tMax = outer_isect.y;
        if (inner_isect.x > 0.0)
            tMax = min(tMax, inner_isect.x);
    }
    else
    {
        tMin = outer_isect.x;
        tMax = outer_isect.y;
        if (inner_isect.x > 0.0)
            tMax = min(tMax, inner_isect.x);
    }

    if (tMin < 0.0 || tMin >= tMax)
    {
        OutCloudResult[DTid.xy] = 0;
        return;
    }
    
    float jitter = BlueNoiseTexture.SampleLevel(LinearWrapSampler, noiseUV, 0).r;
    t += stepSize * jitter;

    float3 accumulatedColor = float3(0, 0, 0);
    float transmittance = 1.0;
    
    float3 accumulatedColor = float3(0, 0, 0);
    float transmittance = 1.0;

[loop]
    for (int i = 0; i < cMaxSteps; ++i)
    {
        if (t > tMax || transmittance < 0.01)
            break;

        float3 pos = ro + rd * t;
        float3 windOffset = float3(cWindDirection.x, 0.0, cWindDirection.y) * cWindSpeed * cTime;
    
    // 1. 밀도 샘플링 (여기서 !isLightMarch = false로 호출)
        float density = GetCloudDensityPorted(pos, windOffset, rPlanetCenter, r_inner, r_outer, false);
    
        if (density > 0.0)
        {
        // 2. 밀도가 0보다 클 때만 조명 연산 진행 (비싼 연산)
            float lightEnergy = LightMarchPorted(pos, -rLightDir, windOffset, rPlanetCenter, r_inner, r_outer, density);
        
        // 3. 최종 빛 결합 (Directional + Ambient)
            float phase = HenyeyGreenstein(dot(rd, -rLightDir), 0.5); // 위상 함수
            float3 directionalLight = cLightDiffuse.rgb * lightEnergy * phase;
        
        // 앰비언트 (고도에 따라 구름 하단은 어둡고 상단은 밝게)
            float heightFrac = saturate((length(pos - rPlanetCenter) - r_inner) / (r_outer - r_inner));
            float3 ambientLight = lerp(float3(0.2, 0.2, 0.25), float3(0.5, 0.6, 0.75), heightFrac);
        
            float3 finalLight = (directionalLight + ambientLight) * cCloudBaseColor; // 구름 기본 색상 적용
        
        // 4. 투과율 누적 연산
            float alpha = density * stepSize;
            accumulatedColor += finalLight * alpha * transmittance;
            transmittance *= exp(-alpha);
        }
    
        t += stepSize;
    }
    
}

float CloudGradient(float h)
{
    return smoothstep(0.0, 0.05, h) * (1.0 - smoothstep(0.5, 1.0, h));
} // CloudGradient: 구름의 상단과 하단을 부드럽게 지워주는 그라디언트 함수

float CloudHeightFract(float p, float innerRadius, float outerRadius)
{
    return saturate((p - innerRadius) / (outerRadius - innerRadius));

} // CloudHeightFract: 구름의 고도 비율 계산 함수 (0.0 = 구름 하단, 1.0 = 구름 상단)

float GetCloudDensity2(float3 pos, float3 windOffset, float3 planetCenter, float innerRadius, float outerRadius, bool lowFreq = false)
{
    float distFromCenter = length(pos - planetCenter);
    float heightFrac = CloudHeightFract(distFromCenter, innerRadius, outerRadius);
    
    if (heightFrac <= 0.0 || heightFrac >= 1.0)
    {
        return 0.0;
    }

    float3 samplePos = pos + windOffset;
    
    float2 weatherUV = samplePos.xz * 0.0008;
    float4 weatherData = CloudMapTexture.SampleLevel(LinearWrapSampler, weatherUV, 0);
    
    float coverage = weatherData.r;
    float densityMask = weatherData.a;
    float poor = saturate(coverage * densityMask);

    if (poor <= 0.01)
        return 0.0;

    float2 flowData = FlowMapTexture.SampleLevel(LinearWrapSampler, weatherUV, 0).rg;
    float2 flowVector = flowData * 2.0 - 1.0;
    float3 distortedPos = samplePos;
    distortedPos.xz += flowVector * cTime * 0.4;

    float finaldense = 0.0;

    // 권운과 적운을 나눠서 각각 다른 노이즈 스케일과 레이어링으로 밀도를 계산
    // 1.권운 : 촘촘한 노이즈 + 상단부 레이어링
    {
        float3 baseUV1 = distortedPos * 0.0006;
        float4 largeNoiseValue = VolmeNoiseTexture.SampleLevel(LinearWrapSampler, baseUV1, 0);
        
        float dense = remap_clamp(largeNoiseValue.r, 1.0 - poor * 0.75, 1.0, 0.0, 1.0);
        if (!lowFreq)
        {
            dense = remap_clamp(dense, 1.0 - largeNoiseValue.g, 1.0, 0.0, 1.0);
            dense = remap_clamp(dense, 1.0 - largeNoiseValue.b, 1.0, 0.0, 1.0);
            dense = remap_clamp(dense, 1.0 - largeNoiseValue.a, 1.0, 0.0, 1.0);
        }
        float height1 = saturate(heightFrac / 0.5);
        float cumulusLayer = remap_clamp(height1, 0.00, 0.20, 0.0, 1.0) * remap_clamp(height1, 0.20, 1.00, 1.0, 0.0);
        dense = remap_clamp(dense, 1.0 - cumulusLayer, 1.0, 0.0, 1.0);

        float anvil = 1.0;
        float slope = 0.2;
        float bottomWide = 0.8;
        dense = pow(dense, remap_clamp(1.0 - height1, slope, bottomWide, 1.0, lerp(1.0, 0.5, anvil)));
        
        finaldense = dense;
    }

    // 2.적운 : 조금 더 촘촘한 노이즈 + 상단부 레이어링
    {
        float3 baseUV2 = (distortedPos + float3(0.5, 0.0, 0.5)) * 0.002; // 권운은 조금 더 촘촘하게
        float4 largeNoiseValue2 = VolmeNoiseTexture.SampleLevel(LinearWrapSampler, baseUV2, 0);
        
        float dense = remap_clamp(largeNoiseValue2.r, 1.0 - poor * 0.5, 1.0, 0.0, 1.0);
        if (!lowFreq)
        {
            dense = remap_clamp(dense, 1.0 - largeNoiseValue2.g, 1.0, 0.0, 1.0);
            dense = remap_clamp(dense, 1.0 - largeNoiseValue2.b, 1.0, 0.0, 1.0);
            dense = remap_clamp(dense, 1.0 - largeNoiseValue2.a, 1.0, 0.0, 1.0);
        }

        float height2 = saturate((heightFrac - 0.7) / 0.3);
        float cirrusLayer = remap_clamp(height2, 0.00, 0.20, 0.0, 1.0) * remap_clamp(height2, 0.20, 1.00, 1.0, 0.0);
        dense = remap_clamp(dense, 1.0 - cirrusLayer, 1.0, 0.0, 1.0);

        float anvil = 1.0;
        float slope = 0.2;
        float bottomWide = 0.8;
        dense = pow(dense, remap_clamp(1.0 - height2, slope, bottomWide, 1.0, lerp(1.0, 0.5, anvil)));
  
        finaldense = max(finaldense, dense);
    }

    // 3. 디테일 침식 (Remap Erosion)
    if (!lowFreq)
    {
        float3 detailUV = distortedPos * 0.03f;
        float4 detail1 = DetailNoiseTexture.SampleLevel(LinearWrapSampler, detailUV, 0);
        float4 detail2 = DetailNoiseTexture.SampleLevel(LinearWrapSampler, detailUV * 2.0, 0);
        
        float detailFBM1 = detail1.r * 0.625 + detail1.g * 0.25 + detail1.b * 0.125;
        float detailFBM2 = detail2.r * 0.625 + detail2.g * 0.25 + detail2.b * 0.125;
        float finalDetail = detailFBM1 * 0.75 + detailFBM2 * 0.25;
        
        float erosionModifier = lerp(0.2, 1.0, pow(heightFrac, 0.5));
        finaldense = remap_clamp(finaldense, finalDetail * erosionModifier, 1.0, 0.0, 1.0);
    }

    return finaldense;
}

float GetCloudDensity3(float3 pos, float3 windOffset, float3 planetCenter, float innerRadius, float outerRadius, bool lowFreq = false)
{
    // 1. 고도 비율 계산 (0.0 = 구름 하단, 1.0 = 구름 상단)
    float distFromCenter = length(pos - planetCenter);
    float heightFrac = CloudHeightFract(distFromCenter, innerRadius, outerRadius);
    
    if (heightFrac <= 0.0 || heightFrac >= 1.0)
    {
        return 0.0;
    }

    float3 samplePos = pos + windOffset;
    
    // Weather Map 및 Flow Map 왜곡
    float2 weatherUV = samplePos.xz * 0.0008;
    //float2 weatherUV = samplePos.xz * 0.0001;
    float4 weatherData = CloudMapTexture.SampleLevel(LinearWrapSampler, weatherUV, 0);
    float coverage = saturate(weatherData.r * weatherData.a);
    
    if (coverage <= 0.01)
        return 0.0;

    float2 flowData = FlowMapTexture.SampleLevel(LinearWrapSampler, weatherUV, 0).rg;
    float2 flowVector = flowData * 2.0 - 1.0;
    float3 distortedPos = samplePos;
    distortedPos.xz += flowVector * cTime * 0.3;

    // 고도에 따른 기본 그라디언트 (상단과 하단을 부드럽게 지워줌)
    float heightGradient = CloudGradient(heightFrac);

    // Base 3D Noise 샘플링
    float3 baseUV = distortedPos * 0.008;
    //float3 baseUV = distortedPos * 0.001;
    float4 baseNoise = VolmeNoiseTexture.SampleLevel(LinearWrapSampler, baseUV, 0);
    
    // - 하단부: pow(1.0 - heightFrac, 12.0)로 인해 바닥이 매우 평평하게 잘림
    // - 상단부: 노이즈(baseNoise.b)를 곱해 위로 갈수록 뭉게뭉게 불규칙하게 솟아오름
    float heightModifier = (heightFrac * heightFrac * baseNoise.b) + pow(1.0 - heightFrac, 12.0);
    
    // Base 덩어리(R채널)에서 형태 변형값을 뺌
    float baseDensity = baseNoise.r - heightModifier * 0.5;
    baseDensity = saturate(remap_clamp(baseDensity, baseNoise.g - 1.0, 1.0, 0.0, 1.0));

    // Coverage(구름 맵) 적용하여 최종 베이스 밀도 계산
    baseDensity = saturate(remap_clamp(baseDensity, 1.0 - coverage, 1.0, 0.0, 1.0)) * coverage;
    baseDensity *= heightGradient;

    //  디테일 침식 (Remap Erosion
    if (baseDensity > 0.0 && !lowFreq)
    {
        float3 detailUV = distortedPos * 0.3;
        float4 detailNoise = DetailNoiseTexture.SampleLevel(LinearWrapSampler, detailUV, 0);
        
        // 디테일 노이즈 조합 (R, G, B 채널)
        float detailFBM = detailNoise.r * 0.625 + detailNoise.g * 0.25 + detailNoise.b * 0.125;
        
        // 이렇게 하면 하단은 부드럽게 흩어지고, 상단은 브로콜리(Cauliflower)처럼 단단
        float detailMod = lerp(detailFBM, 1.0 - detailFBM, saturate(heightFrac * 4.0));

        // remap_clamp의 하한선(min) 자리에 detailMod를 넣어서 '가장자리'만 날카롭게 파먹도록
        float erosionStrength = detailMod * cDetailStrength;
        baseDensity = saturate(remap_clamp(baseDensity, erosionStrength, 1.0, 0.0, 1.0));
    }

    return baseDensity;
}

float GetLightEnergy(float3 pos, float3 lightDir, int curStep,
    float3 windOffset, float3 planetCenter, float ray_inner, float ray_outer,
    float shadowFactor, float density)
{
    float totalShadowDensity = 0.0;
    
    // 태양 방향으로 고정된 거리만큼 3번 샘플링하여 짙은 그림자 누적
    float shadowStepSize = 5.0; // 0.5km 간격으로 태양 쪽 확인
    
    for (int i = 1; i <= 3; i++)
    {
        float3 lightSamplePos = pos + (-lightDir * (shadowStepSize * (float) i));
        float d = GetCloudDensity3(lightSamplePos, windOffset, planetCenter, ray_inner, ray_outer, true);
        totalShadowDensity += d;
    }
            
    // Beer-Lambert Law 
    float lightEnergy = exp(-totalShadowDensity * shadowFactor);
            
    // Powder Effect
    float powderTerm = BeerPowder(density);
            
    return lightEnergy * powderTerm;
} // GetLightEnergy: 주어진 위치에서 태양 빛이 얼마나 도달하는지 계산하는 함수

//float MarchToLight(float3 p, float3 sunDir, float scatterHeight, float3 rPlanetCenter, float r_inner, float r_outer, float3 windOffset)
//{
//    // 광선이 나아갈 한 스텝의 크기 (사용자님의 엔진 스케일에 맞춰 조절 필요)
//    float lightRayStepSize = 0.5f;
//    float3 lightRayDir = -sunDir * lightRayStepSize; // 태양 방향으로 역추적
//    float3 lightRayDist = lightRayDir * 0.5f;
    
//    // 콘(Cone) 확산 정도. 태양 방향으로 멀어질수록 샘플링 영역을 넓힘
//    float coneSpread = length(lightRayDir);
//    float totalDensity = 0.0f;

//    [unroll]
//    for (int i = 0; i < 6; ++i) // 보통 CLOUD_LIGHT_STEPS는 6 정도로 고정
//    {
//        // Cone Sampling: 현재 위치에서 태양 방향으로 가면서 주변을 원뿔 모양으로 샘플링
//        float3 cp = p + lightRayDist + (coneSpread * NoiseKernel[i] * (float) i);
        
//        // 현재 위치의 상대적 높이 (0.0 ~ 1.0)
//        float distToCenter = length(cp - rPlanetCenter);
//        float y = saturate((distToCenter - r_inner) / (r_outer - r_inner));

//        // 조기 종료 조건: 구름층을 벗어났거나 이미 빛이 다 차단된 경우
//        if (y > 0.99f || totalDensity > 0.95f)
//            break;

//        // 밀도 누적
//        //totalDensity += GetCloudDensity3(cp, windOffset, rPlanetCenter, r_inner, r_outer, false) * lightRayStepSize;
//        float d = GetCloudDensity4(p, windOffset, rPlanetCenter, r_inner, r_outer);
//        totalDensity += max(0.0, d);
//        // 다음 스텝으로 거리 증가
//        lightRayDist += lightRayDir;
//    }

//    float absorption = lerp(CLOUD_ABSORPTION_BOTTOM, CLOUD_ABSORPTION_BOTTOM, scatterHeight);
//    float beer = exp(-totalDensity * absorption);
//    float powder = 1.0f - exp(-totalDensity * 2.0f);
    
//    return beer * powder * 2.0f; // 2.0f는 강도 보정치
//}

float GetCloudBase(float3 p, float y)
{
    // 바람에 따른 이동 반영 (CloudBase용 저주파 노이즈)
    float3 windOffset = float3(cWindDirection.x, 0.0, cWindDirection.y) * cWindSpeed * cTime;
    float3 uvw = (p + windOffset) * 0.008; // CLOUD_BASE_FREQ 대용

    float4 noise = VolmeNoiseTexture.SampleLevel(LinearWrapSampler, uvw, 0);

    // 고도에 따른 형태 보정 (레퍼런스 로직)
    // - 하단은 pow(1-y, 12)로 평평하게, 상단은 noise.b로 뭉게뭉게하게
    float n = (y * y * noise.b) + pow(saturate(1.0 - y), 12.0);
    
    // 기본 밀도 리맵핑
    float cloud = remap_clamp(noise.r - n, noise.g - 1.0, 1.0, 0.0, 1.0);
    
    return cloud;
}

// 2. 구름의 가장자리를 파먹는(Erosion) 디테일 함수
float GetCloudDetail(float3 p, float baseDensity, float y)
{
    // 디테일용 바람 이동 (보통 베이스보다 빠르게 설정)
    float3 windOffset = float3(cWindDirection.x, 0.0, cWindDirection.y) * cWindSpeed * cTime * 3.0;
    float3 uvw = (p + windOffset) * 0.3; // CLOUD_DETAIL_FREQ 대용

    float4 detailNoise = DetailNoiseTexture.SampleLevel(LinearWrapSampler, uvw, 0);
    
    // 디테일 FBM 조합 (R, G, B 채널)
    float hf = detailNoise.r * 0.625 + detailNoise.g * 0.25 + detailNoise.b * 0.125;
    
    // 고도에 따라 디테일 반전 (상단은 뽀글뽀글하게, 하단은 부드럽게)
    hf = lerp(hf, 1.0 - hf, saturate(y * 4.0));
    
    // 베이스 밀도에서 디테일만큼 깎아냄 (remap_clamp 이용)
    return remap_clamp(baseDensity, hf * cDetailStrength, 1.0, 0.0, 1.0);
}

// 3. 최종 밀도를 조립하는 메인 함수
float GetCloudDensity(float3 pos, float3 planetCenter, float innerRadius, float outerRadius, bool lowFreq = false)
{
    // [고도 계산]
    float distFromCenter = length(pos - planetCenter);
    float y = CloudHeightFract(distFromCenter, innerRadius, outerRadius);
    
    if (y <= 0.0 || y >= 1.0)
        return 0.0;

    // [로직 추가] Cloud Top Offset (고도가 높을수록 바람에 밀리는 효과)
    // 레퍼런스의 p.xz -= WIND_DIR.xz * y * CLOUD_TOP_OFFSET 반영
    float3 p = pos;
    p.xz -= cWindDirection.xy * y * 0.5; // 0.5는 Offset 강도

    // [웨더맵 샘플링]
    float2 weatherUV = p.xz * 0.0005;
    float4 weatherData = CloudMapTexture.SampleLevel(LinearWrapSampler, weatherUV, 0);
    float coverage = saturate(weatherData.r * weatherData.a);
    
    if (coverage <= 0.01)
        return 0.0;

    // [1. 베이스 밀도 계산]
    float d = GetCloudBase(p, y);
    
    // 커버리지(웨더맵) 적용
    d = remap_clamp(d, 1.0 - coverage, 1.0, 0.0, 1.0) * coverage;
    
    // 고도 그라디언트 적용 (전체적인 구름 두께감 조절)
    d *= CloudGradient(y);

    // [2. 디테일 침식 적용]
    // 베이스 밀도가 적당히 있고 디테일 연산이 필요한 경우에만 실행 (최적화)
    bool isDetailRange = (d > 0.0 && d < 0.7); // 0.3보다 조금 더 넓게 잡는 것이 자연스럽습니다.
    
    if (isDetailRange && !lowFreq)
    {
        d = GetCloudDetail(p, d, y);
    }

    return d;
}

float GetDensityForCloudGradient(float heightFraction, float cloudType)
{
    // Cloud Type에 따른 3가지 기본 형태의 가중치
    float stratusFactor = 1.0 - saturate(cloudType * 2.0);
    float stratoCumulusFactor = 1.0 - abs(cloudType - 0.5) * 2.0;
    float cumulusFactor = saturate(cloudType - 0.5) * 2.0;

    // 각 구름 형태별 고도(Height) 그라디언트 범위 [하단 컷, 하단 최고점, 상단 꺾임, 상단 컷]
    float4 stratusGrad = float4(0.0, 0.1, 0.2, 0.3);
    float4 stratoCumulusGrad = float4(0.02, 0.2, 0.48, 0.625);
    float4 cumulusGrad = float4(0.00, 0.1625, 0.88, 0.98);

    // 구름 타입에 맞춰 그라디언트 혼합
    float4 baseGradient = stratusFactor * stratusGrad +
                          stratoCumulusFactor * stratoCumulusGrad +
                          cumulusFactor * cumulusGrad;

    // 하단부는 부드럽게 나타나고, 상단부는 부드럽게 사라지도록 smoothstep 적용
    return smoothstep(baseGradient.x, baseGradient.y, heightFraction) -
           smoothstep(baseGradient.z, baseGradient.w, heightFraction);
}

float GetCloudDensity4(float3 pos, float3 windOffset, float3 planetCenter, float innerRadius, float outerRadius, bool lowFreq = false)
{
    float heightFraction = CloudHeightFract(length(pos - planetCenter), innerRadius, outerRadius);
    if (heightFraction <= 0.0 || heightFraction >= 1.0)
        return 0.0;

    float2 uv = pos.xz * 0.00008; // 텍스처 타일링 스케일 (ImGui 등으로 조절 추천)
    
    // 바람에 의한 상단부 쏠림 효과 (CLOUD_TOP_OFFSET)
    float3 animation = heightFraction * float3(cWindDirection.x, 0.0, cWindDirection.y) * 7.5 + windOffset;
    float2 moving_uv = (pos + animation).xz * 0.00008;

    // 베이스 노이즈 샘플링 (128x128x128)
    float3 baseUV = float3(uv * cCloudDensityScale, heightFraction);
    float4 baseNoise = VolmeNoiseTexture.SampleLevel(LinearWrapSampler, baseUV, 0);
    
    // FBM 조합 (G, B, A 채널)
    float lowFreqFBM = dot(baseNoise.yzw, float3(0.625, 0.25, 0.125));
    // Perlin 노이즈(R)를 Worley FBM으로 깎아내어 기본 덩어리 생성
    float base_cloud = remap_clamp(baseNoise.x, -(1.0 - lowFreqFBM), 1.0, 0.0, 1.0);
    
    // 웨더맵 샘플링 및 밀도 그라디언트 적용
    float4 weatherData = CloudMapTexture.SampleLevel(LinearWrapSampler, moving_uv, 0);
    float cloud_coverage = saturate(weatherData.r); // 웨더맵 R: 커버리지
    float cloud_type = saturate(weatherData.g); // 웨더맵 G: 구름 종류

    // 고도 및 타입에 따른 밀도 적용
    float densityGrad = GetDensityForCloudGradient(heightFraction, cloud_type);
    base_cloud *= (densityGrad / max(heightFraction, 0.0001));

    // 커버리지 기반 리맵핑
    float base_cloud_with_coverage = remap_clamp(base_cloud, cloud_coverage, 1.0, 0.0, 1.0);
    base_cloud_with_coverage *= cloud_coverage;

    // 디테일 침식 (Erosion) - 그림자 계산(lowFreq)이 아닐 때만 수행하여 성능 최적화
    if (!lowFreq && base_cloud_with_coverage > 0.0)
    {
        // 디테일 노이즈 샘플링 (32x32x32) - 베이스보다 더 잘게 타일링 (curliness 대용)
        float3 detailUV = float3(moving_uv * cDetailStrength * 7.0, heightFraction);
        //float3 detailUV = float3(moving_uv * cDetailStrength * 5.0, heightFraction);
        float4 detailNoise = DetailNoiseTexture.SampleLevel(LinearWrapSampler, detailUV, 0);
        
        // 고주파 FBM 조합
        float highFreqFBM = dot(detailNoise.rgb, float3(0.625, 0.25, 0.125));
        
        // 고도에 따른 디테일 변조: 상단부일수록 1.0-FBM을 섞어 뽀글뽀글하게 만듦
        float highFreqNoiseModifier = lerp(highFreqFBM, 1.0 - highFreqFBM, saturate(heightFraction * 10.0));
        
        // 가장자리만 깎아내기
        base_cloud_with_coverage = base_cloud_with_coverage - highFreqNoiseModifier * (1.0 - base_cloud_with_coverage);
        //base_cloud_with_coverage = remap_new(base_cloud_with_coverage * 2.0, highFreqNoiseModifier * 0.2, 1.0, 0.0, 1.0);
        base_cloud_with_coverage = remap_clamp(base_cloud_with_coverage * 2.0, highFreqNoiseModifier * 0.2, 1.0, 0.0, 1.0);
    }

    return saturate(base_cloud_with_coverage);
}

float MarchToLight(float3 p, float3 sunDir, float scatterHeight, float3 rPlanetCenter, float r_inner, float r_outer, float3 windOffset)
{
    // 광선이 나아갈 한 스텝의 크기 (사용자님의 엔진 스케일에 맞춰 조절 필요)
    float lightRayStepSize = 1.5f;
    float3 lightRayDir = -sunDir * lightRayStepSize; // 태양 방향으로 역추적
    float3 lightRayDist = lightRayDir * 0.5f;
    
    // 콘(Cone) 확산 정도. 태양 방향으로 멀어질수록 샘플링 영역을 넓힘
    float coneSpread = length(lightRayDir);
    float totalDensity = 0.0f;

    [unroll]
    for (int i = 0; i < 6; ++i) // 보통 CLOUD_LIGHT_STEPS는 6 정도로 고정
    {
        // Cone Sampling: 현재 위치에서 태양 방향으로 가면서 주변을 원뿔 모양으로 샘플링
        float3 cp = p + lightRayDist + (coneSpread * NoiseKernel[i] * (float) i);
        
        // 현재 위치의 상대적 높이 (0.0 ~ 1.0)
        float distToCenter = length(cp - rPlanetCenter);
        float y = saturate((distToCenter - r_inner) / (r_outer - r_inner));

        // 조기 종료 조건: 구름층을 벗어났거나 이미 빛이 다 차단된 경우
        if (y > 0.99f || totalDensity > 0.95f)
            break;

        // 밀도 누적
        float d = GetCloudDensity4(p, windOffset, rPlanetCenter, r_inner, r_outer, true);
        totalDensity += max(0.0, d);
        // 다음 스텝으로 거리 증가
        lightRayDist += lightRayDir;
    }

    float absorption = lerp(CLOUD_ABSORPTION_BOTTOM, CLOUD_ABSORPTION_BOTTOM, scatterHeight);
    float beer = exp(-totalDensity * absorption);
    float powder = 1.0f - exp(-totalDensity * 2.0f);
    
    return beer * powder * 2.0f; // 2.0f는 강도 보정치
}

//[numthreads(8, 8, 1)]
//void main(uint3 DTid : SV_DispatchThreadID)
//{
//    if (DTid.x >= (uint) rResolution.x || DTid.y >= (uint) rResolution.y)
//        return;

//    float2 uv = (float2(DTid.xy) + 0.5f) / rResolution;
//    float3 rd = ray_direction_restore(uv, rProjInv, rViewInv);
//    float3 ro = rCameraPosition / KM_UNIT; // km 단위 변환
//    //float depth = SceneDepthTexture.Load(int3(DTid.xy * DIV, 0)).r;
//    float max_dist = MAX_DIST;

//    // 구름 레이마칭 영역 교차점
//    float r_inner = rPlanetRadius + cCloudMinHeight;
//    float r_outer = rPlanetRadius + cCloudMaxHeight;
    
//    float2 inner_isect = ray_sphere_intersect(ro - rPlanetCenter, rd, r_inner);
//    float2 outer_isect = ray_sphere_intersect(ro - rPlanetCenter, rd, r_outer);
    
//    if (outer_isect.y < 0.0)
//    {
//        OutCloudResult[DTid.xy] = float4(0, 0, 0, 0);
//        return;
//    }

//    float tMin = -1.0;
//    float tMax = -1.0;
//    float currentHeight = length(ro - rPlanetCenter);

//    if (currentHeight < r_inner)
//    {
//        if (rd.y < 0.0)
//        {
//            OutCloudResult[DTid.xy] = float4(0, 0, 0, 0);
//            return;
//        }
    
//        tMin = max(0.0, inner_isect.y);
//        tMax = outer_isect.y;
//    }
//    else if (currentHeight < r_outer)
//    {
//        tMin = 0.0;
//        tMax = outer_isect.y;
    
//        if (inner_isect.x > 0.0)
//            tMax = min(tMax, inner_isect.x);
//    }
//    else
//    {
//        tMin = outer_isect.x;
//        tMax = outer_isect.y;

//        if (inner_isect.x > 0.0)
//            tMax = min(tMax, inner_isect.x);
//    }

//    tMax = min(tMax, max_dist);
//    if (tMin < 0.0 || tMin >= tMax)
//    {
//        OutCloudResult[DTid.xy] = float4(0, 0, 0, 0);
//        return;
//    }
  
//    float stepSize = (tMax - tMin) / (float) cStep;
//    float t = tMin;

//    float2 noiseUV = float2(DTid.xy % rResolution.x) / rResolution.x;
//    float blueNoise = BlueNoiseTexture.SampleLevel(LinearWrapSampler, noiseUV, 0).r;
//    t += stepSize * frac(blueNoise); // 블루 노이즈 지터링
    
//    float transmittance = cTransmittance;
//    float3 accumulatedColor = float3(0, 0, 0);
    
//    float2 sunUV = direction_to_uv(-rLightDir);
//    float3 atmosphericSunColor = SkyLUTTexture.SampleLevel(LinearWrapSampler, sunUV, 0).rgb;
//    float2 ambientUV = direction_to_uv(DEFAULT_LIGHT_DIR);
//    float3 atmosphericAmbientColor = SkyLUTTexture.SampleLevel(LinearWrapSampler, ambientUV, 0).rgb;
    
//    float cosTheta = dot(rd, -rLightDir);
//    float forwardHG = HenyeyGreenstein(cosTheta, cHGForward);
//    float backwardHG = HenyeyGreenstein(cosTheta, cHGBackward);
//    float phase = lerp(forwardHG, backwardHG, cHGBlend);
//    bool hit = false;

//    [loop]
//    for (int i = 0; i < cStep; ++i)
//    {
//        if (t > tMax)
//            break; // 최대 거리 도달 시 조기 종료

//        float3 pos = ro + rd * t;
//        float3 windOffset = float3(cWindDirection.x, 0.0, cWindDirection.y) * cWindSpeed * cTime;
//        float3 samplePos = pos + windOffset;
//        // 밀도 확인
//        float density = GetCloudDensity4(pos, windOffset, rPlanetCenter, r_inner, r_outer, false);
//        //float density = GetCloudDensity(pos, rPlanetCenter, r_inner, r_outer, false);
        
//        if (density > 0.0)
//        {
//            if (!hit)
//            {
//                //cloudDepth = t / MAX_DIST;
//                hit = true;
//            }
            
//            float ambientPhase = 0.5; // 사방으로 퍼지는 기초 밝기
//            float finalPhase = lerp(ambientPhase, phase, 1.0);
            
//            float heightPercent = saturate((length(pos - rPlanetCenter) - r_inner) / (r_outer - r_inner));
//            float3 ambientLight = atmosphericAmbientColor * lerp(0.2, 0.8, heightPercent);
//            float powderTerm = BeerPowder(density);
//            float scatterHeight = saturate((length(pos - rPlanetCenter) - r_inner) / (r_outer - r_inner));
//            float finalEnergy = MarchToLight(pos, rLightDir, scatterHeight, rPlanetCenter, r_inner, r_outer, windOffset);
//            //float finalEnergy = GetLightEnergy(pos, rLightDir, i, windOffset, rPlanetCenter, r_inner, r_outer, cShadowFactor, density);
//            float3 directionalLight = rLightAmbient.rgb * finalEnergy * phase * cLightIntensity;
//            float3 indirectLight = ambientLight * powderTerm;
            
//            //float3 shadowColor = lerp(rLightAmbient.rgb * 0.5f, float3(1, 1, 1), finalEnergy);
//            float3 shadowColor = lerp(atmosphericAmbientColor * 0.5f, float3(0, 0, 0), finalEnergy);
//            //float3 finalLight = (directionalLight + cAmbientColor) * cCloudBaseColor;
//            //float3 finalLight = shadowColor * atmosphericSunColor * cCloudBaseColor * phase;
//            float3 finalLight = (directionalLight + indirectLight) * cCloudBaseColor;
            
//            float alpha = density * stepSize * cCloudDensityScale;
//            accumulatedColor += finalLight * alpha * transmittance;
//            transmittance *= exp(-alpha);
            

//            if (transmittance < 0.1)
//                break; // 완전 불투명해지면 조기 종료
//        }

//        t += stepSize; // 레이 전진
//    }
    
//    float distanceAlpha = 1.0 - smoothstep(cFadeStart, cFadeDistance, t);
//    float3 upDir = normalize(ro - rPlanetCenter);
//    float viewAngle = dot(rd, upDir);
//    float horizonFade = smoothstep(0.02, 0.15, viewAngle);
//    float finalCloudAlpha = (1.0 - transmittance) * distanceAlpha * horizonFade;

//    OutCloudResult[DTid.xy] = float4(accumulatedColor, finalCloudAlpha);
//} // main

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    if (DTid.x >= (uint) rResolution.x || DTid.y >= (uint) rResolution.y)
        return;

    float2 uv = (float2(DTid.xy) + 0.5f) / rResolution;
    float3 rd = ray_direction_restore(uv, rProjInv, rViewInv);
    float3 ro = rCameraPosition / KM_UNIT;
    float3 planetToCam = ro - rPlanetCenter;
    float currentHeight = length(planetToCam);

    // 구름 레이마칭 영역 설정 (고도)
    float r_inner = rPlanetRadius + cCloudMinHeight;
    float r_outer = rPlanetRadius + cCloudMaxHeight;
    
    float2 inner_isect = ray_sphere_intersect(planetToCam, rd, r_inner);
    float2 outer_isect = ray_sphere_intersect(planetToCam, rd, r_outer);
    
    if (outer_isect.y < 0.0)
    {
        OutCloudResult[DTid.xy] = float4(0, 0, 0, 0);
        return;
    }

    // 레이 시작/끝점 결정 (tMin, tMax)
    float tMin, tMax;
    if (currentHeight < r_inner)
    {
        if (rd.y < 0.0)
        {
            OutCloudResult[DTid.xy] = 0;
            return;
        }
        tMin = max(0.0, inner_isect.y);
        tMax = outer_isect.y;
    }
    else if (currentHeight < r_outer)
    {
        tMin = 0.0;
        tMax = outer_isect.y;
        if (inner_isect.x > 0.0)
            tMax = min(tMax, inner_isect.x);
    }
    else
    {
        tMin = outer_isect.x;
        tMax = outer_isect.y;
        if (inner_isect.x > 0.0)
            tMax = min(tMax, inner_isect.x);
    }

    if (tMin < 0.0 || tMin >= tMax)
    {
        OutCloudResult[DTid.xy] = 0;
        return;
    }

    // 지터링 및 루프 준비
    float stepSize = (tMax - tMin) / (float) cStep;
    float2 noiseUV = float2(DTid.xy % 128) / 128.0f; // 블루노이즈 텍스처 크기에 맞춤
    tMin += stepSize * BlueNoiseTexture.SampleLevel(LinearWrapSampler, noiseUV, 0).r;

    float3 accumulatedColor = 0;
    float transmittance = 1.0; // cTransmittance 대신 1.0 시작 권장
    float3 windOffset = float3(cWindDirection.x, 0.0, cWindDirection.y) * cWindSpeed * cTime;

    // 루프 밖에서 미리 계산 가능한 조명 파라미터
    float cosTheta = dot(rd, -rLightDir);
    float phase = lerp(HenyeyGreenstein(cosTheta, cHGForward),
                       HenyeyGreenstein(cosTheta, cHGBackward), cHGBlend);

    [loop]
    for (int i = 0; i < cStep; ++i)
    {
        float t = tMin + stepSize * i;
        if (t > tMax || transmittance < 0.01)
            break;

        float3 pos = ro + rd * t;
        
        float density = GetCloudDensity4(pos, windOffset, rPlanetCenter, r_inner, r_outer, false);
        
        if (density > 0.0)
        {
            float heightPercent = saturate((length(pos - rPlanetCenter) - r_inner) / (r_outer - r_inner));
            
            // 조명 계산 정리
            float powderTerm = BeerPowder(density * stepSize);
            //float lightEnergy = MarchToLight(pos, rLightDir, heightPercent, rPlanetCenter, r_inner, r_outer, windOffset);
            
            //// 앰비언트: 고도에 따라 밑면은 어둡고 윗면은 하늘색을 더 받도록
            float3 ambientLight = SkyLUTTexture.SampleLevel(LinearWrapSampler, float2(0.5, 0.5), 0).rgb * lerp(0.2, 0.8, heightPercent);
            
            //float3 directionalLight = lightEnergy * phase * cLightIntensity * rLightDiffuse.rgb;
            float lightOpticalDepth = MarchToLight(pos, rLightDir, heightPercent, rPlanetCenter, r_inner, r_outer, windOffset);

            // 1. 빛의 투과율 (Beer's Law): 광원까지의 밀도 누적량에 따라 빛이 지수적으로 감소
            float lightAbsorption = 2.0; // 빛 흡수율 (조절 가능한 파라미터로 빼는 것이 좋습니다)
            float lightTransmittance = exp(-lightOpticalDepth * lightAbsorption);

            // 2. Powder Effect (Silver Lining): 구름 안으로 들어오는 빛이 구름 겉면을 밝게 비추는 현상
            // 기존 BeerPowder 함수 대신 직관적인 공식 사용
            //float powderTerm = 1.0 - exp(-density * stepSize * 2.0);

            // 최종 디렉셔널 라이트 (투과된 빛 * 파우더 효과 * 위상 함수)
            float3 directionalLight = lightTransmittance * powderTerm * phase * cLightIntensity * rLightDiffuse.rgb;
            float3 indirectLight = ambientLight * powderTerm;
            
            float3 stepColor = (directionalLight + indirectLight) * cCloudBaseColor.rgb;
            
            // 알파 및 투과율 갱신
            float alpha = density * stepSize * cCloudDensityScale;
            accumulatedColor += stepColor * alpha * transmittance;
            transmittance *= exp(-alpha);
        }
    }
    
    // 5. 페이드 및 최종 합성
    float3 upDir = normalize(ro - rPlanetCenter);
    float viewAngle = dot(rd, upDir);
    float horizonFade = smoothstep(0.0, 0.1, viewAngle); // 지평선 근처 부드럽게 처리
    //float distanceAlpha = 1.0 - smoothstep(cFadeStart, cFadeDistance, tMin);
    float distanceAlpha = 1.0 - smoothstep(cFadeStart, cFadeDistance, tMax);
    
    //float finalAlpha = (1.0 - transmittance) * distanceAlpha;
    float finalAlpha = (1.0 - transmittance) * distanceAlpha * horizonFade;
    OutCloudResult[DTid.xy] = float4(accumulatedColor, finalAlpha);
}


//float ComputeCloudDensity(float3 pos, float norY, float dist = 0.0f, bool isShadow = false)
//{
//    float m = GetCloudMapBase(pos, norY);
//    m *= cloud_gradient(norY); // 기존 베이스
    
//    // 고도(norY) 기반 모루(Anvil) 깎기
//    // norY가 1.0(구름 꼭대기)에 가까워질수록 윗부분을 깎아냅니다.
//    // anvilFactor가 1.0이면 둥근 뭉게구름, 작아지면 옆으로 퍼지는 모루 구름이 됩니다.
//    float anvilFactor = 1.0f;
//    m = pow(abs(m), remap_clamp(1.0f - norY, 0.2f, 0.8f, 1.0f, lerp(1.0f, 0.5f, anvilFactor)));

//    // [적용 2] Coverage 보정 (덧셈 대신 remap_clamp 사용)
//    // CLOUDS_COVERAGE가 작을수록 m의 하한선이 올라가 구름이 지워짐
//    m = remap_clamp(m, CLOUDS_COVERAGE - 1.0f, 1.0f, 0.0f, 1.0f);

//    // [적용 3] 그림자 연산이 아닐 때만 Detail Noise 적용 (isShadow 최적화)
//    if (!isShadow && m > 0.0f)
//    {
//        float detail = GetWorleyNoiseMip(pos, dist);
//        m = remap_clamp(m, detail * CLOUDS_DETAIL_STRENGTH, 1.0f, 0.0f, 1.0f);
//    }

//    //m = smoothstep(0.0f, CLOUDS_BASE_EDGE_SOFTNESS, m + (CLOUDS_COVERAGE - 1.0f));
//    // 하단 소프트니스 및 최종 밀도/거리 감쇄 적용
//    m *= linear_step_org(CLOUDS_BOTTOM_SOFTNESS, norY);
    
//    return clamp(m * CLOUDS_DENSITY * (1.0f + max((pos.x - 7000.0f) * 0.005f, 0.0f)), 0.0f, 1.0f);
//} // ComputeCloudDensity

//float VolumetricShadow(float3 from, float sundotrd, float3 sphereCenter)
//{
//    float dd = CLOUDS_SHADOW_MARGE_STEP_SIZE; // 각 스텝의 크기
//    float3 rd = LIGHT_DIRECTION; // 빛을 향하는 방향
//    float d = dd * 0.5f; // 첫 지점 오프셋
//    float shadow = 1.0f;

//    // 빛 방향으로 짧게 레이마칭
//    for (int s = 0; s < CLOUD_SELF_SHADOW_STEPS; s++)
//    {
//        float3 pos = from + rd * d;
//        float norY = clamp((length(pos - sphereCenter) - (EARTH_RADIUS + CLOUDS_BOTTOM)) / (CLOUDS_TOP - CLOUDS_BOTTOM), 0.0f, 1.0f);

//        // 구름층 천장을 벗어나면 즉시 반환
//        if (norY > 1.0f)
//        {
//            return shadow;
//        }

//        float muE = ComputeCloudDensity(pos, norY);
//        shadow *= exp(-muE * dd); //  Beer-Lambert Law\
//        dd *= CLOUDS_SHADOW_MARGE_STEP_MULTIPLY;
//        d += dd;
//    }

//    return shadow;
//} // VolumetricShadow
