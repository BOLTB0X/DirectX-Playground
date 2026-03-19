// https://blog.uhawkvr.com/rendering/rendering-volumetric-clouds-using-signed-distance-fields/
// CloudRayPS.hlsl
cbuffer CameraBuffer : register(b0)
{
    float3 cCameraPosition;
    float  cPadding1;
    matrix cViewInv;
    matrix cProjInv;
}; // CameraBuffer

cbuffer LightBuffer : register(b1)
{
    float4 lDiffuseColor;
    float3 lLightDirection;
    float  lPadding;
}; // LightBuffer

cbuffer CloudBuffer : register(b2)
{
    float3 cPlanetCenter;
    float  cPlanetRadius;
    float  cCloudMinHeight;
    float  cCloudMaxHeight;
    float2 cPadding2;
}; // CloudBuffer

#define RAY_MAX_STEPS 32
#define LIGHT_MAX_STEPS 6
static const float CLOUD_DENSITY_SCALE = 1.5f;
static const float LIGHT_ABSORPTION = 1.0f;
static const float CLOUD_TILING_SCALE = 0.1f;

Texture3D volumeTexture : register(t0);
Texture2D weatherMapTexture : register(t1);
Texture2D depthTexture : register(t2);

SamplerState wrapSampler : register(s0);
SamplerState clampSampler : register(s1);

struct PS_INPUT
{
    float4 suvPos : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    float3 worldPos : POSITION;
}; // PS_INPUT

// 레이와 구체의 교점 계산 함수 (t0: 진입, t1: 퇴출)
bool ray_sphere_Intersection(float3 ro, float3 rd, float3 center, float radius, out float t0, out float t1)
{
    float3 oc = ro - center;
    float b = dot(oc, rd);
    float c = dot(oc, oc) - radius * radius;
    float h = b * b - c;
    if (h < 0.0)
        return false; // 교점 없음
    h = sqrt(h);
    t0 = -b - h;
    t1 = -b + h;
    return true;
} // ray_sphere_Intersection

float HG_phase(float cosTheta, float g)
{
    float g2 = g * g;
    float num = 1.0f - g2;
    float den = 1.0f + g2 - 2.0f * g * cosTheta;
    return (1.0f / (4.0f * 3.141592f)) * num / pow(abs(den), 1.5f);
} // HGPhase

float3 world_To_UVW(float3 pos)
{
    return pos * CLOUD_TILING_SCALE;
} // WorldToUVW

float get_density(float3 pos)
{
    // 3D 텍스처에서 밀도를 샘플링 (R채널 혹은 합성된 값)
    // 여기서 pos는 월드 좌표를 텍스처 UVW 공간(0~1)으로 변환한 값이어야 합니다.
    float3 uvw = world_To_UVW(pos);
    return volumeTexture.SampleLevel(wrapSampler, uvw, 0).r;
} // get_density

float3 calculate_light(float3 rayPos, float cosTheta)
{
    float3 lightDir = normalize(-lLightDirection); // 태양을 향하는 방향
    float lightStepSize = 0.2f; // 빛 마칭 보폭 (km)
    float opticalDepth = 0.0f;

    // 태양을 향해 짧게 레이를 쏴서 구름의 두께(광학적 깊이)를 측정
    for (int i = 0; i < LIGHT_MAX_STEPS; i++)
    {
        float3 lightPos = rayPos + lightDir * (i * lightStepSize);
        opticalDepth += get_density(lightPos) * lightStepSize;
    }

    // Beer's Law (빛의 감쇄)
    float attenuation = exp(-opticalDepth * LIGHT_ABSORPTION);
    
    // 은빛 테두리 (Silver Lining)를 위한 위상 함수 적용
    float phase = HG_phase(cosTheta, 0.5f); // g=0.5: 앞쪽으로 산란이 잘 일어남

    // 주변광(Ambient)을 살짝 더해 구름 아랫부분이 완전 까매지는 것을 방지
    float3 ambient = float3(0.2f, 0.2f, 0.3f);
    
    return lDiffuseColor.rgb * (attenuation * phase * 10.0f) + ambient;
} // calculate_light

float4 raymarch(float3 rayStart, float3 rayDir, float maxDistance, float stepSize)
{
    float4 intScattTrans = float4(0, 0, 0, 1);
    float t = 0;
    
    // 카메라 광선과 태양빛 사이의 각도 (위상 함수용)
    float cosTheta = dot(rayDir, normalize(-lLightDirection));

    for (int u = 0; u < RAY_MAX_STEPS; u++)
    {
        if (t > maxDistance || intScattTrans.a < 0.003f)
            break;

        float3 rayPos = rayStart + rayDir * t;
        float density = get_density(rayPos);

        if (density > 0.0f)
        {
            float extinction = density * CLOUD_DENSITY_SCALE;
            float transmittance = exp(-extinction * stepSize);
            
            float3 luminance = calculate_light(rayPos, cosTheta);

            float3 integScatt = luminance - (luminance * transmittance);
            intScattTrans.rgb += integScatt * intScattTrans.a;
            intScattTrans.a *= transmittance;
        }

        t += stepSize;
    }

    return float4(intScattTrans.rgb, 1.0f - intScattTrans.a);
} // raymarch

float4 debug_check_uv(float3 worldPos)
{
    float3 debugColor = frac(worldPos / 10.0f);
    float weatherScale = 0.01f; // 100km 반복 스케일
    float2 weatherUV = worldPos.xz * weatherScale;
    float weatherData = weatherMapTexture.SampleLevel(wrapSampler, weatherUV, 0).r;
    
    //return float4(frac(weatherUV), 0, 1);
    return float4(weatherData.xxx, 1.0f);
} // debug_check_uv

float4 debug_check_height(float3 worldPos)
{
    // 1. 높이 판정 (평면 지구 기준)
    // worldPos.y가 구름 층(예: 1.5km ~ 4.0km) 사이가 아니라면 그리지 않음
    if (worldPos.y < cCloudMinHeight || worldPos.y > cCloudMaxHeight)
    {
        discard;
    }

    // 2. WeatherMap 샘플링
    float2 weatherUV = worldPos.xz * 0.01f; // 스케일 조절
    float weather = weatherMapTexture.SampleLevel(wrapSampler, weatherUV, 0).r;

    // 기상도 값이 낮은 곳(맑은 하늘)도 그리지 않음
    if (weather < 0.1f)
    {
        discard;
    }

    // 결과 확인용: 구름 층에 해당하는 부분만 하얀색으로 출력
    return float4(weather.xxx, 1.0f);
} // debug_check_height

float4 main(PS_INPUT input) : SV_Target
{
    //return debug_check_uv(input.worldPos);
    //return debug_check_height(input.worldPos);
    float3 ro_km = cCameraPosition / 1000.0f;
    
    // rd는 방향 벡터이므로 정규화(normalize)하면 거리가 제거되고 방향만 남습니다.
    // worldPos와 cCameraPosition 모두 미터(m) 단위이므로 그대로 빼서 정규화하면 됩니다.
    float3 rd = normalize(input.worldPos - cCameraPosition);
    
    float innerRadius_km = cPlanetRadius + (cCloudMinHeight / 1000.0f);
    float outerRadius_km = cPlanetRadius + (cCloudMaxHeight / 1000.0f);

    float tInnerNear, tInnerFar;
    float tOuterNear, tOuterFar;

    // 2. 바깥쪽 구체와 안쪽 구체 교차 테스트 (모두 km 기준)
    bool hitOuter = ray_sphere_Intersection(ro_km, rd, cPlanetCenter, outerRadius_km, tOuterNear, tOuterFar);
    bool hitInner = ray_sphere_Intersection(ro_km, rd, cPlanetCenter, innerRadius_km, tInnerNear, tInnerFar);

    // 바깥 껍질(구름 천장)에 부딪히지 않았거나 구름을 등지고 있다면 그리지 않음
    if (!hitOuter || tOuterFar < 0.0f)
        discard;

    // 3. 구름층이 시작되는 첫 지점 (km)
    float startT = max(0.0f, tOuterNear);
    
    // 구름 껍질에 부딪힌 실제 3D 월드 좌표 (km 단위)
    float3 hitPos_km = ro_km + rd * startT;
    
    // 4. WeatherMap 매핑을 위해 미터(m)로 복구하여 UV 생성
    float3 hitPos_m = hitPos_km * 1000.0f;
    
    // 웨더맵 UV 스케일 (값이 너무 작거나 크면 텍스처가 깨져 보이니 0.00005f 등으로 조절해가며 테스트)
    float2 weatherUV = hitPos_m.xz * 0.00005f;
    float weatherData = weatherMapTexture.SampleLevel(wrapSampler, weatherUV, 0).r;

    // 기상도 값이 너무 낮으면(맑은 구역) 하늘을 보여주기 위해 discard (선택 사항)
    // if (weatherData < 0.05f) discard;

    // 5. 알파값을 1.0f로 설정하여 흑백의 웨더맵 패턴이 화면에 나오도록 반환
    return float4(weatherData.xxx, 1.0f);
} // main