## DX11 - ShaderToy 포팅연습

### [SDF Cloud](https://github.com/BOLTB0X/DirectX11-Draw/tree/Sun-and-Cloud/SisyphusEngine)

<p align="center">
  <table style="width:100%; text-align:center; border-spacing:20px;">
    <tr>
        <td style="text-align:center; vertical-align:middle;">
        <p align="center">
        <img src="https://github.com/BOLTB0X/DirectX11-Draw/blob/main/DemoGIF/volumetric-raymarching/%EA%B5%AC%EB%A6%84%EA%B3%BC%ED%83%9C%EC%96%91.png?raw=true" 
             alt="image 2" 
             style="; object-fit:contain; border:1px solid #ddd; border-radius:4px;"/>
        </p>
      </td>
    </tr>
    <tr>
      <td style="text-align:center; font-size:14px; font-weight:bold;">
      <p align="center">
      <a></a>노이즈 텍스처 기반 SDF 레이마칭을 이용한 절차적 Volumetric Cloud 렌더링</a>
      </p>
      </td>
    </tr>
  </table>
</p>

순수 코드로 무엇을 렌더링해볼까 고민하다, 구름의 질감을 표현하는게 상당히 어렵다는 것을 알게 되었음

이 [보랏빛 하늘](https://www.shutterstock.com/ko/image-photo/purple-sky-clouds-backdrop-orange-pink-2678999665?trackingId=19e277b6-eb35-471f-b876-09a679b367e4&listId=searchResults) 사진과 구름과 태양을 렌더링을 도전

- [feature](https://github.com/BOLTB0X/DirectX11-Draw/tree/Sun-and-Cloud/SisyphusEngine#feature): **Ray Marching**, **Volumetric Cloud System** , **Atmospheric Lighting**

-  [자세한 README는 여기 클릭](https://github.com/BOLTB0X/DirectX11-Draw/tree/Sun-and-Cloud/SisyphusEngine#feature)


<details>
<summary> 셰이더 Buffer (Refactoring: Lensflare) </summary>

- `CloudBuffer` 

  ```cpp
  struct CloudBuffer {
    // Row 1
    DirectX::XMFLOAT3 baseColor;
    float iCloudType;

    // Row 2
    DirectX::XMFLOAT3 ambient;
    float maxSteps;

    // Row 3
    DirectX::XMFLOAT3 shadowColor;
    float marchSize;

    // Row 4
    float radius;
    float height;
    float thickness;
    float noiseRes;

    // Row 5
    float densityScale;
    float falloffScale;
    float mieIntensity;
    float miePower;

    // Row 6
    float diffusePower;
    float lightMultiply;
    float shadowDist;
    float maxDepth;

    // Row 7
    DirectX::XMFLOAT3 windDir;
    float cloudSpeed;

    // Row 8
    float fbmScale;
    float fbmFactor;
    float fbmIncrement;
    float fbmPersistance;

    // Row 9
    int fbmOctaves;
    DirectX::XMFLOAT3 padding;

    CloudBuffer(float cloudType)
        : iCloudType(cloudType)
    {
        baseColor = { 1.0f, 1.0f, 1.0f };
        ambient = { 0.2f, 0.15f, 0.3f };
        shadowColor = { 0.4f, 0.4f, 0.5f };
        maxSteps = 100.0f;
        marchSize = 0.08f;

        radius = 2.0f;
        height = 1.0f;
        thickness = 2.0f;
        noiseRes = 256.0f;

        densityScale = 0.4f;
        falloffScale = 0.1f;
        mieIntensity = 3.0f; // 전방 산란 밝기
        miePower = 8.0f; // 전방 산란 날카로움

        diffusePower = 2.0f;
        lightMultiply = 3.0f;
        shadowDist = 0.4f;
        maxDepth = 50.0f;

        windDir = { 1.0f, -0.2f, -1.0f };
        cloudSpeed = 0.5f;

        fbmScale = 0.5f;
        fbmFactor = 2.02f;
        fbmIncrement = 0.21f;
        fbmPersistance = 0.5f;
        fbmOctaves = 6;

        padding = { 0.0f, 0.0f, 0.0f };
    }
  }; // CloudBuffer
  ```

  ```cpp
  Texture2D iNoise : register(t0); // 노이즈 텍스처
  Texture2D iBlueNoise : register(t1); // 블루 노이즈
  SamplerState iSampler : register(s0); // 샘플러 상태


  cbuffer CloudBuffer : register(b3)
  {
    // Row 1
    float3 iCloudBaseColor;
    float iCloudType;

    // Row 2
    float3 iCloudAmbient;
    float iMaxSteps;

    // Row 3
    float3 iCloudShadowColor;
    float iMarchSize;

    // Row 4
    float iRadius;
    float iHeight;
    float iTickness;
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
    float3 iCloudPadding7;
  };
  ```

- `SkyBuffer`

  ```cpp
  struct SkyBuffer {
    // Row 1
    DirectX::XMFLOAT3 topColor;
    float skyExponent;

    // Row 2
    DirectX::XMFLOAT3 horizonColor;
    float sunDistScale;

    // Row 3
    DirectX::XMFLOAT3 lowerColor;
    float sunSize;

    // Row 4
    DirectX::XMFLOAT3 atmosphereColor;
    float wideGlowScale;

    // Row 5
    float sunBloom;
    float sunIntensity;
    float bloomMult;
    float glowMult;

    // Row 6
    float rayFreq;
    float rayTimeScale;
    DirectX::XMFLOAT2 padding;

    SkyBuffer()
    {
        topColor = { 0.05f, 0.1f, 0.3f };
        skyExponent = 0.3f;

        horizonColor = { 0.5f, 0.2f, 0.4f };
        sunDistScale = 0.2f;

        lowerColor = { 0.05f, 0.02f, 0.1f };
        sunSize = 0.005f;

        atmosphereColor = { 1.0f, 0.6f, 0.2f };
        wideGlowScale = 10.0f;

        sunBloom = 80.0f;
        sunIntensity = 0.8f;
        bloomMult = 3.5f;
        glowMult = 0.6f;

        rayFreq = 3.0f;
        rayTimeScale = 0.15f;
        padding = { 0.0f, 0.0f };
    }
  }; // SkyBuffer
  ```

  ```cpp
  cbuffer SkyBuffer : register(b3)
  {
    // Row 1: 하늘 기본 색상
    float3 iSkyTopColor;
    float iSkyExponent;

    // Row 2: 지평선 및 태양 거리 스케일
    float3 iSkyHorizonColor;
    float iSunDistScale;

    // Row 3: 하단 색상 및 태양 크기
    float3 iSkyLowerColor;
    float iSunSize;

    // Row 4: 태양 산란 색상 및 범위 제어
    float3 iAtmosphereColor;
    float iWideGlowScale;

    // Row 5: 태양 강도 및 감쇄 속성
    float iSunBloom;
    float iSunIntensity;
    float iBloomMult;
    float iGlowMult;

    // Row 6: 레이 및 시간 속성
    float iRayFreq;
    float iRayTimeScale;
    float2 iPadding;
  }; // SkyBuffer
  ```

- [`CloudPS.hlsl`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/HLSL/CloudPS.hlsl)

- [`SkyPS.hlsl`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/HLSL/SkyPS.hlsl)

- [`RenderingEngine.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/RenderingEngine.cpp)

</details>

<details>
<summary> more 영상 및 설명 </summary>

<p align="center">
  <table style="width:100%; text-align:center; border-spacing:20px;">
    <tr>
        <td style="text-align:center; vertical-align:middle;">
        <p align="center">
        <img src="https://github.com/BOLTB0X/Graphics-Draw/blob/main/DemoGIF/volumetric-raymarching/%EB%B3%BC%EB%A5%A8%EB%A7%88%EC%B9%AD05_%EA%B5%AC%EB%A6%84%EA%B3%BC%ED%83%9C%EC%96%91.gif?raw=true" 
             alt="image 2" 
             style="width:600px; height:400px; object-fit:contain; border:1px solid #ddd; border-radius:4px;"/>
        </p>
      </td>
      <td style="text-align:center; vertical-align:middle;">
        <p align="center">
        <img src="https://github.com/BOLTB0X/Graphics-Draw/blob/main/DemoGIF/volumetric-raymarching/%EB%B3%BC%EB%A5%A8%EB%A7%88%EC%B9%AD05_%EA%B5%AC%EB%A6%84%EA%B3%BC%ED%83%9C%EC%96%912.gif?raw=true" 
             alt="image 2" 
             style="width:600px; height:400px; object-fit:contain; border:1px solid #ddd; border-radius:4px;"/>
        </p>
      </td>
    </tr>
    <tr>
      <td style="text-align:center; font-size:14px; font-weight:bold;">
      <p align="center">
      <a></a>구름 하나</a>
      </p>
      </td>
      <td style="text-align:center; font-size:14px; font-weight:bold;">
      <p align="center">
      <a></a>구름 정면</a>
      </p>
      </td>
    </tr>
  </table>
</p>

<p align="center">
  <table style="width:100%; text-align:center; border-spacing:20px;">
    <tr>
        <td style="text-align:center; vertical-align:middle;">
        <p align="center">
        <img src="https://github.com/BOLTB0X/Graphics-Draw/blob/main/DemoGIF/volumetric-raymarching/%EB%B3%BC%EB%A5%A8%EB%A7%88%EC%B9%AD06_%EA%B5%AC%EB%A6%84%EC%A7%80%EB%8C%80%EC%99%80%ED%83%9C%EC%96%913.gif?raw=true" 
             alt="image 2" 
             style="width:600px; height:400px; object-fit:contain; border:1px solid #ddd; border-radius:4px;"/>
        </p>
      </td>
      <td style="text-align:center; vertical-align:middle;">
        <p align="center">
        <img src="https://github.com/BOLTB0X/Graphics-Draw/blob/main/DemoGIF/volumetric-raymarching/%EB%B3%BC%EB%A5%A8%EB%A7%88%EC%B9%AD06_%EA%B5%AC%EB%A6%84%EC%A7%80%EB%8C%80%EC%99%80%ED%83%9C%EC%96%914%EC%9D%B4%EB%8F%99.gif?raw=true" 
             alt="image 2" 
             style="width:600px; height:400px; object-fit:contain; border:1px solid #ddd; border-radius:4px;"/>
        </p>
      </td>
    </tr>
    <tr>
      <td style="text-align:center; font-size:14px; font-weight:bold;">
      <p align="center">
      <a></a>구름 지대 1</a>
      </p>
      </td>
      <td style="text-align:center; font-size:14px; font-weight:bold;">
      <p align="center">
      <a></a>구름 지대 2</a>
      </p>
      </td>
    </tr>
  </table>
</p>

- [시행착오 및 기록물들](https://github.com/BOLTB0X/DirectX11-Draw/tree/main/DemoGIF/volumetric-raymarching)

- [자세한 `src` 폴더 구조 및 코드 설명](https://github.com/BOLTB0X/DirectX11-Draw/tree/Sun-and-Cloud/SisyphusEngine#src)

    - [파이프라인 - 그래픽](https://github.com/BOLTB0X/DirectX11-Draw/blob/Sun-and-Cloud/SisyphusEngine/src/RederingEngine/RenderingEngine.cpp)

    - [파이프라인 - etc](https://github.com/BOLTB0X/DirectX11-Draw/blob/Sun-and-Cloud/SisyphusEngine/src/MainEngine/MainEngine.cpp)

    - [Cloud HLSL 코드](https://github.com/BOLTB0X/DirectX11-Draw/blob/Sun-and-Cloud/SisyphusEngine/src/HLSL/CloudPS.hlsl)

    - [Sky HLSL 코드](https://github.com/BOLTB0X/DirectX11-Draw/blob/Sun-and-Cloud/SisyphusEngine/src/HLSL/SkyPS.hlsl)

</details>

<details>
<summary> Ref </summary>

- [maximeheckel - Three.js : Real-time dreamy Cloudscapes with Volumetric Raymarching](https://blog.maximeheckel.com/posts/real-time-cloudscapes-with-volumetric-raymarching/)

- [42yeah - Raymarching Clouds](https://blog.42yeah.is/rendering/2023/02/11/clouds.html)

- [Chris' Graphics Blog - Volumetric Rendering](https://wallisc.github.io/rendering/2020/05/02/Volumetric-Rendering-Part-1.html)

- [h3r3 - shadertoy : Sunset on the sea](https://www.shadertoy.com/view/4dl3zr)

- [Mythical - godotshaders : Cloud material](https://godotshaders.com/shader/cloud-material/)

- [shff - github : OpenGL Sky](https://github.com/shff/opengl_sky)


</details>

---

### [LensFlare](https://github.com/BOLTB0X/DirectX11-Draw/tree/LensFlare/SisyphusEngine)

<p align="center">
  <table style="width:100%; text-align:center; border-spacing:20px;">
    <tr>
        <td style="text-align:center; vertical-align:middle;">
        <p align="center">
        <img src="https://github.com/BOLTB0X/DirectX11-Draw/blob/main/DemoGIF/lensFlare/%EB%A0%8C%EC%A6%88%ED%94%8C%EB%A0%88%EC%96%B4.png?raw=true" 
             alt="image 2" 
             style="; object-fit:contain; border:1px solid #ddd; border-radius:4px;"/>
        </p>
      </td>
        <td style="text-align:center; vertical-align:middle;">
        <p align="center">
        <img src="https://github.com/BOLTB0X/DirectX11-Draw/blob/main/DemoGIF/lensFlare/%EB%A0%8C%EC%A6%88%ED%94%8C%EB%A0%88%EC%96%B407_%EC%A0%88%EC%B0%A8%EC%A0%8100.png?raw=true" 
             alt="image 2" 
             style="; object-fit:contain; border:1px solid #ddd; border-radius:4px;"/>
        </p>
      </td>
    </tr>
    <tr>
      <td style="text-align:center; font-size:14px; font-weight:bold;">
      <p align="center">
      <a></a>texture LensFlare</a>
      </p>
      </td>
            <td style="text-align:center; font-size:14px; font-weight:bold;">
      <p align="center">
      <a></a>noise LensFlare</a>
      </p>
      </td>
    </tr>
  </table>
</p>

렌즈 플레어 고정된 현상을 해결을 위해,

[John Chapman의 Pseudo LensFlare](https://john-chapman-graphics.blogspot.com/2013/02/pseudo-lens-flare.html) 과 [shadertoy - musk's lens flare mod(icecool)](https://www.shadertoy.com/view/XdfXRX) 을 기반으로 렌즈플레어 적용

- feature: **Ghost-Halo-Glow Generation** , **Falloff** , **Visibility Check** , **LensDrift**

- [Texture 기반 렌즈플레어의 자세한 README는 여기 클릭](https://github.com/BOLTB0X/DirectX11-Draw/tree/LensFlare/SisyphusEngine#feature)

- [Noise 기반 procedural 렌즈플레어의 자세한 README는 여기 클릭](https://github.com/BOLTB0X/DirectX11-Draw/tree/Procedural_LensFlare/SisyphusEngine)


<details>
<summary> LenFlare 셰이더 Buffer / 렌더링 파이프라인 </summary>

- **Noise 기반 procedural 렌즈플레어**

  ```cpp
  struct LenFlareBuffer {
      // Row 1: 기본 고스트 제어
      int   count;
      float spacing;
      float threshold;
      float alpha;

      // Row 2: 태양 위치 및 기본 글로우
      DirectX::XMFLOAT2 sunUV;
      float glowSize;
      float starScale;

      // Row 3: 고스트 물리 속성 및 태양 코어
      float ghostPull;
      float ghostIntensity;
      float ghostFalloff;
      float sunCoreTightness;

      // Row 4: 왜곡 및 휘도
      DirectX::XMFLOAT3 distortion;
      float padding1;

      // Row 5: 휘도 기준
      DirectX::XMFLOAT3 luminance;
      float padding2;

      // Row 6: F2 설정 (Offset + Sharpness)
      DirectX::XMFLOAT3 f2Offset;
      float f2Sharpness;

      // Row 7: F2 색상
      DirectX::XMFLOAT3 f2ColorMult;
      float padding3;

      // Row 8: F4 설정 (Offset + Power)
      DirectX::XMFLOAT3 f4Offset;
      float f4Power;

      // Row 9: F4 색상
      DirectX::XMFLOAT3 f4ColorMult;
      float padding4;

      // Row 10: F5 설정 (Offset + Power)
      DirectX::XMFLOAT3 f5Offset;
      float f5Power;

      // Row 11: F5 색상
      DirectX::XMFLOAT3 f5ColorMult;
      float padding5;

      // Row 12: F6 설정 (Offset + Power)
      DirectX::XMFLOAT3 f6Offset;
      float f6Power;

      // Row 13: F6 색상
      DirectX::XMFLOAT3 f6ColorMult;
      float padding6;

      // Row 14~17: 행렬
      DirectX::XMMATRIX lensMatrix;

      LenFlareBuffer()
      {
          // 기본 제어
          count = 8;
          spacing = 0.25f;
          threshold = 0.9f;
          alpha = 1.0f;

          // 태양 관련
          sunUV = { 0.5f, 0.5f };
          glowSize = (float)ConstantHelper::SCREEN_WIDTH / (float)ConstantHelper::SCREEN_HEIGHT;
          starScale = 0.8f;

          // 고스트 속성 (#define 값들 이식)
          ghostPull = 0.1f;
          ghostIntensity = 1.5f;
          ghostFalloff = 1.0f;
          sunCoreTightness = 36.0f;

          distortion = { -0.005f, 0.0f, 0.005f };
          luminance = { 0.3f, 0.59f, 0.11f };

          // F2 파라미터
          f2Offset = { 0.80f, 0.85f, 0.90f };
          f2Sharpness = 32.0f;
          f2ColorMult = { 0.25f, 0.23f, 0.21f };

          // F4 파라미터
          f4Offset = { 0.40f, 0.45f, 0.50f };
          f4Power = 2.4f;
          f4ColorMult = { 6.0f, 5.0f, 3.0f };

          // F5 파라미터
          f5Offset = { 0.20f, 0.40f, 0.60f };
          f5Power = 5.5f;
          f5ColorMult = { 2.0f, 2.0f, 2.0f };

          // F6 파라미터
          f6Offset = { -0.3f, -0.325f, -0.35f };
          f6Power = 1.6f;
          f6ColorMult = { 6.0f, 3.0f, 5.0f };

          // 패딩 초기화
          padding1 = padding2 = padding3 = padding4 = padding5 = padding6 = 0.0f;

          lensMatrix = DirectX::XMMatrixIdentity();
      }
  }; // LenFlareBuffer
  ```

  ```cpp
  // Noise 기반 procedural 렌즈플레어
  cbuffer LenFlareBuffer : register(b3)
  {
      // Row 1: 기본 고스트 제어
      int iGhostCount; // 고스트 개수
      float iGhostSpacing; // 고스트 간격
      float iGhostThreshold; // 밝기 임계값
      float iGhostAlpha; // 전체 투명도

      // Row 2: 태양 위치 및 기본 글로우
      float2 iSunUV; // 태양의 Screen UV
      float iGlowSize; // 태양 주변 글로우 크기
      float iStarScale;

      // Row 3: 고스트 물리 속성 및 태양 코어
      float iGhostPull;
      float iGhostIntensity;
      float iGhostFalloff;
      float iSunCoreTight;

      // Row 4: 왜곡 및 휘도
      float3 iDistortion;
      float iPadding1;

      // Row 5: 휘도 기준
      float3 iLuminance;
      float iPadding2;

      // Row 6: F2 설정
      float3 iF2Offset;
      float iF2Sharpness;

      // Row 7: F2 색상
      float3 iF2ColorMult;
      float iPadding3;

      // Row 8: F4 설정
      float3 iF4Offset;
      float iF4Power;

      // Row 9: F4 색상
      float3 iF4ColorMult;
      float iPadding4;

      // Row 10: F5 설정
      float3 iF5Offset;
      float iF5Power;

      // Row 11: F5 색상
      float3 iF5ColorMult;
      float iPadding5;

      // Row 12: F6 설정
      float3 iF6Offset;
      float iF6Power;

      // Row 13: F6 색상
      float3 iF6ColorMult;
      float iPadding6;

      // Row 14~17: 행렬
      float4x4 iLensMatrix;
  }; // LenFlareBuffer
  ```

- **Texture 기반 렌즈플레어**

  ```cpp
  struct LenFlareBuffer {
	  // Row 1
    int count;
    float spacing;
    float threshold;
    float alpha;
	  // Row 2
    float glowSize;
    float aspectRatio;
	  DirectX::XMFLOAT2 sunUV;
	  // Row 3
    DirectX::XMMATRIX lensMatrix;

    LenFlareBuffer()
    {
        count = 10;
        spacing = 0.5f;
        threshold = 0.8f;
        alpha = 1.0f;

		    glowSize = 0.2f;
		    aspectRatio = (float)ConstantHelper::SCREEN_WIDTH / (float)ConstantHelper::SCREEN_HEIGHT;
        sunUV = { 0.5f, 0.5f };

        lensMatrix = DirectX::XMMatrixIdentity();
    } 
  }; // LenFlareBuffer
  ```

  ```cpp
  Texture2D iSceneTex : register(t0);
  Texture2D iGhost : register(t1);
  Texture2D iGlow : register(t2);
  Texture2D iHalo1 : register(t3);
  Texture2D iHalo2 : register(t4);
  Texture2D iHalo3 : register(t5);
  Texture2D iStar : register(t6);
  Texture2D iDepthTex : register(t7);
  SamplerState iSampler : register(s0);


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

- [Noise 기반 procedural `LensFlarePS.hlsl`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/HLSL/LensFlarePS.hlsl)

- [Texture 기반 `LensFlarePS.hlsl`](https://github.com/BOLTB0X/DirectX11-Draw/blob/LensFlare/SisyphusEngine/src/HLSL/LensFlarePS.hlsl)

- [`RenderingEngine.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/RenderingEngine.cpp)

</details>

<details>
<summary> more 영상 및 설명 </summary>

<p align="center">
  <table style="width:100%; text-align:center; border-spacing:20px;">
      <td style="text-align:center; vertical-align:middle;">
        <p align="center">
        <img src="https://github.com/BOLTB0X/DirectX11-Draw/blob/main/DemoGIF/lensFlare/%EB%A0%8C%EC%A6%88%ED%94%8C%EB%A0%88%EC%96%B404_Distorted02.png?raw=true" 
             alt="image 2" 
             style="width:600px; height:400px; object-fit:contain; border:1px solid #ddd; border-radius:4px;"/>
        </p>
      </td>
      <td style="text-align:center; vertical-align:middle;">
        <p align="center">
        <img src="https://github.com/BOLTB0X/DirectX11-Draw/blob/main/DemoGIF/lensFlare/%EB%A0%8C%EC%A6%88%ED%94%8C%EB%A0%88%EC%96%B406_drift02.gif?raw=true" 
             alt="image 2" 
             style="width:600px; height:400px; object-fit:contain; border:1px solid #ddd; border-radius:4px;"/>
        </p>
      </td>
    </tr>
    <tr>
      <td style="text-align:center; font-size:14px; font-weight:bold;">
      <p align="center">
      <a></a>Texture 기반(정면)</a>
      </p>
      </td>
      <td style="text-align:center; font-size:14px; font-weight:bold;">
      <p align="center">
      <a></a>Texture 기반(Drift)</a>
      </p>
      </td>
    </tr>
  </table>
</p>

<p align="center">
  <table style="width:100%; text-align:center; border-spacing:20px;">
      <td style="text-align:center; vertical-align:middle;">
        <p align="center">
        <img src="https://github.com/BOLTB0X/DirectX11-Draw/blob/main/DemoGIF/lensFlare/%EB%A0%8C%EC%A6%88%ED%94%8C%EB%A0%88%EC%96%B407_%EC%A0%88%EC%B0%A8%EC%A0%8102.gif?raw=true" 
             alt="image 2" 
             style="width:600px; height:400px; object-fit:contain; border:1px solid #ddd; border-radius:4px;"/>
        </p>
      </td>
      <td style="text-align:center; vertical-align:middle;">
        <p align="center">
        <img src="https://github.com/BOLTB0X/DirectX11-Draw/blob/main/DemoGIF/lensFlare/%EB%A0%8C%EC%A6%88%ED%94%8C%EB%A0%88%EC%96%B407_%EC%A0%88%EC%B0%A8%EC%A0%8104_%EC%8B%9C%EC%95%BC.gif?raw=true" 
             alt="image 2" 
             style="width:600px; height:400px; object-fit:contain; border:1px solid #ddd; border-radius:4px;"/>
        </p>
      </td>
    </tr>
    <tr>
      <td style="text-align:center; font-size:14px; font-weight:bold;">
      <p align="center">
      <a></a>Noise 기반 procedural(Drift)</a>
      </p>
      </td>
      <td style="text-align:center; font-size:14px; font-weight:bold;">
      <p align="center">
      <a></a>Noise 기반 procedural(vision)</a>
      </p>
      </td>
    </tr>
  </table>
</p>

- [시행착오 및 기록물들](https://github.com/BOLTB0X/DirectX11-Draw/tree/main/DemoGIF/lensFlare)

- [Texture 기반 자세한 `src` 폴더 구조 및 코드 설명](https://github.com/BOLTB0X/DirectX11-Draw/tree/LensFlare/SisyphusEngine/src)

    - [파이프라인 - 그래픽](https://github.com/BOLTB0X/DirectX11-Draw/blob/LensFlare/SisyphusEngine/src/RederingEngine/RenderingEngine.cpp)

    - [파이프라인 - etc](https://github.com/BOLTB0X/DirectX11-Draw/blob/LensFlare/SisyphusEngine/src/MainEngine/MainEngine.cpp)

    - [LensFlare Pixel Shader](https://github.com/BOLTB0X/DirectX11-Draw/blob/LensFlare/SisyphusEngine/src/HLSL/LensFlarePS.hlsl)

    - [셰이더 버퍼 정의(C++)](https://github.com/BOLTB0X/DirectX11-Draw/blob/LensFlare/SisyphusEngine/src/RederingEngine/Shader/ShaderBuffers.h)

- [Noise 기반 procedural 자세한 `src` 폴더 구조 및 코드 설명](https://github.com/BOLTB0X/DirectX11-Draw/tree/Procedural_LensFlare/SisyphusEngine/src)

    - [파이프라인 - 그래픽](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/RenderingEngine.cpp)

    - [파이프라인 - etc](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/MainEngine/MainEngine.cpp)

    - [LensFlare Pixel Shader](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/HLSL/LensFlarePS.hlsl)

    - [셰이더 버퍼 정의(C++)](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Shader/ShaderBuffers.h)


</details>


<details>
<summary> Ref </summary>

- [John Chapman Graphics 블로그 - Pseudo LensFlare](https://john-chapman-graphics.blogspot.com/2013/02/pseudo-lens-flare.html)

- [ShaderToy - musk's lens flare mod(Icecool)](https://www.shadertoy.com/view/XdfXRX)

- [therealmjp - Stairway To (Programmable Sample Point) Heaven](https://therealmjp.github.io/posts/programmable-sample-points/)

- [pngtree - 이미지 사용](https://pngtree.com/so/lens-flare)

</details>

---

## [Water](https://github.com/BOLTB0X/DirectX11-Draw/tree/Water-Basic/SisyphusEngine)

<p align="center">
  <table style="width:100%; text-align:center; border-spacing:20px;">
    <tr>
        <td style="text-align:center; vertical-align:middle;">
        <p align="center">
        <img src="https://github.com/BOLTB0X/DirectX11-Draw/blob/main/DemoGIF/water/%EB%AC%BC03_%EB%B0%B0%EA%B2%BD03_%EC%83%89%EC%83%81%EB%B3%80%EA%B2%BD05.gif?raw=true" 
             alt="image 2" 
             style="; object-fit:contain; border:1px solid #ddd; border-radius:4px;"/>
        </p>
    </tr>
    <tr>
      <td style="text-align:center; font-size:14px; font-weight:bold;">
      <p align="center">
      <a></a>노말맵 왜곡을 통한 Water 물결</a>
      </p>
      </td>
    </tr>
  </table>
</p>

두 개의 노말맵을 서로 다른 속도로 **이동(UV Animation)** 시키고, 이를 합성하여 얻은 벡터값으로 **굴절(Refraction)** 과 **반사(Reflection)** 를 왜곡시키는 전형적인 실시간 **Water** 렌더링 기법

- feature: **Dual Normal Animation** , **Refraction & Reflection Distortion** , **LightColumn** , **Specular & Sparkle** , **Alpha Blending**

- [자세한 README는 여기 클릭](https://github.com/BOLTB0X/DirectX11-Draw/tree/Water-Basic/SisyphusEngine)


<details>
<summary> Water 셰이더 Buffer / 노말맵 왜곡 </summary>

```cpp
struct WaterBuffer {
    // Row 1: 기본 색상 및 기본 변환
    DirectX::XMFLOAT3 waterBaseColor;
    float waterTranslation;

    // Row 2: 왜곡 및 파도 설정
    float reflectRefractScale;
    float waveLength;
    float specularShininess;
    float waterAlpha;

    // Row 3: 바람 설정
    DirectX::XMFLOAT2 windDirection;
    float windForce;
    float finalAlpha;

    // Row 4:스펙큘러 상세 제어
    float highlightsSize; // 하이라이트 크기
    float sunColumnWidth; // 빛 기둥 너비
    float sunColumnInensity; // 빛 기둥 강도
    float sparkleIntensity; // 물결 자글거림 강도

    WaterBuffer() {
        waterBaseColor = { 0.1f, 0.15f, 0.2f };
        waterTranslation = 0.0f;
        reflectRefractScale = 0.03f;
        waveLength = 0.5f;
        specularShininess = 200.0f;
        waterAlpha = 0.6f;
        windDirection = { 1.0f, 0.5f };
        windForce = 0.5f;
        finalAlpha = 0.9f;

        highlightsSize = 0.05f;
        sunColumnWidth = 30.0f;
        sunColumnInensity = 0.5f;
        sparkleIntensity = 0.1f;
    }
}; // WaterBuffer
```

```cpp
cbuffer WaterBuffer : register(b4)
{
    // Row 1
    float3 iWaterBaseColor;
    float iWaterTranslation;
    
    // Row 2
    float iReflectRefractScale;
    float iWaveLength;
    float iSpecularShininess;
    float iWaterAlpha;
    
    // Row 3
    float2 iWindDirection;
    float iWindForce;
    float iFinalAlpha;
    
    // Row 4
    float iHighlightsSize;
    float iSunColumnWidth;
    float iSunColumnIntensity;
    float iSparkleIntensity;
}; // WaterBuffer
```

```cpp
// 노말 애니메이션 
float3 waterNormal(float2 texCoord)
{
    // 바람 적용
    float2 scaledUV = texCoord / iWaveLength;
    float2 moveOffset = normalize(iWindDirection) * (iWaterTranslation * iWindForce);

    float2 uv1 = scaledUV + moveOffset;
    float2 uv2 = scaledUV - (moveOffset * 0.5f) + float2(0.1f, 0.1f);
    
    float3 n1 = normalTexture.Sample(SampleType, uv1).xyz * 2.0f - 1.0f;
    float3 n2 = normalTexture.Sample(SampleType, uv2).xyz * 2.0f - 1.0f;
    
    return normalize(n1 + n2);
} // waterNormal
```
```cpp
// 노말 애니메이션 
float4 main(WaterPixelInput input) : SV_TARGET
{
    // ....

    // 노멀 및 왜곡
    float3 normal = waterNormal(input.tex);
    float2 distortion = normal.xy * iReflectRefractScale;

    reflectUV = clamp(reflectUV + distortion, 0.001f, 0.999f);
    refractUV = clamp(refractUV + distortion, 0.001f, 0.999f);

    // 텍스처 샘플링
    float4 reflectionColor = reflectionTexture.Sample(SampleType, reflectUV);
    float4 refractionColor = refractionTexture.Sample(SampleType, refractUV);

    // ...
}
```

- [`WaterPS.hlsl`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/HLSL/WaterPS.hlsl)

- [`RenderingEngine.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/RenderingEngine.cpp)

- [`ShaderBuffersManager`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Shader/ShaderBuffersManager.h)

</details>

<details>
<summary> more 영상 및 설명 </summary>

<p align="center">
  <table style="width:100%; text-align:center; border-spacing:20px;">
      <td style="text-align:center; vertical-align:middle;">
        <p align="center">
        <img src="https://github.com/BOLTB0X/DirectX11-Draw/blob/main/DemoGIF/water/%EB%AC%BC02_%EB%AC%BC%EA%B2%B002_%EB%AC%BC%EC%95%88%EC%97%90%EC%84%9C%EB%B3%B4%EB%8A%94.gif?raw=true" 
             alt="image 2" 
             style="width:600px; height:400px; object-fit:contain; border:1px solid #ddd; border-radius:4px;"/>
        </p>
      </td>
      <td style="text-align:center; vertical-align:middle;">
        <p align="center">
        <img src="https://github.com/BOLTB0X/DirectX11-Draw/blob/main/DemoGIF/water/%EB%AC%BC04_windTest.gif?raw=true" 
             alt="image 2" 
             style="width:600px; height:400px; object-fit:contain; border:1px solid #ddd; border-radius:4px;"/>
        </p>
      </td>
    </tr>
    <tr>
      <td style="text-align:center; font-size:14px; font-weight:bold;">
      <p align="center">
      <a></a>물안에서 보는</a>
      </p>
      </td>
      <td style="text-align:center; font-size:14px; font-weight:bold;">
      <p align="center">
      <a></a>wind</a>
      </p>
      </td>
    </tr>
  </table>
</p>

<p align="center">
  <table style="width:100%; text-align:center; border-spacing:20px;">
      <td style="text-align:center; vertical-align:middle;">
        <p align="center">
        <img src="https://github.com/BOLTB0X/DirectX11-Draw/blob/main/DemoGIF/water/%EB%AC%BC04_%EC%95%8C%ED%8C%8C_%ED%85%8C%EC%8A%A4%ED%8A%B8.gif?raw=true" 
             alt="image 2" 
             style="width:600px; height:400px; object-fit:contain; border:1px solid #ddd; border-radius:4px;"/>
        </p>
      </td>
      <td style="text-align:center; vertical-align:middle;">
        <p align="center">
        <img src="https://github.com/BOLTB0X/DirectX11-Draw/blob/main/DemoGIF/water/%EB%AC%BC04_waveTest.gif?raw=true" 
             alt="image 2" 
             style="width:600px; height:400px; object-fit:contain; border:1px solid #ddd; border-radius:4px;"/>
        </p>
      </td>
    </tr>
    <tr>
      <td style="text-align:center; font-size:14px; font-weight:bold;">
      <p align="center">
      <a></a>Alpha</a>
      </p>
      </td>
      <td style="text-align:center; font-size:14px; font-weight:bold;">
      <p align="center">
      <a></a>wave</a>
      </p>
      </td>
    </tr>
  </table>
</p>

<p align="center">
  <table style="width:100%; text-align:center; border-spacing:20px;">
      <td style="text-align:center; vertical-align:middle;">
        <p align="center">
        <img src="https://github.com/BOLTB0X/DirectX11-Draw/blob/main/DemoGIF/water/%EB%AC%BC04_%EC%84%A0%EA%B8%B0%EB%91%A5_%ED%85%8C%EC%8A%A4%ED%8A%B8.gif?raw=true" 
             alt="image 2" 
             style="width:600px; height:400px; object-fit:contain; border:1px solid #ddd; border-radius:4px;"/>
        </p>
      </td>
      <td style="text-align:center; vertical-align:middle;">
        <p align="center">
        <img src="https://github.com/BOLTB0X/DirectX11-Draw/blob/main/DemoGIF/water/%EB%AC%BC04_SpecularTest.gif?raw=true" 
             alt="image 2" 
             style="width:600px; height:400px; object-fit:contain; border:1px solid #ddd; border-radius:4px;"/>
        </p>
      </td>
    </tr>
    <tr>
      <td style="text-align:center; font-size:14px; font-weight:bold;">
      <p align="center">
      <a></a>LightColumn</a>
      </p>
      </td>
      <td style="text-align:center; font-size:14px; font-weight:bold;">
      <p align="center">
      <a></a>Specular</a>
      </p>
      </td>
    </tr>
  </table>
</p>

- [시행착오 및 기록물들](https://github.com/BOLTB0X/DirectX11-Draw/tree/main/DemoGIF/water)

- [자세한 `src` 폴더 구조 및 코드 설명](https://github.com/BOLTB0X/DirectX11-Draw/tree/Water-Basic/SisyphusEngine/src)

    - [파이프라인 - 그래픽](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/RenderingEngine.cpp)

    - [파이프라인 - etc](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/MainEngine/MainEngine.cpp)

    - [Water Vertex Shader](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/HLSL/WaterVS.hlsl)

    - [Water Pixel Shader](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/HLSL/WaterPS.hlsl)

    - [셰이더 버퍼 정의(C++)](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Shader/ShaderBuffers.h)


</details>


<details>
<summary> Ref </summary>

- [rastertek (DirectX 11 on Windows 10 Tutorials) - Tutorial 31: Water](https://www.rastertek.com/dx11win10tut31.html)

- [네이버 블로그( 프로그래머의 인생, 강동훈) - DirectX 11 : Lake from Habib's Water Shader](https://blog.naver.com/fah204)

- [Gaem Development: GLSL Shader - Change Hue/Saturation/Brightness](http://gamedev.stackexchange.com/questions/59797/glsl-shader-change-hue-saturation-brightness)

- [Github (jamesscully) - OpenGL Beach Scene](https://github.com/jamesscully/OpenGL-Beach-Scene)

</details>