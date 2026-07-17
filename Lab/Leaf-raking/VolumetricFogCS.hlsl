// VolumetricFogCS.hlsl
// 터레인 높이맵 기반 하이트 포그 + 볼류메트릭 라이트 스캐터링
#include "Common.hlsli"
#include "Volumetric.hlsli"
#include "ShadowMap.hlsli"

RWTexture2D<float4>    OutTexture : register(u0);
SamplerState           LinearWrapSampler : register(s0);
SamplerState           PointClampSampler : register(s1);
SamplerComparisonState ShadowSampler : register(s5);

Texture2D<float>       SceneDepth : register(t1);
Texture2D              NormalMap : register(t2);
Texture2D<float>       HeightMap : register(t3);
Texture3D              WorleyNoise : register(t4);
Texture2D              TerrainShadowMap : register(t11);

cbuffer VolumetricFogBuffer : register(b2)
{
    // Row 1: 높이 기반 밀도
    float  fBaseHeight;
    float  fHeightFalloff;
    float  fDensity;
    float  fMaxDistance;
    // Row 2: 노이즈 (듬성듬성)
    float  fNoiseScale;
    float  fNoiseStrength;
    float  fWindSpeed;
    float  fPhaseG;
    // Row 3: 색상
    float3 fColor;
    float  fAmbientStrength;
    // Row 4: 경사 감쇠 + 바람 방향
    float  fSlopeMin;
    float  fSlopeMax;
    float2 fWindDirection;
    // Row 5: 지형 월드 매핑
    float  fTerrainWidth;
    float  fTerrainDepth;
    float  fTerrainHeightScale;
    int    fMarchSteps;
    //
    float4 fPadding;
}; // VolumetricFogBuffer

#define FOG_BASE_HEIGHT      fBaseHeight
#define FOG_HEIGHT_FALLOFF   fHeightFalloff
#define FOG_DENSITY          fDensity
#define FOG_MAX_DISTANCE     fMaxDistance
#define FOG_NOISE_SCALE      fNoiseScale
#define FOG_NOISE_STRENGTH   fNoiseStrength
#define FOG_WIND_SPEED       fWindSpeed
#define FOG_PHASE_G          fPhaseG
#define FOG_COLOR            fColor
#define FOG_AMBIENT_STRENGTH fAmbientStrength
#define FOG_SLOPE_MIN        fSlopeMin
#define FOG_SLOPE_MAX        fSlopeMax
#define FOG_WIND_DIRECTION   fWindDirection
#define TERRAIN_WIDTH        fTerrainWidth
#define TERRAIN_DEPTH        fTerrainDepth
#define TERRAIN_HEIGHT_SCALE fTerrainHeightScale
#define FOG_MARCH_STEPS      fMarchSteps

// 1. 하이트맵 기반 지형 높이 추출
float GetTerrainHeightAt(float3 worldPos)
{
    float2 uv = float2((worldPos.x + TERRAIN_WIDTH * 0.5f) / TERRAIN_WIDTH,
                       (worldPos.z + TERRAIN_DEPTH * 0.5f) / TERRAIN_DEPTH);
    
    uv = saturate(uv);

    float h = HeightMap.SampleLevel(LinearWrapSampler, uv, 0).r;
    return h * TERRAIN_HEIGHT_SCALE;
} // GetTerrainHeightAt

float ComputeFogDensity(float3 worldPos)
{
    // 광선 위치의 실제 지형 높이를 구해 상대적인 고도 계산
    float terrainHeight = GetTerrainHeightAt(worldPos);
    float heightAboveGround = max(worldPos.y - terrainHeight - FOG_BASE_HEIGHT, 0.0f);
    
    // 3D 노이즈 샘플링
    float2 noiseCoord = worldPos.xz * FOG_NOISE_SCALE;
    noiseCoord.x += TIME * FOG_WIND_SPEED;
    float noise = WorleyNoise.SampleLevel(LinearWrapSampler, float3(noiseCoord.x, worldPos.y * FOG_NOISE_SCALE, noiseCoord.y), 0).r;
    
    float expFog = exp(-heightAboveGround * FOG_HEIGHT_FALLOFF) * FOG_DENSITY * noise;

    // 런타임 나누기 오류 방지용 최소값 보장
    return max(expFog, 0.0001f);
}

float GetFogShadow(float3 worldPos)
{
    float4 lightClipPos = mul(mul(float4(worldPos, 1.0f), LIGHT_VIEW), LIGHT_PROJ);
    return calculate_poisson_shadow(ShadowSampler, TerrainShadowMap, lightClipPos,
        SHADOW_MAP_SIZE, SHADOW_SPREAD, SHADOW_BIAS);
}

// 볼륨 레이마칭 핵심 루프
float4 RaymarchFog(float3 ro, float3 rd, float maxDist, uint2 pixelPos)
{
    float3 scatteredLight = float3(0.0f, 0.0f, 0.0f);
    float transmittance = 1.0f;

    if (maxDist <= 0.0f)
        return float4(0.0f, 0.0f, 0.0f, 1.0f);

    float stepSize = maxDist / (float) FOG_MARCH_STEPS;
    
    // 밴딩 현상 완화를 위한 지터링
    float d = stepSize * jittering(float3(pixelPos, TIME));
    
    float cosTheta = dot(rd, -LIGHT_DIRECTION);
    float phaseFunction = henyey_greenstein(cosTheta, FOG_PHASE_G);
    
    // 태양광 및 앰비언트 강도
    float3 sunColor = get_dynamic_light_color(LIGHT_DIRECTION).rgb * 20.0f;
    float3 ambientColor = float3(1.2f, 1.5f, 2.0f) * FOG_AMBIENT_STRENGTH;

    [loop]
    for (int i = 0; i < FOG_MARCH_STEPS; i++)
    {
        float3 p = ro + rd * d;
        
        float density = ComputeFogDensity(p);
        float shadow = GetFogShadow(p);
        
        // 도달한 빛 계산
        float3 lightContrib = (sunColor * phaseFunction * shadow) + ambientColor;
        float currentTransmittance = exp(-density * stepSize);
        
        // 산란광 누적
        float3 currentScattering = lightContrib * density;
        float3 Sint = (currentScattering - currentScattering * currentTransmittance) / density;
        
        scatteredLight += transmittance * Sint * FOG_COLOR;
        transmittance *= currentTransmittance;

        d += stepSize;

        // 투과율이 거의 0이 되면 연산 조기 종료 (최적화)
        if (transmittance <= 0.01f)
        {
            transmittance = 0.0f;
            break;
        }
    }

    return float4(scatteredLight, transmittance);
}

[numthreads(8, 8, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint width, height;
    OutTexture.GetDimensions(width, height);
    if (DTid.x >= width || DTid.y >= height)
        return;

    float2 uv = (float2(DTid.xy) + 0.5f) / float2(width, height);
    float3 ro = CAMERA_POSITION;
    float3 rd = ray_direction_restore(uv, PROJ_INV, VIEW_INV);

    float sceneDepth = SceneDepth.SampleLevel(PointClampSampler, uv, 0).r;
    float dist = FOG_MAX_DISTANCE;

    // 엔진의 라스터라이저 깊이 버퍼를 사용하여 레이마칭 거리 최적화
    if (sceneDepth > 0.0f)
    {
        float3 worldPos = get_world_from_depth(uv, sceneDepth, VIEW_INV, PROJ_INV);
        dist = min(length(worldPos - ro), FOG_MAX_DISTANCE);
    }

    float4 col = RaymarchFog(ro, rd, dist, DTid.xy);

    // 엔진 노말맵을 이용한 Slope 감쇠 로직 적용 (절벽에서는 안개가 옅어짐)
    float3 normal = NormalMap.SampleLevel(PointClampSampler, uv, 0).xyz * 2.0f - 1.0f;
    normal = normalize(normal);
    float slopeFactor = smoothstep(FOG_SLOPE_MIN, FOG_SLOPE_MAX, normal.y);
    
    col.rgb *= lerp(0.3f, 1.0f, slopeFactor);
    col.a = lerp(1.0f, col.a, slopeFactor);

    OutTexture[DTid.xy] = col;
}