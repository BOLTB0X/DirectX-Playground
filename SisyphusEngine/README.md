# Sisyphus Engine : Water (Basic)

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
      <td style="text-align:center; vertical-align:middle;">
        <p align="center">
        <img src="https://github.com/BOLTB0X/DirectX11-Draw/blob/main/DemoGIF/water/%EB%AC%BC00_%EC%B4%88%EA%B8%B0.png?raw=true" 
             alt="image 2" 
             style="width:600px; height:400px; object-fit:contain; border:1px solid #ddd; border-radius:4px;"/>
        </p>
      </td>
      <td style="text-align:center; vertical-align:middle;">
        <p align="center">
        <img src="https://github.com/BOLTB0X/DirectX11-Draw/blob/main/DemoGIF/water/%EB%AC%BC01_%ED%98%B8%EC%88%9803.gif?raw=true" 
             alt="image 2" 
             style="width:600px; height:400px; object-fit:contain; border:1px solid #ddd; border-radius:4px;"/>
        </p>
      </td>
    </tr>
    <tr>
      <td style="text-align:center; font-size:14px; font-weight:bold;">
      <p align="center">
      <a></a>물 평면</a>
      </p>
      </td>
      <td style="text-align:center; font-size:14px; font-weight:bold;">
      <p align="center">
      <a></a>반사</a>
      </p>
      </td>
    </tr>
  </table>
</p>

<p align="center">
  <table style="width:100%; text-align:center; border-spacing:20px;">
      <td style="text-align:center; vertical-align:middle;">
        <p align="center">
        <img src="https://github.com/BOLTB0X/DirectX11-Draw/blob/main/DemoGIF/water/%EB%AC%BC02_%EB%AC%BC%EA%B2%B003_%EB%85%B8%EB%A7%90%EB%A7%B5%EC%95%A0%EB%8B%88%EB%A9%94%EC%9D%B4%EC%85%98.gif?raw=true" 
             alt="image 2" 
             style="width:600px; height:400px; object-fit:contain; border:1px solid #ddd; border-radius:4px;"/>
        </p>
      </td>
      <td style="text-align:center; vertical-align:middle;">
        <p align="center">
        <img src="https://github.com/BOLTB0X/DirectX11-Draw/blob/main/DemoGIF/water/%EB%AC%BC01_%ED%98%B8%EC%88%9802.png?raw=true" 
             alt="image 2" 
             style="width:600px; height:400px; object-fit:contain; border:1px solid #ddd; border-radius:4px;"/>
        </p>
      </td>
    </tr>
    <tr>
      <td style="text-align:center; font-size:14px; font-weight:bold;">
      <p align="center">
      <a></a>물 물결</a>
      </p>
      </td>
      <td style="text-align:center; font-size:14px; font-weight:bold;">
      <p align="center">
      <a></a>구름 아래 물</a>
      </p>
      </td>
    </tr>
  </table>
</p>

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


- **Dual Normal Animation**

    - 두 개의 노말맵을 서로 다른 속도로 이동(UV Scrolling) 및 합성하여 역동적인 물결 생성

    - **Wind Direction** 과 **Wind Force**  수치를 반영하여 파도의 흐름과 세기를 실시간 제어


- **Refraction & Reflection Distortion**

    - 합성된 노말 벡터의 $x, z$ 성분을 활용하여 투영된 UV 좌표를 왜곡
    
    - `Refract Scale` 에 따라 물속 지형의 일렁임과 상단 반사상의 왜곡 강도 결정

- **Sun Column (Light Pillar)**

    - 광원 방향과 수면 노말의 상관관계를 계산하여 수면 위로 길게 늘어지는 빛기둥 생성

    - `Column Width` 와 `Intensity` 를 통해 태양 고도에 따른 반사 영역 최적화


- **Specular & Sparkle**

    - Shininess 제어 및 고광택 스펙큘러 구현

    - 노말의 미세한 변화를 이용해 특정 각도에서 반짝이는 Sparkle Intensity 효과 추가

- **Alpha Blending**

    - *Water Alpha* 와 최종 결과물에 곱해지는 *Final Alpha* 를 이원화하여 심도 있는 투명도 관리

    - 수면의 기본 색상(Base Color)과 대기 환경광의 자연스러운 혼합


- [시행착오 및 기록물들](https://github.com/BOLTB0X/DirectX11-Draw/tree/main/DemoGIF/water)

## [src](https://github.com/BOLTB0X/DirectX11-Draw/tree/Water-Basic/SisyphusEngine/src)

### [Common](https://github.com/BOLTB0X/DirectX11-Draw/tree/Water-Basic/SisyphusEngine/src/Common)

엔진 전반에서 공통적으로 사용되는 유틸리티, 수학 연산, 전역 설정을 관리

- [`ConstantHelper.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/Common/ConstantHelper.h) : 중앙 제어 및 환경 설정
- [`DebugHelper.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/Common/DebugHelper.h) : 진단 및 성능 모니터링
- [`MathHelper.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/Common/MathHelper.h) : 그래픽스 수학 유틸리티
- [`PropertyHelper.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/Common/PropertyHelper.h) : Getter, Setter 관리
- [`Pch.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/Common/Pch.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/Common/Pch.cpp) : Precompiled Header

    ---

### [Framework](https://github.com/BOLTB0X/DirectX11-Draw/tree/Water-Basic/SisyphusEngine/src/Framework)

엔진의 핵심 로직과 그래픽 리소스를 관리하는 추상화 레이어, 응용 프로그램이 구동되는 데 필요한 **'뼈대' 역할**

- [`IWidget.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/Framework/IWidget.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/Framework/IWidget.cpp) : UI 구성 요소의 최상위 인터페이스
- [`IImGUI.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/Framework/IImGUI.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/Framework/IImGUI.cpp) : ImGUI 라이브러리와 엔진 사이의 브릿지 역할
- [`Position.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/Framework/Position.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/Framework/Position.cpp) : 공간상의 좌표 데이터를 관리하는 클래스, 2D/3D 공간에서의 위치 계산을 공통으로 처리
- [`Shader.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/Framework/Shader.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/Framework/Shader.cpp) : DirectX 11의 셰이더 리소스(Vertex/Pixel Shader)를 래핑한 클래스, HLSL 컴파일, 상수 버퍼 바인딩 등
- [`StructContainer.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/Framework/StructContainer.h) / [`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/Framework/StructContainer.cpp) : 구조체 전용 템플릿 컨테이너

    ---

### [System](https://github.com/BOLTB0X/DirectX11-Draw/tree/Water-Basic/SisyphusEngine/src/System)

HW 및 OS와의 통신을 담당하며, 엔진의 **Life Cycle를 관리**

- [`System.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/System/System.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/System/System.cpp) : 엔진의 전체적인 진입점(Entry Point), . `Window`, `Input`, `MainEngine` 을 소유
- [`Window.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/System/Window.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/System/Window.cpp): : `Win32 API` 를 래핑하여 실제 창을 생성하고 관리
- [`Input.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/System/Input.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/System/Input.cpp) : 키보드 및 마우스의 원시 입력 데이터를 처리
- [`InputManager.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/System/InputManager.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/System/InputManager.cpp): 여러 입력 상태를 조합하거나, 프레임 간의 입력 변화를 추적하여 엔진 내부에서 사용하기 쉬운 이벤트 형태로 가공 및 관리

    ---

### [MainEngine](https://github.com/BOLTB0X/DirectX11-Draw/tree/Water-Basic/SisyphusEngine/src/MainEngine)

엔진의 실행 흐름을 제어

- [`MainEngine.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/MainEngine/MainEngine.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/MainEngine/MainEngine.cpp) : 시스템 전체의 메인 로직을 구동, `System` 클래스 위에서 동작하며, 프레임마다 업데이트와 렌더링 명령을 하위 시스템에 전달하는 컨트롤러 역할
- [`Camera.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/MainEngine/Camera.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/MainEngine/Camera.cpp): : 시각적 기준점(View/Projection Matrix)을 생성, 사용자 입력에 따라 시점을 이동하거나 화면 비율에 맞는 투영 행렬을 계산
- [`Timer.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/MainEngine/Timer.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/MainEngine/Timer.cpp) : 델타 타임($\Delta t$)을 계산하여 프레임 독립적인 객체 이동과 물리 계산이 가능하도록 정밀한 시간을 측정
- [`Fps.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/MainEngine/Fps.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/MainEngine/Fps.cpp): 초당 프레임 수를 측정하고 기록

    ---

### [RenderingEngine](https://github.com/BOLTB0X/DirectX11-Draw/tree/Water-Basic/SisyphusEngine/src/RederingEngine)

GPU를 직접 제어하고, 화면에 픽셀을 그려내기 위한 모든 리소스와 상태를 관리

- [`RenderingEngine.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/RederingEngine.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/RenderingEngine.cpp) : 렌더링 시스템의 최상위 클래스로, 모델, 셰이더, 렌더러를 총괄하여 최종적인 화면 출력을 실행

- [`Model`](https://github.com/BOLTB0X/DirectX11-Draw/tree/Water-Basic/SisyphusEngine/src/RederingEngine/Model) : 3D 객체의 정점(Vertex)과 인덱스 데이터를 관리
    - [`DefaultModel.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Model/DefaultModel.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Model/DefaultModel.cpp): `DefaultModelBuffer` 활용한 객체 단위
    - [`DefaultModelBuffer.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Model/DefaultModelBuffer.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Model/DefaultModelBuffer.cpp) : GPU 메모리에 데이터를 올리는 저수준 작업
    - [`Light.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Model/Light.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Model/Light.cpp): 조명 데이터(색상, 방향, 강도 등)를 관리
    - [`Texture.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Model/Texture.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Model/Texture.cpp): : 이미지 데이터를 GPU 리소스
    - [`TextureLoader.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Model/TextureLoader.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Model/TextureLoader.cpp): : 이미지 데이터를 GPU 리소스로 로드
    - [`TexturesManager.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Model/TexturesManager.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Model/TexturesManager.cpp): : 텍스처가 중복 로드되지 않도록 효율적으로 관리

    ---

- [`Renderer`](https://github.com/BOLTB0X/DirectX11-Draw/tree/Water-Basic/SisyphusEngine/src/RederingEngine/Renderer): DirectX 11의 렌더링 파이프라인 상태(State) 관리
    - [`Renderer.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Renderer/Renderer.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Renderer/Renderer.cpp): 각각의 객체로 분리하여 관리
    - [`DX11Device.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Renderer/DX11Device.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Renderer/DX11Device.cpp) : `ID3D11Device`와 `Context` 를 소유하며, 리소스 생성 및 명령 하달의 핵심 인터페이스 역할
    - [`DisplayInfo.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Renderer/DisplayInfo.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Renderer/DisplayInfo.cpp):  해상도, 주사율 등 디스플레이 설정
    - [`RenderTarget.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Renderer/RenderTarget.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Renderer/RenderTarget.cpp): 출력이 그려지는 캔버스
    - [`Rasterizer.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Renderer/Rasterizer.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Renderer/Rasterizer.cpp): 그리기 방식(와이어프레임/솔리드)
    - [`DepthStencilState.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Renderer/DepthStencilState.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Renderer/DepthStencilState.cpp): 깊이 판정
    - [`BlendState.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Renderer/BlendState.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Renderer/BlendState.cpp): 색상 혼합
    - [`SamplerState.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Renderer/SamplerState.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Renderer/SamplerState.cpp): 텍스처 필터링 방식

    ---

- [`Shader`](https://github.com/BOLTB0X/DirectX11-Draw/tree/Water-Basic/SisyphusEngine/src/RederingEngine/Shader) : 셰이더 관리 구역
    - [`ShaderBuffers.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Shader/ShaderBuffers.h) : HLSL과 연동될 셰이더 버퍼
    - [`ShaderBuffersManager.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Shader/ShaderBuffersManager.h) / [`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Shader/ShaderBuffersManager.cpp) : `ShaderBuffers.h` 에서 선언된 버퍼들을 관리하는 클래스
    - [`ShaderManager.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Shader/ShaderManager.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Shader/ShaderManager.cpp): 다양한 셰이더 객체를 통합 관리하고 적재적소에 바인딩
    - [`CloudShader.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Shader/CloudShader.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Shader/CloudShader.cpp) : 구름 표현을 위해 계산하는 전용 셰이더
    - [`SkyShader.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Shader/SkyShader.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Shader/SkyShader.cpp): 하늘 표현을 위해 계산하는 전용 셰이더
    - [`BicubicShader.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Shader/BicubicShader.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Shader/BicubicShader.cpp): 보간 알고리즘(Bicubic Interpolation)을 적용하여 고품질의 텍스처 샘플링이나 포스트 프로세싱 효과를 처리
    - [`LensFlareShader.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Shader/WLensFlareShader.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Shader/LensFlareShader.cpp) : 렌즈플레어 전용 셰이더 클래스
    - [`WaterShader.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Shader/WaterShader.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Shader/WaterShader.cpp) : 워터 전용 셰이더
    - [`RefractionShader.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Shader/RefractionShader.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/RederingEngine/Shader/RefractionShader.cpp) : 굴절 효과를 위한 셰이더 클래스
    
    ---

### [HLSL](https://github.com/BOLTB0X/DirectX11-Draw/tree/Water-Basic/SisyphusEngine/src/HLSL)

HLSL 폴더, GPU에서 실행되는 프로그램들

- [`Common.hlsli`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/HLSL/Common.hlsli) : : 모든 셰이더에서 공통으로 사용하는 Constant Buffer(World, View, Projection 행렬 등)와 기본 구조체를 모다둔 파일
- [`MAths.hlsli`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/HLSL/Maths.hlsli) : 셰이더 연산에 공통적으로 필요한 수학적 함수들을 모아둔 파일

- [`RefractionVS.hlsl`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/HLSL/RefractionVS.hlsl) : 굴절 버텍스 셰이더

    ---

- [`DefaultVS.hlsl`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/HLSL/DefaultVS.hlsl) : 일반적인 3D 구체를 위한 기본 정점 셰이더
- [`QuadVS.hlsl`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/HLSL/QuadVS.hlsl) : 화면 전체를 덮는 사각형이나 Post-processing을 위한 버텍스셰이더
- [`SkyVS.hlsl`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/HLSL/SkyVS.hlsl) : 하늘(Skybox/Skydome) 렌더링을 위해 카메라의 위치에 고정된 정점 변환을 처리
- [`WaterVS.hlsl`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/HLSL/WaterVS.hlsl) : 레스터택 기반, `reflectionPosition `, `refractionPosition `을 받아와 PS에게 전달해주는 버텍스 셰이더

    ---

- [`CloudPS.hlsl`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/HLSL/CloudPS.hlsl) : 구름의 밀도, 빛 투과율 등을 연산하여 실감 나는 구름 효과를 구현
- [`SkyPS.hlsl`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/HLSL/SkyPS.hlsl) : 시간대나 대기 산란(Atmospheric Scattering) 등을 고려한 하늘의 색상 및 태양을 계산
- [`BicubicPS.hlsl`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/HLSL/BicubicPS.hlsl) : 텍스처를 확대/축소할 때 더 부드러운 화질을 제공하는 Bicubic Interpolation 필터링을 수행
- [`LensPlarePS.hlsl`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/HLSL/LensPlare.hlsl) : 존 챔피언 렌즈플레어 방식 기반으로 고스트, 할로, 글로우, 드리프트 등 계산
- [`WaterPS.hlsl`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/HLSL/WaterPS.hlsl) : 레스터 텍 기반을 넘어서 **노말 애니메이션** , **빛기둥** , **Wind**  효과 까지 계산

- [`RefractionPS.hlsl`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/HLSL/RefractionPS.hlsl)
 : 레스터택 코드 기반, 굴전된 빛 관련 계산 처리

    ---

### [UI](https://github.com/BOLTB0X/DirectX11-Draw/tree/Water-Basic/SisyphusEngine/src/UI)

`IWidget`과 `IImGUI` 를 상속받아 구현된 실제 에디터 도구, . 엔진 내부의 파라미터를 실시간으로 조작할 수 있게 해주는 코드들을 모아둔 폴더

- [`UI.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/UI/UI.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/UI/UI.cpp) : 엔진의 전체 UI 시스템을 총괄하고 각 위젯들을 등록하고 프레임마다 UI 렌더링 루프를 실행
- [`CameraWidget.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/UI/CameraWidget.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/UI/CameraWidget.cpp): : 메라의 위치, 회전, 속도, FOV(시야각) 등을 실시간으로 확인하고 수정할 수 있는 제어 패널
- [`StatsWidget.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/UI/StatsWidget.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/UI/StatsWidget.cpp) : 엔진의 성능 데이터(FPS, Delta Time, 정점 개수 등)를 시각적으로 표시
- [`RenderStateWidget.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/UI/RenderStateWidget.h)/[`.cpp`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/UI/RenderStateWidget.cpp): 와이어프레임 모드 전환, 블렌드 상태 변경, 조명 값 조절 등 렌더링 엔진의 핵심 설정을 즉각적으로 변경하며 테스트

- [`ShaderBufferWidget.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/UI/ShaderBufferWidget.h): 셰이더 버퍼 전용 ImGui 위젯, 템플릿으로 선언해 두어, 이 클래스을 통해 ImGui 위젯 생성

- [`ShaderBufferImGuiDrawer.h`](https://github.com/BOLTB0X/DirectX11-Draw/blob/Water-Basic/SisyphusEngine/src/UI/ShaderBufferImGuiDrawer.h) : 기존 클래스로 ImGui 내 요소들을 그렸던것과 달리 `namespace` 를 통해서 사용할 내용들을 정의하는 코드

    ---

## Ref

- [John Chapman Graphics 블로그 - Pseudo Water-Basic](https://john-chapman-graphics.blogspot.com/2013/02/pseudo-lens-flare.html)

- [ShaderToy - musk's lens flare mod(Icecool)](https://www.shadertoy.com/view/XdfXRX)

- [therealmjp - Stairway To (Programmable Sample Point) Heaven](https://therealmjp.github.io/posts/programmable-sample-points/)

- [pngtree - 이미지 사용](https://pngtree.com/so/lens-flare)
