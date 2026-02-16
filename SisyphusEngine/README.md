# Sisyphus Engine : Procedural 렌즈플레어

<p align="center">
  <table style="width:100%; text-align:center; border-spacing:20px;">
    <tr>
        <td style="text-align:center; vertical-align:middle;">
        <p align="center">
        <img src="https://github.com/BOLTB0X/DirectX11-Draw/blob/main/DemoGIF/lensFlare/%EB%A0%8C%EC%A6%88%ED%94%8C%EB%A0%88%EC%96%B407_%EC%A0%88%EC%B0%A8%EC%A0%8102.gif?raw=true" 
             alt="image 2" 
             style="; object-fit:contain; border:1px solid #ddd; border-radius:4px;"/>
        </p>
      </td>
    </tr>
    <tr>
      <td style="text-align:center; font-size:14px; font-weight:bold;">
      <p align="center">
      <a></a> John Chapman의 Pseudo Procedural_LensFlare 와 icecool(Shadertoy)d의 코드 기반으로 시지프스 엔진 환경에 맞춰 적용</a>
      </p>
      </td>
    </tr>
  </table>
</p>

기존 **Procedural_LensFlare** 의 클래식인 존 챔피언 방식에서 `processLensFeatures` 에 로직을 수정<br/>
[시지프스 엔진:태양과 구름](https://github.com/BOLTB0X/DirectX11-Draw/tree/Sun-and-Cloud/SisyphusEngine) 컨셉과 동일하게 텍스처는 오로직 **noise** 만 활용


## Development Environment

- **OS** : Windows 11
- **IDE** : [VisualStudio 2022 community](https://www.microsoft.com/en-us/download/details.aspx?id=17431)
- **Lang** : C++, HLSL
- **API** 
    - [DirectX 11](https://www.microsoft.com/en-us/download/details.aspx?id=17431)
    - [Win32 API](https://learn.microsoft.com/ko-kr/windows/win32/api/)
- **Build & Package Manager**
    - [vcpkg](https://github.com/microsoft/vcpkg?tab=readme-ov-file)
- **External Libraries**
    - [stb_image](https://github.com/nothings/stb/tree/master) : `jpg`, `png` 파일 로딩
    - [Imgui](https://github.com/ocornut/imgui) : 디버깅 및 개발용 UI

## Feature

<p align="center">
  <table style="width:100%; text-align:center; border-spacing:20px;">
    <tr>
    <td style="text-align:center; vertical-align:middle;">
        <p align="center">
        <img src="https://github.com/BOLTB0X/DirectX11-Draw/blob/main/DemoGIF/lensFlare/%EB%A0%8C%EC%A6%88%ED%94%8C%EB%A0%88%EC%96%B407_%EC%A0%88%EC%B0%A8%EC%A0%8103_%EC%A0%95%EB%A9%B4.png?raw=true" 
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
      <a></a>정면</a>
      </p>
      </td>
      <td style="text-align:center; font-size:14px; font-weight:bold;">
      <p align="center">
      <a></a>시야 체크</a>
      </p>
      </td>
    </tr>
  </table>
</p>

<p align="center">
  <table style="width:100%; text-align:center; border-spacing:20px;">
      <td style="text-align:center; vertical-align:middle;">
        <p align="center">
        <img src="https://github.com/BOLTB0X/DirectX11-Draw/blob/main/DemoGIF/lensFlare/%EB%A0%8C%EC%A6%88%ED%94%8C%EB%A0%88%EC%96%B408_%ED%83%9C%EC%96%91_%EC%B0%8C%EA%B7%B8%EB%9F%AC%EC%A7%90_%EC%88%98%EC%A0%95.png?raw=true" 
             alt="image 2" 
             style="width:600px; height:400px; object-fit:contain; border:1px solid #ddd; border-radius:4px;"/>
        </p>
      </td>
      <td style="text-align:center; vertical-align:middle;">
        <p align="center">
        <img src="https://github.com/BOLTB0X/DirectX11-Draw/blob/main/DemoGIF/lensFlare/%EB%A0%8C%EC%A6%88%ED%94%8C%EB%A0%88%EC%96%B408_%ED%83%9C%EC%96%91_%EC%B0%8C%EA%B7%B8%EB%9F%AC%EC%A7%90_%EC%88%98%EC%A0%952.png?raw=true" 
             alt="image 2" 
             style="width:600px; height:400px; object-fit:contain; border:1px solid #ddd; border-radius:4px;"/>
        </p>
      </td>
    </tr>
    <tr>
      <td style="text-align:center; font-size:14px; font-weight:bold;">
      <p align="center">
      <a></a>태양 찌그러짐 수정 1</a>
      </p>
      </td>
      <td style="text-align:center; font-size:14px; font-weight:bold;">
      <p align="center">
      <a></a>태양 찌그러짐 수정 2</a>
      </p>
      </td>
    </tr>
  </table>
</p>



- **Noise 활용**

    ```cpp
    float noise(float t)
    {
        return iNoise.Sample(iSampler, float2(t, 0.0f)).x;
    } // noise
    ```

    ```cpp
    float noise(float2 t)
    {
        return iNoise.Sample(iSampler, t + float2(iTime * 0.1f, iTime * 0.1f)).x;
    } // noise
    ```


- **Ghost, sortHalo, HardHalo** 코드로 대체

    ```cpp
    float3 softHalo(float2 uvd, float2 pos)
    {
        float3 color;
    
        color.r = max(1.0 / (1.0 + iF2Sharpness * pow(length(uvd + iF2Offset.r * pos), 2.0)), 0.0) * iF2ColorMult.r;
        color.g = max(1.0 / (1.0 + iF2Sharpness * pow(length(uvd + iF2Offset.g * pos), 2.0)), 0.0) * iF2ColorMult.g;
        color.b = max(1.0 / (1.0 + iF2Sharpness * pow(length(uvd + iF2Offset.b * pos), 2.0)), 0.0) * iF2ColorMult.b;
        return color;
    } // softHalo
    ```

    ```cpp
    // f4, f5, f6
    float3 spotsPattern(float2 uvx, float2 pos, float3 offsets, float power, float3 intensity)
    {
        float3 color;
    
        color.r = max(0.01 - pow(length(uvx + offsets.r * pos), power), 0.0) * intensity.r;
        color.g = max(0.01 - pow(length(uvx + offsets.g * pos), power), 0.0) * intensity.g;
        color.b = max(0.01 - pow(length(uvx + offsets.b * pos), power), 0.0) * intensity.b;
        return color;
    } // spotsPattern
    ```


- **Falloff**

    - 광원과 멀어질수록 Ghost, Halo들이 작아지고 연해지는 효과 추가

- **Visibility Check**

    - 광원이 카메라의 시야 방향과 충분히 정렬되어 있을 때만 플레어 생성

    - 광원과 일정 거리 및 각도가 맞지 않으면 렌즈플레어가 사라짐


- [시행착오 및 기록물들](https://github.com/BOLTB0X/DirectX11-Draw/tree/Procedural_LensFlare/DemoGIF/Procedural_LensFlare)

## [src](https://github.com/BOLTB0X/DirectX11-Draw/tree/Procedural_Procedural_LensFlare/SisyphusEngine/src)

### [Common](https://github.com/BOLTB0X/DirectX11-Draw/tree/Procedural_Procedural_LensFlare/SisyphusEngine/src/Common)

엔진 전반에서 공통적으로 사용되는 유틸리티, 수학 연산, 전역 설정을 관리

- [`ConstantHelper.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_Procedural_LensFlare/SisyphusEngine/src/Common/ConstantHelper.h) : 중앙 제어 및 환경 설정
- [`DebugHelper.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/Common/DebugHelper.h) : 진단 및 성능 모니터링
- [`MathHelper.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/Common/MathHelper.h) : 그래픽스 수학 유틸리티
- [`PropertyHelper.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/Common/PropertyHelper.h) : Getter, Setter 관리
- [`Pch.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/Common/Pch.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/Common/Pch.cpp) : Precompiled Header

    ---

### [Framework](https://github.com/BOLTB0X/DirectX11-Draw/tree/Procedural_LensFlare/SisyphusEngine/src/Framework)

엔진의 핵심 로직과 그래픽 리소스를 관리하는 추상화 레이어, 응용 프로그램이 구동되는 데 필요한 **'뼈대' 역할**

- [`IWidget.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/Framework/IWidget.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/Framework/IWidget.cpp) : UI 구성 요소의 최상위 인터페이스
- [`IImGUI.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/Framework/IImGUI.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/Framework/IImGUI.cpp) : ImGUI 라이브러리와 엔진 사이의 브릿지 역할
- [`Position.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/Framework/Position.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/Framework/Position.cpp) : 공간상의 좌표 데이터를 관리하는 클래스, 2D/3D 공간에서의 위치 계산을 공통으로 처리
- [`Shader.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/Framework/Shader.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/Framework/Shader.cpp) : DirectX 11의 셰이더 리소스(Vertex/Pixel Shader)를 래핑한 클래스, HLSL 컴파일, 상수 버퍼 바인딩 등

    ---

### [System](https://github.com/BOLTB0X/DirectX11-Draw/tree/Procedural_LensFlare/SisyphusEngine/src/System)

HW 및 OS와의 통신을 담당하며, 엔진의 **Life Cycle를 관리**

- [`System.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/System/System.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/System/System.cpp) : 엔진의 전체적인 진입점(Entry Point), . `Window`, `Input`, `MainEngine` 을 소유
- [`Window.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/System/Window.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/System/Window.cpp): : `Win32 API` 를 래핑하여 실제 창을 생성하고 관리
- [`Input.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/System/Input.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/System/Input.cpp) : 키보드 및 마우스의 원시 입력 데이터를 처리
- [`InputManager.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/System/InputManager.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/System/InputManager.cpp): 여러 입력 상태를 조합하거나, 프레임 간의 입력 변화를 추적하여 엔진 내부에서 사용하기 쉬운 이벤트 형태로 가공 및 관리

    ---

### [MainEngine](https://github.com/BOLTB0X/DirectX11-Draw/tree/Procedural_LensFlare/SisyphusEngine/src/MainEngine)

엔진의 실행 흐름을 제어

- [`MainEngine.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/MainEngine/MainEngine.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/MainEngine/MainEngine.cpp) : 시스템 전체의 메인 로직을 구동, `System` 클래스 위에서 동작하며, 프레임마다 업데이트와 렌더링 명령을 하위 시스템에 전달하는 컨트롤러 역할
- [`Camera.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/MainEngine/Camera.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/MainEngine/Camera.cpp): : 시각적 기준점(View/Projection Matrix)을 생성, 사용자 입력에 따라 시점을 이동하거나 화면 비율에 맞는 투영 행렬을 계산
- [`Timer.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/MainEngine/Timer.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/MainEngine/Timer.cpp) : 델타 타임($\Delta t$)을 계산하여 프레임 독립적인 객체 이동과 물리 계산이 가능하도록 정밀한 시간을 측정
- [`Fps.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/MainEngine/Fps.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/MainEngine/Fps.cpp): 초당 프레임 수를 측정하고 기록

    ---

### [RenderingEngine](https://github.com/BOLTB0X/DirectX11-Draw/tree/Procedural_LensFlare/SisyphusEngine/src/RederingEngine)

GPU를 직접 제어하고, 화면에 픽셀을 그려내기 위한 모든 리소스와 상태를 관리

- [`RenderingEngine.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/RederingEngine.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/RenderingEngine.cpp) : 렌더링 시스템의 최상위 클래스로, 모델, 셰이더, 렌더러를 총괄하여 최종적인 화면 출력을 실행

- [`Model`](https://github.com/BOLTB0X/DirectX11-Draw/tree/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Model) : 3D 객체의 정점(Vertex)과 인덱스 데이터를 관리
    - [`DefaultModel.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Model/DefaultModel.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Model/DefaultModel.cpp): `DefaultModelBuffer` 활용한 객체 단위
    - [`DefaultModelBuffer.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Model/DefaultModelBuffer.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Model/DefaultModelBuffer.cpp) : GPU 메모리에 데이터를 올리는 저수준 작업
    - [`Light.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Model/Light.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Model/Light.cpp): 조명 데이터(색상, 방향, 강도 등)를 관리
    - [`Texture.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Model/Texture.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Model/Texture.cpp): : 이미지 데이터를 GPU 리소스
    - [`TextureLoader.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Model/TextureLoader.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Model/TextureLoader.cpp): : 이미지 데이터를 GPU 리소스로 로드
    - [`TexturesManager.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Model/TexturesManager.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Model/TexturesManager.cpp): : 텍스처가 중복 로드되지 않도록 효율적으로 관리

    ---

- [`Renderer`](https://github.com/BOLTB0X/DirectX11-Draw/tree/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Renderer): DirectX 11의 렌더링 파이프라인 상태(State) 관리
    - [`Renderer.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Renderer/Renderer.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Renderer/Renderer.cpp): 각각의 객체로 분리하여 관리
    - [`DX11Device.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Renderer/DX11Device.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Renderer/DX11Device.cpp) : `ID3D11Device`와 `Context` 를 소유하며, 리소스 생성 및 명령 하달의 핵심 인터페이스 역할
    - [`DisplayInfo.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Renderer/DisplayInfo.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Renderer/DisplayInfo.cpp):  해상도, 주사율 등 디스플레이 설정
    - [`RenderTarget.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Renderer/RenderTarget.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Renderer/RenderTarget.cpp): 출력이 그려지는 캔버스
    - [`Rasterizer.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Renderer/Rasterizer.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Renderer/Rasterizer.cpp): 그리기 방식(와이어프레임/솔리드)
    - [`DepthStencilState.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Renderer/DepthStencilState.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Renderer/DepthStencilState.cpp): 깊이 판정
    - [`BlendState.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Renderer/BlendState.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Renderer/BlendState.cpp): 색상 혼합
    - [`SamplerState.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Renderer/SamplerState.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Renderer/SamplerState.cpp): 텍스처 필터링 방식

    ---

- [`Shader`](https://github.com/BOLTB0X/DirectX11-Draw/tree/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Shader) : 셰이더 관리 구역
    - [`ShaderBuffers.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Shader/ShaderBuffers.h) : HLSL과 연동될 셰이더 버퍼
    - [`ShaderManager.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Shader/ShaderManager.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Shader/ShaderManager.cpp): 다양한 셰이더 객체를 통합 관리하고 적재적소에 바인딩
    - [`CloudShader.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Shader/CloudShader.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Shader/CloudShader.cpp) : 구름 표현을 위해 계산하는 전용 셰이더
    - [`SkyShader.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Shader/SkyShader.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Shader/SkyShader.cpp): 하늘 표현을 위해 계산하는 전용 셰이더
    - [`BicubicShader.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Shader/BicubicShader.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Shader/BicubicShader.cpp): 보간 알고리즘(Bicubic Interpolation)을 적용하여 고품질의 텍스처 샘플링이나 포스트 프로세싱 효과를 처리
    - [`Procedural_LensFlareShader.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Shader/Procedural_LensFlareShader.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/RederingEngine/Shader/Procedural_LensFlareShader.cpp)
    
    ---

### [HLSL](https://github.com/BOLTB0X/DirectX11-Draw/tree/Procedural_LensFlare/SisyphusEngine/src/HLSL)

HLSL 폴더, GPU에서 실행되는 프로그램들

- [`Common.hlsli`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/HLSL/Common.hlsli) : : 모든 셰이더에서 공통으로 사용하는 Constant Buffer(World, View, Projection 행렬 등)와 기본 구조체를 모다둔 파일
- [`MAths.hlsli`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/HLSL/Maths.hlsli) : 셰이더 연산에 공통적으로 필요한 수학적 함수들을 모아둔 파일

    ---

- [`DefaultVS.hlsl`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/HLSL/DefaultVS.hlsl) : 일반적인 3D 구체를 위한 기본 정점 셰이더
- [`QuadVS.hlsl`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/HLSL/QuadVS.hlsl) : 화면 전체를 덮는 사각형이나 Post-processing을 위한 버텍스셰이더
- [`SkyVS.hlsl`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/HLSL/SkyVS.hlsl) : 하늘(Skybox/Skydome) 렌더링을 위해 카메라의 위치에 고정된 정점 변환을 처리

    ---

- [`CloudPS.hlsl`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/HLSL/CloudPS.hlsl) : 구름의 밀도, 빛 투과율 등을 연산하여 실감 나는 구름 효과를 구현
- [`SkyPS.hlsl`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/HLSL/SkyPS.hlsl) : 시간대나 대기 산란(Atmospheric Scattering) 등을 고려한 하늘의 색상 및 태양을 계산
- [`BicubicPS.hlsl`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/HLSL/BicubicPS.hlsl) : 텍스처를 확대/축소할 때 더 부드러운 화질을 제공하는 Bicubic Interpolation 필터링을 수행
- [`Procedural_LensFlarePS.hlsl`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/HLSL/Procedural_LensFlarePS.hlsl) : 존 챔피언 렌즈플레어 방식 기반으로 고스트, 할로, 글로우, 드리프트 등 계산

    ---

### [UI](https://github.com/BOLTB0X/DirectX11-Draw/tree/Procedural_LensFlare/SisyphusEngine/src/UI)

`IWidget`과 `IImGUI` 를 상속받아 구현된 실제 에디터 도구, . 엔진 내부의 파라미터를 실시간으로 조작할 수 있게 해주는 코드들을 모아둔 폴더

- [`UI.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/UI/UI.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/UI/UI.cpp) : 엔진의 전체 UI 시스템을 총괄하고 각 위젯들을 등록하고 프레임마다 UI 렌더링 루프를 실행
- [`CameraWidget.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/UI/CameraWidget.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/UI/CameraWidget.cpp): : 메라의 위치, 회전, 속도, FOV(시야각) 등을 실시간으로 확인하고 수정할 수 있는 제어 패널
- [`StatsWidget.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/UI/StatsWidget.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/UI/StatsWidget.cpp) : 엔진의 성능 데이터(FPS, Delta Time, 정점 개수 등)를 시각적으로 표시
- [`RenderStateWidget.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/UI/RenderStateWidget.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Procedural_LensFlare/SisyphusEngine/src/UI/RenderStateWidget.cpp): 와이어프레임 모드 전환, 블렌드 상태 변경, 조명 값 조절 등 렌더링 엔진의 핵심 설정을 즉각적으로 변경하며 테스트

    ---

## Ref

- [John Chapman Graphics 블로그 - Pseudo Procedural_LensFlare](https://john-chapman-graphics.blogspot.com/2013/02/pseudo-lens-flare.html)

- [ShaderToy - musk's lens flare mod(Icecool)](https://www.shadertoy.com/view/XdfXRX)

- [therealmjp - Stairway To (Programmable Sample Point) Heaven](https://therealmjp.github.io/posts/programmable-sample-points/)

- [pngtree - 이미지 사용](https://pngtree.com/so/lens-flare)
