# Ghost가 고정되었던 이유에서 존 챔피언 방식으로 고치는 이야기

## Intro: 고스트가 안 사라지는 원인

그냥 포스트 프로세싱인데 기존 렌더타켓 뷰처럼 스크린 영역에 전달할 태양핵, 좌표를 내적, 변환같이 복잡하게 변환하고 HLSL로 전달하는 과정에서 결과가 트였음

이를 위해선 기존 [Shadertoy(Icecool) - musk's lens flare mod](https://www.shadertoy.com/view/XdfXRX) 레퍼런스 코드를 보면

각 고스트나 렌즈플레어 현상의 결과들을 수학적으로 계산하는데 `f0`을 태양에 고정하면 내가 카메라로 움직여도 `f0` 만 고정될줄 알았는데 **고스트까지 고정되어버림**

<details>
<summary> 내가 생각했던 것들 </summary>

### 처음 생각했던 것(파이프라인)

`XMVector3Project` 함수는 수학적으로 3D 공간의 점을 2D 화면에 투영하는데

이때 카메라 뒤에 있는 점도 계산상으로는 화면 좌표로 투영될 수 있음

```cpp
struct LensFlareBuffer {
    // ...

    void Init(const DirectX::XMVECTOR& sunWorldPos,
        const DirectX::XMFLOAT3& camPos,
        const DirectX::XMMATRIX& view,
        const DirectX::XMMATRIX& proj)
    {
        using namespace DirectX;

        // ..

        // 투영 계산
        XMVECTOR farSunPos = camPosVec + (sunDir * 10000.0f);
        XMMATRIX world = XMMatrixIdentity();
        XMVECTOR sunScreenPos = XMVector3Project(farSunPos,
            0, 0, (float)ConstantHelper::SCREEN_WIDTH, (float)ConstantHelper::SCREEN_HEIGHT, 0.0f, 1.0f,
            proj, view, world);
        
        // ...
    } // Init
} // LensFlareBuffer
```

`LensFlareBuffer` 구조체 내 `Init` 함수에서 **"카메라가 바라보는 방향(Forward)"** 과 **"태양 방향"** 의 각도를 계산해버림

**태양이 카메라 뒤에 있다면 강제로 scale을 0으로 만들어버리는 로직(Hard Cut)** 이 반드시 필요하고 셰이더에 상수버퍼들을 업데이트를 계속 해주면 가능할 거라 생각했었음

### 파이프라인이 잘 못 되었던 이유

[Shadertoy(Icecool) - musk's lens flare mod](https://www.shadertoy.com/view/XdfXRX) 레퍼런스를 적용하다 몇가지 문제를 발견

#### 1. 물리적 산란(Atmospheric Scattering)과의 정밀한 결합

- 기존 셰이더토이 방식은 화면의 특정 좌표(UV)를 기반으로 가상의 빛을 그려내지만, 시지프스 엔진은 **SkyBox(큐빅) 내에서 물리적인 빛의 산란을 통해 태양을 표현**

- 단순히 화면 위에 덧그리는 식이 아니라, 엔진 내 실제 광원의 위치와 산란 데이터가 연동되어야 하므로 **자체적인 Post-Process 기반 렌즈 플레어가 필수적이란 생각이 들었음**

##### 2. CPU-GPU 연동 데이터 오버헤드 관리

```cpp
vec3 lensflare(vec2 uv,vec2 pos)
{
	vec2 main = uv-pos;
	vec2 uvd = uv*(length(uv));
	
	float ang = atan(main.y, main.x);
	float dist=length(main); dist = pow(dist,.1);
	float n = noise(vec2((ang-iTime/9.0)*16.0,dist*32.0));
	
	float f0 = 1.0/(length(uv-pos)*16.0+1.0);
	
	f0 = f0+f0*(sin((ang+iTime/18.0 + noise(abs(ang)+n/2.0)*2.0)*12.0)*.1+dist*.1+.8);

	float f2 = max(1.0/(1.0+32.0*pow(length(uvd+0.8*pos),2.0)),.0)*00.25;
	float f22 = max(1.0/(1.0+32.0*pow(length(uvd+0.85*pos),2.0)),.0)*00.23;
	float f23 = max(1.0/(1.0+32.0*pow(length(uvd+0.9*pos),2.0)),.0)*00.21;
	
	vec2 uvx = mix(uv,uvd,-0.5);
	
	float f4 = max(0.01-pow(length(uvx+0.4*pos),2.4),.0)*6.0;
	float f42 = max(0.01-pow(length(uvx+0.45*pos),2.4),.0)*5.0;
	float f43 = max(0.01-pow(length(uvx+0.5*pos),2.4),.0)*3.0;
	
	uvx = mix(uv,uvd,-.4);
	
	float f5 = max(0.01-pow(length(uvx+0.2*pos),5.5),.0)*2.0;
	float f52 = max(0.01-pow(length(uvx+0.4*pos),5.5),.0)*2.0;
	float f53 = max(0.01-pow(length(uvx+0.6*pos),5.5),.0)*2.0;
	
	uvx = mix(uv,uvd,-0.5);
	
	float f6 = max(0.01-pow(length(uvx-0.3*pos),1.6),.0)*6.0;
	float f62 = max(0.01-pow(length(uvx-0.325*pos),1.6),.0)*3.0;
	float f63 = max(0.01-pow(length(uvx-0.35*pos),1.6),.0)*5.0;
	
	vec3 c = vec3(.0);
	
	c.r+=f2+f4+f5+f6; c.g+=f22+f42+f52+f62; c.b+=f23+f43+f53+f63;
	c+=vec3(f0);
	
	return c;
}
```

- 셰이더토이는 모든 연산을 GPU 내에서 수학적으로 처리하려 하지만, 엔진 기반에서는 *카메라 포지션* , *시야각(FOV)* , *태양의 월드 좌표(`f0`)* 등을 **CPU에서 계산하여 상수 버퍼(`Constant Buffer`)**를 통해 넘겨줘야 했음

```cpp
struct LensFlareBuffer {
    // Row 1
    DirectX::XMFLOAT2 sunPos;
    float threshold;
    float scale;
    // Row 2
    float bias;
    float ghostCount;
    float ghostSpacing;
    float distortion;
    // Row 3
    DirectX::XMFLOAT3 tintColor;
    float padding1;
    
    // Row 4
    float visibility;
    DirectX::XMFLOAT3 padding2;
    
    LensFlareBuffer()
    {
        sunPos = { 0.0f, 0.0f };
        threshold = 0.85f;
        scale = 0.0f; // 기본값은 0
        bias = -0.5f;
        ghostCount = 4.0f;
        ghostSpacing = 0.4f;
        distortion = 2.5f;
        tintColor = { 1.0f, 0.9f, 0.7f }; padding1 = 0.0f;
        visibility = 0.0f;
        padding2 = { 0.0f, 0.0f, 0.0f };
        } // LensFlareBuffer
        
        void Init(const DirectX::XMVECTOR& sunWorldPos, const DirectX::XMFLOAT3& camPos, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj)
        { 
            using namespace DirectX; XMMATRIX invView = XMMatrixInverse(nullptr, view);
            XMVECTOR camForward = invView.r[2]; // 카메라가 바라보는 방향
            XMVECTOR camPosVec = XMLoadFloat3(&camPos); // 태양 방향 벡터 (World Space)
            XMVECTOR sunDir = XMVector3Normalize(sunWorldPos);
            // 카메라 전방과 태양 방향의 내적
            // 카메라가 태양을 정면으로 볼 때 1.0, 등지면 -1.0
            
            float dot = XMVectorGetX(XMVector3Dot(camForward, sunDir)); // 가시성 결정 (Threshold)
            
            if (dot < 0.1f)
            {
                this->scale = 0.0f; this->visibility = 0.0f; return;
            } // 정면에 가까울수록 강해지게 설정
            this->visibility = MathHelper::clamp((dot - 0.1f) / 0.9f, 0.0f, 255.0f); // 더 극적인 효과를 위해 제곱
             
            this->visibility *= this->visibility; // 투영 계산
            XMVECTOR farSunPos = camPosVec + (sunDir * 10000.0f); XMMATRIX world = XMMatrixIdentity();

            XMVECTOR sunScreenPos = XMVector3Project(farSunPos, 0, 0, (float)ConstantHelper::SCREEN_WIDTH, (float)ConstantHelper::SCREEN_HEIGHT, 0.0f, 1.0f, proj, view, world);
            
            this->sunPos.x = (XMVectorGetX(sunScreenPos) / (float)ConstantHelper::SCREEN_WIDTH) - 0.5f; this->sunPos.y = (1.0f - (XMVectorGetY(sunScreenPos) / (float)ConstantHelper::SCREEN_HEIGHT)) - 0.5f; this->scale = ConstantHelper::LightIntensity * this->visibility;
    } // Init
}; // LensFlareBuffer
```

- 이 과정에서 코드가 복잡해지고 관리 포인트가 늘어나는 것보다, 이미 렌더링된 **Scene Texture의 휘도(Luminance)**를 추출하여 활용하는 방식이 구조적으로 훨씬 명확하지 않을 까란 생각이 듬

#### 3. 태양 핵(Sun Core)의 실질적 소스화

- 셰이더토이의 공식에서 임의의 점(`f0`)을 설정과 실제 태양핵이 중복되어버리는 문제

- 실제 이를 내가 제어할수 있을거라 생각했지만 마땅한 방안이 떠오르지 않음

</details>

---

## 문제 정리

1. **고스트 현상의 고착화**

    렌즈 플레어는 카메라의 움직임에 따라 유기적으로 변해야 하는데, 수학적으로 고정된 좌표값은 카메라가 회전해도 고스트들이 화면 특정 위치에 박혀 있는(Fixed) 현상을 만들었음

2. **데이터 오버헤드** 

    `XMVector3Project` 를 통해 매 프레임 CPU에서 화면 좌표를 계산하고 상수 버퍼를 업데이트하는 과정은 연산량보다 **관리 포인트(Maintenance Point)를 복잡하게 만들어짐**

3. **물리적 불일치**

    실제 태양 핵은 큐빅 맵 산란에 의해 그려지는데, 렌즈 플레어만 화면 좌표(UV) 기반으로 따로 놀다 보니 시각적인 이질감이 발생

---

## Sol: [존 챔피언(Texture-Based)](https://john-chapman-graphics.blogspot.com/2013/02/pseudo-lens-flare.html) 방식의 채택

구글링 중에 **존 챔피언** 의 렌즈플레어 코드를 알게 되었고 학습해보며 생각이 바뀌게 됌

단순히 수학적 좌표를 따라가는 것이 아닌, **"실제 화면에 그려진 밝은 빛"** 을 소스로 사용하는 방식이 현재 내 상황에 맞다고 판단

### 1. 물리 기반 가시성 판단

단순히 태양의 좌표가 화면 안에 있는지만 체크하는 것이 아니라, 실제 그려진 화면의 `Luminance` 를 분석

```cpp
float calculateSunVisibility()
{
    // 텍스처 좌표 기준 오프셋
    float2 blurOffset = float2(0.003f, 0.003f * iAspect);
    
    float3 samples[5];
    samples[0] = iSceneTex.Sample(iSampler, iSunUV).rgb;
    samples[1] = iSceneTex.Sample(iSampler, iSunUV + float2(blurOffset.x, 0)).rgb;
    samples[2] = iSceneTex.Sample(iSampler, iSunUV - float2(blurOffset.x, 0)).rgb;
    samples[3] = iSceneTex.Sample(iSampler, iSunUV + float2(0, blurOffset.y)).rgb;
    samples[4] = iSceneTex.Sample(iSampler, iSunUV - float2(0, blurOffset.y)).rgb;

    float combinedLuma = 0;
    
    [unroll]
    for (int i = 0; i < 5; i++)
    {
        float luma = getLuminance(samples[i]);
        
        // Threshold 적용
        luma = saturate((luma - iGhostThreshold) / (1.0f - iGhostThreshold));
        
        combinedLuma += luma;
    }
    
    return combinedLuma / 5.0f; // 5점 평균 반환
} // calculateSunVisibility
```

- 5점 샘플링 (`calculateSunVisibility`): 태양 중심과 주변 4개 지점을 샘플링하여 평균 휘도를 계산


```cpp

// 가시성 판단
float checkVisibility(float2 center)
{
    // 블러 및 임계값이 적용된 평균 휘도
    float avgLuma = calculateSunVisibility();

    // 0.1~0.9 구간에서 부드럽게 가시성 결정
    float visibility = smoothstep(0.1f, 0.9f, avgLuma);

    // 화면 외곽 페이드 아웃
    float d = distance(iSunUV, center);
    float edgeFade = 1.0f - smoothstep(0.45f, 0.75f, d);

    return visibility * edgeFade * iGhostAlpha;
} // checkVisibility
```

- 지형 가림 처리: 태양이 산이나 건물 뒤로 숨으면 휘도가 낮아지며, `smoothstep`을 통해 렌즈 플레어가 **부드럽게 감쇠(Fade-out)**

### 2. 존 챔피언 코드 커스텀


1. **회전행렬** : 카메라 회전에 따라 고스트들이 단순 슬라이드가 아닌, 실제 렌즈 내부의 굴절처럼 회전하도록 구현

    ```cpp
    XMFLOAT2 RenderingEngine::CalculateSunUV(const XMMATRIX& view, const XMMATRIX& proj)
    {
        using namespace ConstantHelper;

        XMVECTOR sunWorldPos = XMLoadFloat3(&m_Sun->GetPosition());

        XMVECTOR localSunPos = XMVector3TransformCoord(sunWorldPos, view);
        if (XMVectorGetZ(localSunPos) < 0.0f)
        {
            return XMFLOAT2(-1.0f, -1.0f);
        }

        XMVECTOR sunScreenPos = XMVector3Project(sunWorldPos, 0, 0,
            SCREEN_WIDTH, SCREEN_HEIGHT, 0, 1,
            proj, view, XMMatrixIdentity());

        return XMFLOAT2(
            XMVectorGetX(sunScreenPos) / (float)SCREEN_WIDTH,
            XMVectorGetY(sunScreenPos) / (float)SCREEN_HEIGHT
        );
    } // CalculateSunUV
    ```

    ```cpp
    XMMATRIX RenderingEngine::CalculateLensMatrix(const XMMATRIX& view)
    {
        float m00 = XMVectorGetX(view.r[0]);
        float m01 = XMVectorGetY(view.r[0]);
        float camRot = atan2(m01, m00);

        // UV 회전 행렬 생성 및 전치(Transpose)하여 셰이더용으로 반환
        return XMMatrixTranspose(MathHelper::TransformUVRotationMatrix(camRot));
    }
    ```

    ```cpp
    cbuffer LenFlareBuffer : register(b3)
    {
        int iGhostCount;
        float iGhostSpacing;
        float iGhostThreshold;
        float iGhostAlpha;
    
        float iAspect;
        float iGlowSize;
        float2 iSunUV;
    
        float4x4 iLensMatrix;
    }; // GhostBuffer
    ```

2. 고스트, 할로 연쇄

    ```cpp
    float3 processLensFeatures(float2 uv, float2 center, float2 ghostVec, float2 direction, float3 distortion)
    {
        float3 ghostColor = float3(0, 0, 0);

        [unroll(10)]
        for (int i = 0; i < iGhostCount; i++)
        {
            float2 ghostPos = frac(iSunUV + ghostVec * (float) i);
        
            // 감쇄 계수
            float falloff = 1.0f - ((float) i / (float) iGhostCount);
            float weight = 1.0f - smoothstep(0.0f, 0.85f, distance(ghostPos, center));
        
            float2 offset = ghostPos - uv;

            float3 texElement = float3(0, 0, 0);
            float currentSize = iGlowSize * max(0.4f, falloff);

            if (i == 0) // 태양 바로 옆의 큰 글로우
            {
                float glowSize = iGlowSize * 3.0f;
                float2 glowUV = offset / glowSize + HALF;

                if (all(glowUV >= 0.0f && glowUV <= 1.0f))
                {
                    float3 baseGlow = iGlow.Sample(iSampler, glowUV).rgb * HALF;
        
                    // 할로들을 왜곡하여 중첩
                    float3 h1 = sampleDistorted(iHalo1, glowUV, direction, distortion * 0.5f);
                    float3 h2 = sampleDistorted(iHalo2, glowUV, direction, distortion * 0.8f);
                    float3 h3 = sampleDistorted(iHalo3, glowUV, direction, distortion * 1.2f);
        
                    texElement = baseGlow + (h1 + h2 + h3) * 0.2f;
        
                    // 드리프트 행렬 적용 
                    glowUV = mul(float3(glowUV, 1.0f), (float3x3)iLensMatrix).xy;
                }
            }
            else
            {
                int pattern = i % 4;
            
                if (pattern == 1 || pattern == 2 || pattern == 3)
                {
                    float haloScale = (pattern == 1) ? PATTERN1_SCALE : (pattern == 2 ? PATTERN2_SCALE : PATTERN3_SCALE);
                    currentSize *= haloScale;
                }
                else // 일반 육각 고스트 등
                {
                    currentSize *= 0.25f;
                }
            
                float2 localUV = offset / currentSize + HALF;
                localUV = mul(float3(localUV, 1.0f), (float3x3) iLensMatrix).xy;
                
                if (pattern == 1)
                    texElement = sampleDistorted(iHalo1, localUV, direction, distortion);
                else if (pattern == 2)
                    texElement = sampleDistorted(iHalo2, localUV, direction, distortion);
                else if (pattern == 3)
                    texElement = sampleDistorted(iHalo3, localUV, direction, distortion);
                else
                    texElement = iGhost.Sample(iSampler, localUV).rgb;
                
                texElement *= pow(falloff, 2.5f);
            
            }
            ghostColor += texElement * weight;
        }
    
        return ghostColor;
    } //
    ```
---

## Res

<p align="center">
  <table style="width:100%; text-align:center; border-spacing:20px;">
      <td style="text-align:center; vertical-align:middle;">
        <p align="center">
        <img src="https://github.com/BOLTB0X/DirectX11-Draw/blob/LensFlare/DemoGIF/lensFlare/%EB%A0%8C%EC%A6%88%ED%94%8C%EB%A0%88%EC%96%B400_%EA%B3%A0%EC%8A%A4%ED%8A%B8%EA%B3%A0%EC%A0%95%ED%98%84%EC%83%81.gif?raw=true" 
             alt="image 2" 
             style="width:600px; height:400px; object-fit:contain; border:1px solid #ddd; border-radius:4px;"/>
        </p>
      </td>
      <td style="text-align:center; vertical-align:middle;">
        <p align="center">
        <img src="https://github.com/BOLTB0X/DirectX11-Draw/blob/LensFlare/DemoGIF/lensFlare/%EB%A0%8C%EC%A6%88%ED%94%8C%EB%A0%88%EC%96%B406_drift02.gif?raw=true" 
             alt="image 2" 
             style="width:600px; height:400px; object-fit:contain; border:1px solid #ddd; border-radius:4px;"/>
        </p>
      </td>
    </tr>
    <tr>
      <td style="text-align:center; font-size:14px; font-weight:bold;">
      <p align="center">
      <a></a>기존 셰이더에서 고정</a>
      </p>
      </td>
      <td style="text-align:center; font-size:14px; font-weight:bold;">
      <p align="center">
      <a></a>존 챔피언방식</a>
      </p>
      </td>
    </tr>
  </table>
</p>