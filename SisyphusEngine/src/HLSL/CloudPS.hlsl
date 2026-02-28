#include "Common.hlsli"
#include "Maths.hlsli"
#define RESOLUTION 1024.0f


Texture2D iNoise : register(t0); // 노이즈 텍스처
Texture2D iBlueNoise : register(t1); // 블루 노이즈
SamplerState iSampler : register(s0); // 샘플러 상태


cbuffer MatrixBuffer : register(b0)
{
    matrix iWorld;
    matrix iView;
    matrix iProj;
    matrix iInvViewProj;
}; // MatrixBuffer

cbuffer CloudBuffer : register(b3)
{
    // Row 1
    float3 iCloudBaseColor;
    float cPadding1;

    // Row 2
    float3 iCloudAmbient;
    float iMaxSteps;

    // Row 3
    float3 iCloudShadowColor;
    float iMarchSize;

    // Row 4
    float iEarthRadius;
    float iCloudStartHeight;
    float iCloudThickness;
    float iNoiseRes;
    
    // Row 5: 밀도 및 물리 감쇄 제어
    float iDensityScale;
    float iFalloffScale; 
    float iMieIntensity;
    float iMiePower;

    // Row 6: 라이팅 디테일
    float iDiffusePower;
    float iLightMultiply;
    float iShadowDist; //그림자 샘플링 거리
    float iMaxDepth; // 최대 가시 거리
    
    // Row 7: FBM 및 애니메이션 제어
    float3 iCloudWindDir;
    float iCloudSpeed;

    // Row 8: FBM 디테일 제어
    float iFbmScale;
    float iFbmFactor;
    float iFbmIncrement;
    float iFbmPersistance;
    
    // Row 9: 최적화 및 기타
    int iFbmOctaves; // 루프 횟수
    float3 cPadding2;
}; // CloudBuffer




// Texture 기반
float GetNoise(float3 x, Texture2D tex, SamplerState samp, float resolution)
{
    float3 p = floor(x);
    float3 f = frac(x);
    
    // Quintic 보간
    f = f * f * (3.0f - 2.0f * f);

    // 3D 좌표를 2D 텍스처 좌표로 투영하는 기법
    float2 uv = (p.xy + float2(37.0f, 239.0f) * p.z) + f.xy;
    float2 rg = tex.SampleLevel(samp, (uv + 0.5f) / resolution, 0.0f).yx;
    return lerp(rg.x, rg.y, f.z) * 2.0f - 1.0f;
} // GetNoise


// FBM (Fractal BrownianMotion)
// 여러 옥타브의 노이즈를 중첩
float fbm(float3 p, float time)
{
    float3 q = p + time * iCloudSpeed * iCloudWindDir;
    
    float f = 0.0f;
    float scale = iFbmScale;
    float factor = iFbmFactor;

    for (int i = 0; i < iFbmOctaves; i++)
    {
        f += scale * GetNoise(q, iNoise, iSampler, iNoiseRes);
        q *= factor;
        factor += iFbmIncrement;
        scale *= iFbmPersistance;
    }

    return f;
} // fbm


// 밀도 정의
float scene(float3 p)
{
    float3 localP = p - iCameraPos;
    float distance = sdSphere(p, iEarthRadius);
    float f = fbm(p, iTime);
    
    return -distance + f;
} // scene


//float planeScene(float3 p)
//{
//    float distance = abs(p.y - iCloudStartHeight) - iCloudThickness;
//    float f = fbm(p, iTime);
    
//    return -distance + f;
//} // planeScene


float planeScene(float3 p)
{
    float heightFromLayer = p.y - iCloudStartHeight;

    float layerMask = saturate(
        1.0 - abs(heightFromLayer) / iCloudThickness
    );

    float noise = fbm(p, iTime); // 스케일 조절

    return layerMask * noise;
} // planeScene


float3 getRayDir(float2 uv)
{
    float2 ndc = uv * 2.0f - 1.0f;

    float4 clip = float4(ndc, 1.0f, 1.0f);
    float4 world = mul(clip, iInvViewProj);
    world /= world.w;

    return normalize(world.xyz - iCameraPos);
} // GetRayDir


float2 intersectLayer(float3 ro, float3 rd)
{
    if (abs(rd.y) < 1e-5)
        return float2(-1.0, -1.0);
    
    float bottom = iCloudStartHeight - iCloudThickness;
    float top = iCloudStartHeight + iCloudThickness;

    float t0 = (bottom - ro.y) / rd.y;
    float t1 = (top - ro.y) / rd.y;

    if (t0 > t1)
    {
        float tmp = t0;
        t0 = t1;
        t1 = tmp;
    }

    return float2(t0, t1);
} // intersectLayer


float4 rayMarch(float3 rayOrigin, float3 rayDirection, float2 uv, float startDepth)
{
    float4 res = float4(0, 0, 0, 0);
    
    // 블루 노이즈 디더링
    float blueNoise = iBlueNoise.Sample(iSampler, uv * (iResolution / RESOLUTION)).r;
    float offset = frac(blueNoise + float(iFrame % 32) / sqrt(0.5));
    
    float depth = startDepth + (iMarchSize * offset);
    float3 sunPos = iLightDirection;
    float3 sunDirection = normalize(sunPos);

    for (int i = 0; i < (int) iMaxSteps; i++)
    {
        float3 p = rayOrigin + depth * rayDirection;
        float density = 0.0f;
        density = planeScene(p);
        
        if (density > 0.0f)
        {
            float densityNearSun = 0.0f;
            densityNearSun = planeScene(p + iShadowDist * sunDirection);
        
            
            // 확산광
            float diffuse = pow(saturate((density - densityNearSun) / iShadowDist), iDiffusePower);

            // 역제곱 법칙(Inverse Square Law)
            float distToSun = length(sunPos - p);
            float falloff = 1.0f / (1.0f + distToSun * iFalloffScale); // 거리에 따른 감쇄 수치

            // Mie Scattering
            float cosTheta = dot(rayDirection, sunDirection);
            float mie = 0.5f + 0.5f * pow(saturate(cosTheta), iMiePower);

            // 최종 조명 색상 조합
            float3 lightEffect = iLightColor.rgb * iIntensity * diffuse * falloff * mie * iLightMultiply;
            
            float3 baseColor = lerp(iCloudBaseColor, iCloudShadowColor, density);
            float3 finalColor = baseColor * (iCloudAmbient + lightEffect);

            // 알파 블렌딩
            float alpha = density * iDensityScale;
            res.rgb += (1.0f - res.a) * finalColor * alpha;
            res.a += (1.0f - res.a) * alpha;
            
            if (res.a > 0.95f)
                break;
        }

        depth += iMarchSize;
        if (depth > iMaxDepth)
            break;
    } // for

    return res;
} // rayMarch


//float4 rayMarch(float3 ro, float3 rd, float2 uv)
//{
//    float4 res = float4(0, 0, 0, 0);

//    float2 tRange = intersectLayer(ro, rd);

//    // 구름 레이어와 교차하지 않으면 종료
//    if (tRange.y < 0.0)
//        return res;

//    float startDepth = max(tRange.x, 0.0);
//    float endDepth = tRange.y;

//    // 블루노이즈 오프셋
//    float blueNoise = iBlueNoise.Sample(iSampler, uv).r;
//    float offset = frac(blueNoise + float(iFrame % 32) * 0.618);

//    float depth = startDepth + iMarchSize * offset;

//    float3 sunDir = normalize(iLightDirection);

//    for (int i = 0; i < (int) iMaxSteps; i++)
//    {
//        if (depth > endDepth)
//            break;

//        float3 p = ro + depth * rd;

//        float density = planeScene(p);

//        if (density > 0.0)
//        {
//            float densityNearSun = planeScene(p + iShadowDist * sunDir);

//            float diffuse = pow(
//                saturate((density - densityNearSun) / iShadowDist),
//                iDiffusePower
//            );

//            float distToSun = length(iLightDirection - p);
//            float falloff = 1.0 / (1.0 + distToSun * iFalloffScale);

//            float cosTheta = dot(rd, sunDir);
//            float mie = 0.5 + 0.5 * pow(saturate(cosTheta), iMiePower);

//            float3 lightEffect =
//                iLightColor.rgb *
//                iIntensity *
//                diffuse *
//                falloff *
//                mie *
//                iLightMultiply;

//            float3 baseColor =
//                lerp(iCloudBaseColor, iCloudShadowColor, saturate(density));

//            float3 finalColor =
//                baseColor * (iCloudAmbient + lightEffect);

//            float alpha = density * iDensityScale;

//            res.rgb += (1.0 - res.a) * finalColor * alpha;
//            res.a += (1.0 - res.a) * alpha;

//            if (res.a > 0.98)
//                break;
//        }

//        depth += iMarchSize;
//    }

//    return res;
//} // rayMarch


float4 main(PixelInput input) : SV_TARGET
{
    // Ray Origin 및 Ray Direction
    float3 ro = iCameraPos;
    float3 rd = normalize(input.worldPos - ro);
    
    //float distToSurface = length(input.worldPos - ro);
    
    //return rayMarch(ro, rd, input.tex, distToSurface);
    
    //float3 rd = getRayDir(input.tex);

    return rayMarch(ro, rd, input.tex, 0.0f);
    //return rayMarch(ro, rd, input.tex);
} // main