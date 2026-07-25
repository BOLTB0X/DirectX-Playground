# Lab

![헤헤](https://github.com/BOLTB0X/DirectX11-Draw/blob/main/DemoGIF/%EC%97%B0%EC%84%B1%EC%A7%84.gif?raw=true)

DriectX 11 개발 연습 및 학습물들 정리(아무 기능 구현이나 삽질 [과정에 생겼던 에러 및 궁금증](https://github.com/BOLTB0X/DirectX11-Draw/blob/main/Lab/Studies/Error-history.md) 들을 기록)

## 알면 좋은 것

<details>
<summary> DirectX 11 란? </summary>


### [DirectX 11](https://www.microsoft.com/en-us/download/details.aspx?id=6812) 란 무엇인가

<p align="center">
   <img src="https://i0.wp.com/howtocheckversion.com/wp-content/uploads/2023/09/1.png?w=800&ssl=1" alt="Example Image" width="100%">
</p>

**MS가 제공하는 저수준 그래픽 및 멀티미디어 API(Application Programming Interface)**

즉, 게임과 멀티미디어 프로그램이 GPU와 효율적으로 통신하여 테셀레이션, Compute Shader 등 고급 그래픽 기술을 구현하고

3D 화면, 사운드 등을 생성하게 해주는 중개자 역할하는 랜더링 라이브러리

---

### 특징

 - 컴퓨팅 셰이더 (DirectCompute)
  
    *렌더링뿐만 아니라 범용 연산(GPGPU)에도 GPU의 병렬 처리 능력을 활용할 수 있게 되어, 물리 시뮬레이션, AI 연산 등 다양한 분야에 응용 가능*

- 렌더링 성능 및 효율 향상

- GPU 메모리 추상화

- 멀티 스레딩 지원 강화

    Direct3D 11부터 멀티 스레딩을 통한 렌더링 파이프라인 효율 증대

---

### DirectX 11 기본 구성

> 이 계산은 CPU가 하나, GPU가 하나?

DirectX 11은 단순한 API가 아니라
CPU와 GPU가 역할을 분담해 *‘프레임’* 을 생산하는 규칙 집합

### DirectX 11 전체 구조

- [CPU 영역]
    ```
    Application
    └─ Device
    └─ DeviceContext
        └─ Draw Call
    ```

- [GPU 영역]
    ```
    Input Assembler
    └─ Vertex Shader
    └─ Hull Shader (optional)
    └─ Domain Shader (optional)
    └─ Geometry Shader (optional)
    └─ Rasterizer
    └─ Pixel Shader
    └─ Output Merger
    ```

DX11은 명확하게 CPU와 GPU의 경계를 나눈 API

---

<br/>

</details>


<details>
<summary> CPU 파이프라인 </summary>


### CPU 파이프라인 핵심 구성 요소

| 구성요소                  | 역할            |
| --------------------- | ------------- |
| `ID3D11Device`        | GPU 리소스 생성 담당 |
| `ID3D11DeviceContext` | 렌더링 명령 실행 담당  |
| `SwapChain` | 화면 출력 담당  |


### (1): `ID3D11Device` - “리소스 공장”

> `Device` 는 공장, `Context` 는 조작자”

```cpp
device->CreateBuffer(...)
device->CreateTexture2D(...)
device->CreateVertexShader(...)
```

GPU 자원을 **‘생성’** 하는 역할만 담당

- **Vertex Buffer**
- **Index Buffer**
- **Texture**
- **Shader**
- **Sampler**
- **Input Layout**

`Device`는 렌더링을 하지 않고 오직 생성만 담당

---

### (2): `ID3D11DeviceContext` — “GPU 상태 조작자”

```cpp
context->IASetVertexBuffers(...)
context->VSSetShader(...)
context->PSSetShader(...)
context->DrawIndexed(...)
```

GPU 파이프라인의 상태를 설정하고 **Draw Call** 을  실제로 날리는 주체

DX11은 상태 기반 API **(State Machine)**

즉시 실행이 아니라, GPU 커맨드 큐에 쌓아 놓고 **Draw Call = CPU -> GPU** 명령

#### (2) - 1. `IASetVertexBuffers`

```cpp
void IASetVertexBuffers(
  [in] UINT StartSlot,                  // 1. 시작 슬롯 인덱스 (0~15)
  [in] UINT NumBuffers,                 // 2. 바인딩할 버퍼 개수
  [in] ID3D11Buffer * const *ppVertexBuffers, // 3. 버퍼 포인터 배열
  [in] const UINT *pStrides,            // 4. 버퍼당 버텍스 구조체 크기(Byte)
  [in] const UINT *pOffsets             // 5. 버퍼 시작점부터의 오프셋
);
```

렌더링 파이프라인의 가장 첫 단계인 입력 어셈블러(`Input Assembler`, IA) 단계에 버텍스 데이터가 담긴 버텍스 버퍼(Vertex Buffer)들을 바인딩(연결)하는 메서드

```cpp
UINT stride = sizeof(Vertex); // Vertex 구조체의 크기
UINT offset = 0;              // 버퍼 시작점

// 0번 슬롯에 1개의 버텍스 버퍼 바인딩
deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
```

GPU가 그림을 그리기 전에 *"어떤 버텍스 데이터를 사용해 도형을 만들지"* 알려주는 핵심적인 함수

- **버퍼 바인딩** : `ID3D11Buffer` 인터페이스로 생성된 버텍스 버퍼를 입력 어셈블러 단계의 슬롯에 연결

- **복수 버퍼 지원** : 한 번의 호출로 여러 개의 정점 버퍼 배열을 연결할 수 있음 (최대 16~32개 입력 슬롯 제공)

- 파이프라인 설정: `Draw` 호출 이전에 반드시 실행되어야 하며, `IASetInputLayout`, `IASetPrimitiveTopology` 와 함께 작동하여 도형을 그리는 형태를 정의


#### (2) - 2. `VSSetShader`

> "GPU야, 이제부터 정점을 처리할 때 이 셰이더 코드를 사용해!"

```cpp
void VSSetShader(
  [in, optional] ID3D11VertexShader   *pVertexShader,
  [in, optional] ID3D11ClassInstance * const *ppClassInstances,
  [in]           UINT                 NumClassInstances
);
```

버텍스 셰이더(Vertex Shader) 스테이지에 특정 **버텍스 셰이더 프로그램을 설정(바인딩)하는 함수**

렌더링 파이프라인(Rendering Pipeline)의 고정 단계가 아닌, 프로그래밍 가능한 셰이더 단계 중 하나인 버텍스 셰이더를 활성화하고, GPU가 버텍스 데이터를 처리할 때 어떤 계산 방식(코드)을 사용할지 지정

#### (2) - 3. `PSSetShader`

> "지금부터 이 픽셀 셰이더 알고리즘을 사용하여 색을 칠해라"

```cpp
void PSSetShader(
  [in, optional] ID3D11PixelShader    *pPixelShader,        // 컴파일된 픽셀 셰이더 포인터
  [in, optional] ID3D11ClassInstance *const *ppClassInstances, // 클래스 인스턴스 배열 (대체로 NULL)
  [in]           UINT                 NumClassInstances    // 클래스 인스턴스 개수 (대체로 0)
);
```

픽셀 셰이더(Pixel Shader) 스테이지를 디바이스 컨텍스트(`Device Context`)에 바인딩(설정)하는 함수

-  `VSSetShader`와 함께 사용되며, 픽셀 셰이더가 사용하는 텍스처나 샘플러를 설정하는 `PSSetShaderResources`, `PSSetSamplers` 등과 함께 쓰임

- 렌더링 횟수가 많으면 효율적인 셰이더 관리가 필요

#### (2) - 4. `DrawIndexed` : 인덱스 기반 그리기 (Indexed Drawing)

```cpp
void DrawIndexed(
  [in] UINT IndexCount,           // 1. 그릴 인덱스 개수 (예: 삼각형 하나면 3)
  [in] UINT StartIndexLocation,   // 2. 인덱스 버퍼에서 읽기 시작할 첫 인덱스의 위치(오프셋)
  [in] INT  BaseVertexLocation    // 3. 인덱스에 더해질 정점 버퍼 내의 기준 위치(오프셋)
);
```

DirectX 11에서 `DrawIndexed` 는 인덱스 버퍼(인덱스 배열)를 사용하여 정점 버퍼에 저장된 정점들을 연결해 3D 도형을 렌더링하는 함수

단순히 정점 순서대로 그리는 `Draw` 함수와 달리, `DrawIndexed` 는 정점 데이터를 재사용할 수 있어 메모리 효율과 렌더링 성능을 높이는 데 필수적인 기법

- 인덱스 기반 렌더링: **Vertex Buffer** 에는 고유한 정점 정보(위치, 색상 등)를 한 번만 저장하고, 인덱스 버퍼(Index Buffer)에 이 정점들을 어떻게 연결(순서)할지 번호(인덱스)를 저장

- 정점 재사용: 큐브(Cube)와 같이 정점이 공유되는 3D 모델을 그릴 때, 24개 정점이 아닌 8개 정점만 저장하고 인덱스만 조절하여 그릴 수 있어 메모리 대역폭을 절약

- 호출 위치: `ID3D11DeviceContext` 인터페이스에서 호출

- 필수 사전 설정: `IASetVertexBuffers`로 버텍스 버퍼를, `IASetIndexBuffer`로 인덱스 버퍼를, `IASetPrimitiveTopology`로 토폴로지(예: 트라이앵글 리스트)를 설정해야 함


### (3): `SwapChain` — “프레임 교체 관리자”

> 더링된 프레임을 화면(모니터)에 표시하기 위해 사용되는 가상 프레임 버퍼들의 집합(체인)이자, 이 버퍼들을 교체(Swap)하여 제어하는 기술

```cpp
swapChain->Present(1, 0);
```

가장 흔한 형태는 2개의 버퍼를 사용하는 **더블 버퍼링(Double Buffering)** 이며, 렌더링 속도 향상과 **화면 찢어짐(Tearing) 현상** 을 방지하는 핵심 역할

즉 **화면 출력 담당**

#### `SwapChain`의 주요 구성

- **Front Buffer** : 현재 모니터에 출력되고 있는 버퍼

- **Back Buffer** : GPU가 다음 프레임을 렌더링하는 버퍼. (더블 버퍼링에서는 1개, 트리플 버퍼링에서는 2개 이상 가능)

- **`IDXGISwapChain` 인터페이스** : DXGI(DirectX Graphics Infrastructure)에서 제공하며, *Back Buffer* 와 *Front Buffer* 를 관리하고 교체하는 COM 인터페이스

#### 더블 버퍼링의 작동 원리

1. 렌더링 : GPU는 **렌더링 대상(`Render Target`)** 인 백 버퍼에 새로운 화면을 그림

2. 교체 (Swap) : 렌더링이 완료되면, **백 버퍼는 프론트 버퍼로 전환되어 화면에 표시되고, 이전 프론트 버퍼는 백 버퍼로 이동하여 지워지고 다시 렌더링을 준비하는데,**
이 과정을 반복하여 끊김 없는 화면을 제공

- 주요 메서드: `Present()`

---

<br/>

</details>


<details>
<summary> GPU 파이프라인 </summary>

### GPU 파이프라인 핵심 구성

GPU 파이프라인은 3D 공간의 Vertex 데이터를 2D 화면 픽셀로 변환하여 렌더링하는 순차적이고 프로그램 가능한 단계적 처리 과정

| 단계 (Stage) | 역할 및 주요 기능 | 비고 |
|-------------|------------------|------|
| Input Assembler (IA) | 정점 버퍼에서 데이터를 읽어 기본 도형(삼각형 등)으로 조립 | 데이터의 시작점 |
| Vertex Shader (VS) | 개별 정점의 위치 변환(MVP), 조명, 텍스처 좌표 계산 | 필수 단계 |
| Tessellation (HS / DS / TS) | 지형 등의 모델을 세분화하여 디테일을 높임 | 선택 사항 |
| Geometry Shader (GS) | 정점 데이터를 바탕으로 도형을 새로 생성하거나 구조 변형 | 선택 사항 |
| Rasterizer (RS) | 3D 도형을 2D 픽셀로 변환 및 보간, 불필요한 데이터 제거 | 하드웨어 고정 단계 |
| Pixel Shader (PS) | 픽셀 단위 최종 색상 결정, 조명 및 텍스처 매핑 | 화면의 질감 결정 |
| Output Merger (OM) | 최종 색상을 화면에 기록, 깊이(Depth) 및 스텐실 테스트 처리 | 렌더링 마무리 |

### (1) Input Assembler (IA)

> > CPU가 보낸 정점 데이터를 GPU 셰이더가 이해할 수 있는 모양으로 조립해서 전달해주는 첫 관문

GPU가 처음 만나는 단계, CPU가 메모리에 저장한 기하학적 데이터(정점 및 인덱스)를 읽어들여 GPU가 셰이더 단계에서 처리할 수 있는 형태(프리미티브)로 조립하는 고정 기능(Fixed-function) 단계

- `Input Layout` (입력 레이아웃): 버텍스 버퍼 내의 데이터 구조(위치, 색상, 노멀 등)를 정의

- `Primitive Topology` (기본 형식): 정점을 어떻게 해석할지 정의 (ex: `D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST` - 삼각형 3개씩 조합)

- **Vertex Buffer** : 기하학적 정점 데이터 자체

    ```cpp
     // ex
    struct Vertex {
        float3 position;
        float3 normal;
        float2 uv;
    };
    ```

- **Index Buffer** (인덱스 버퍼 - 선택 사항): 버텍스를 어떤 순서로 조합할지 정의한 인덱스 배열


### (2) Vertex Shader (VS)

> Input Assembler (IA) -> **Vertex Shader (VS)**  -> (Hull/Domain/Geometry Shader)  -> Rasterizer -> Pixel Shader

**3D 데이터를 2D 화면으로 그리기 위해 정점의 위치와 특성을 계산하고 변환하는 핵심 단계**

1. **입력** : Input Assembler(IA)로부터 정점 데이터(위치, 법선, UV 등)를 받음

2. **처리** : HLSL로 작성된 코드가 각 정점마다 병렬로 실행

3. **출력** : 변환된 위치(`SV_POSITION` 시맨틱 필수)와 다음 단계에 필요한 데이터를 출력

그래픽스 파이프라인의 핵심적인 프로그래밍 가능 단계로, 3D 모델을 구성하는 개별 Vertex 데이터를 처리하는 첫 번째 GPU 단계

```cpp
float4 worldPos = mul(float4(pos,1), world);
float4 viewPos  = mul(worldPos, view);
float4 clipPos  = mul(viewPos, proj);
```

- 공간 변환은 VS의 책임

- 카메라 개념은 여기서 수학으로 구현됨

### (3) Tessellation (Opt)

> DX11에서만 존재

3D 모델의 단순한 폴리곤(삼각형)을 GPU가 실시간으로 더 작고 많은 삼각형으로 세분화(Subdivision)하여, 입체감과 디테일(세밀함)을 극적으로 향상시키는 단계이며 필수는 아님

- 대부분 프로젝트에서 안 쓰지만 지형, 디테일 증가엔 강력

### (4) Geometry Shader (GS, Opt)

> 정점 -> 정점<br/>점 -> 선 -> 삼각형 변환 가능

Vertex Shader와 Rasterizer 사이에서 Vertex뿐만 아니라 점, 선, 삼각형 같은 기초 도형(Primitive) 단위를 입력받아 처리하는 옵션인 셰이더

- 도형을 새로 생성하거나 삭제할 수 있어 파티클 시스템, 메쉬 증폭, 빌보드 기법 등에 활용

### (5) Rasterizer

> 기하 -> 픽셀로 변환하는 단계

** 그래픽 렌더링 파이프라인의 핵심 단계로, 3차원 공간상의 기하학적 정보(벡터 데이터)를 화면에 표시할 2차원 픽셀(래스터 데이터)로 변환하는 역할**

즉, Vertex들로 만들어진 3D 모델을 실제 화면의 픽셀 단위로 쪼개고 색을 채울 준비를 하는 과정

#### 주요 역할 및 특징

Rasterizer는 **고정 기능(Fixed-function) 단계로, 프로그래머가 셰이더로 코딩하는 대신 파이프라인 설정(State)을 통해 제어**

- **래스터화 (Rasterization/Scan Conversion)** : 3D 삼각형 프리미티브(Primitive)가 화면의 어떤 픽셀들을 덮는지 계산

- **좌표 변환 (Viewport Transform)** : 3차원 투영 좌표(Homogeneous Clip Space)를 2차원 뷰포트(렌더 타깃) 좌표로 매핑

- **클리핑 (Clipping)** : 카메라 뷰 프러스텀(View Frustum, 시야 범위) 밖에 있는 삼각형을 잘라내어, 화면 안에 있는 부분만 렌더링

- **컬링 (Culling)** : 보이지 않는 삼각형(예: 뒷면)을 제거하여 불필요한 계산량을 줄이는데, 일반적으로 뒷면(Back-face) 컬링이 주로 사용 됌

- **보간 (Interpolation)** : 정점 셰이더에서 계산된 데이터(색상, UV 좌표 등)를 삼각형 내부의 픽셀 단위로 선형 보간하여 픽셀 셰이더로 전달함

- **시저 테스트 (Scissor Test)** : 뷰포트 내의 특정 사각형 영역 외부에 있는 픽셀을 추가로 버림

#### Rasterizer 상태 설정 (Rasterizer State)

- `FillMode` : 도형을 채울지(`Solid`) 아니면 `Wireframe` 할지, 결정

- `CullMode` : 앞면(Front)을 그릴지, 뒷면(Back)을 그릴지, 아니면 둘 다 그리지 않을지(None) 결정

- `FrontCounterClockwise` : 시계 반대 방향 정점 순서를 앞면으로 볼지 설정

- `DepthBias` : Z-fighting(두 면이 겹쳐 픽셀이 깜빡이는 현상)을 방지하기 위해 깊이 값에 미세한 차이를 둠

픽셀은 여기서 처음 등장하고 Pixel Shader가 화면에 색을 칠할 수 있는 **기초 데이터를 제공하는 3D-to-2D 변환기**

### (6) Pixel Shader (Ps)

> PS는 가장 비싼 단계

Rasterizer 단계를 거쳐 화면에 그려질 픽셀(Fragment) 각각에 대해 색상을 계산하는 GPU 프로그램이 실행되어 픽셀 단위 최종 색상 계산

- 픽셀 단편마다 Texture에서 색상을 읽거나 Lighting을 적용하여 최종 색상 값을 생성

- Input 데이터: 정점 셰이더에서 넘겨준 위치, 노말, UV 텍스처 좌표 등이 픽셀들 사이에서 보간(Interpolation)되어 입력됌

- 픽셀 셰이더는 조명 계산, 그림자, 표면의 질감 표현(노말 맵), 포스트 프로세싱 등 거의 모든 시각적 효과에 사용

### (7) Output Merger (OM)

**렌더링 파이프라인의 최종 단계로, 픽셀 셰이더에서 계산된 픽셀 데이터, 깊이/스텐실 버퍼, 렌더 타겟을 결합하여 최종적인 픽셀 색상을 생성**

```cpp
FinalColor = Blend( SrcColor, DestColor )
```

픽셀 셰이더(PS)에서 넘어온 조명 및 텍스처 계산 결과(Src)와 기존 렌더 타겟의 값(Dest)을 블렌드 스테이트에 따라 혼합


- **Depth Test** : 이 버퍼를 이용해 앞/뒤 관계를 판정하여, 더 멀리 있는 픽셀을 기각(Discard)함

- **Stencil Test** : 스텐실 버퍼를 통해 특정 영역의 렌더링 여부를 결정

- **멀티 렌더 타겟 (MRT)** : 대 8개의 렌더 타겟(RenderTargetView)과 1개의 깊이/스텐실 뷰를 동시에 설정하여 다양한 버퍼(색상, 노말, 깊이 등)에 동시에 렌더링 가능

- Blend: 투명도 처리(Alpha Blending)

- etc: 그림자 맵(Shadow Map) 생성 시 깊이 판정


---

<br/>

</details>

<details>
<summary> Buffer </summary>

### `ID3D11Buffer`

> 버텍스, 인덱스, 상수 등 3D 그래픽 데이터를 GPU가 빠르게 읽고 처리할 수 있도록 메모리(주로 VRAM)에 저장해두는 구조화되지 않은 일시적 데이터 저장소

DirectX 11에서  `ID3D11Buffer`  **인터페이스로 캡슐화되며, CPU에서 GPU로 데이터를 넘겨줄 때 병목 현상을 줄이는 역할**

주로 `D3D11_BUFFER_DESC` 구조체와 `ID3D11Device::CreateBuffer` 를 사용하여 생성하며, `BindFlags` 를 통해 버퍼의 종류(Vertex, Index 등)를 설정

### Buffer의 종류

| 종류                | 용도             |
| ----------------- | -------------- |
| Vertex Buffer     | 정점 데이터         |
| Index Buffer      | 정점 재사용         |
| Constant Buffer   | CPU -> GPU 파라미터 |
| Structured Buffer | 배열 데이터         |
| Texture Buffer    | 이미지            |


- **Vertex Buffer** : 3D 모델의 위치, 색상, UV 등 버텍스 정보를 저장

- **Index Buffer** : 버텍스 버퍼의 데이터 순서를 가리키는 인덱스를 저장하여 정점 재사용

- **Constant Buffer** : **행렬(World, View, Projection)** 이나 조명 데이터 같은 셰이더 상수 데이터를 셰이더 스테이지에 전달

- **Structured Buffer** : 사용자 정의 구조체(Struct) 배열을 GPU 메모리에 저장하여 셰이더에서 임의 접근(Read/Write)할 수 있게 하는 고유한 버퍼 자원

- **Texture Buffer** :  텍스처 데이터(주로 1차원 데이터, 구조화된 데이터, 혹은 Raw 데이터)를 일반적인 텍스처 샘플링 대신 1차원 배열처럼 직접 접근하여 읽을 수 있도록 최적화된 리소스이며, *Texture Buffer Object (TBO)* 형식으로 생성되며, 데이터 로딩 속도가 빠르고 많은 양의 데이터를 쉐이더에 전달할 때 사용


<br/>

</details>

## Issue in 시지프스

- [포스트 프로세싱 SRV 에러](https://github.com/BOLTB0X/DirectX11-Draw/issues/4) -> [포스트 프로세싱이 잘못된걸 고치는 이야기](https://github.com/BOLTB0X/DirectX11-Draw/blob/main/Lab/DX11_Studies/BicubicModifyingPlaneScene.md)

- [렌즈플레어 적용시 화면 고스트 고정 현상](https://github.com/BOLTB0X/DirectX11-Draw/issues/5) -> [Ghost가 고정되었던 이유에서 존 챔피언 방식으로 고치는 이야기](https://github.com/BOLTB0X/DirectX11-Draw/blob/main/Lab/DX11_Studies/FixedGhost.md)

---

## Study

- [Noise](https://github.com/BOLTB0X/DirectX11-Draw/blob/main/Lab/DX11_Studies/Noise.md)

- [FBM](https://github.com/BOLTB0X/DirectX11-Draw/blob/main/Lab/DX11_Studies/FBM.md)

- [RayMarch](https://github.com/BOLTB0X/DirectX11-Draw/blob/main/Lab/DX11_Studies/Raymarch.md)

- [Lighting](https://github.com/BOLTB0X/DirectX11-Draw/blob/main/Lab/DX11_Studies/Light.md)


---

## 참고

### [**공식문서**](https://learn.microsoft.com/ko-kr/windows/win32/direct3d11/dx-graphics-overviews)

- [공식문서 - 스왑 체인이란?](https://learn.microsoft.com/ko-kr/windows/win32/direct3d9/what-is-a-swap-chain-)

- [공식문서 - ID3D11DeviceContext::IASetVertexBuffers method (d3d11.h)](https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-iasetvertexbuffers)

- [공식문서 - Vertex-shader stage](https://learn.microsoft.com/en-us/windows/win32/direct3d11/vertex-shader-stage)

- [공식문서 - ID3D11DeviceContext::PSSetShader method (d3d11.h)](https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-pssetshader)

- [공식문서 - Introduction to Buffers in Direct3D 11](https://learn.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-resources-buffers-intro)

- [Directx Tutorial - Lesson 3: Preparing the Swap Chain](http://www.directxtutorial.com/lesson.aspx?lessonid=111-4-3)


### **rastertek**

- [rastertek - DirectX 11 on Windows 10 Tutorials](https://www.rastertek.com/tutdx11win10.html)

- [rastertek - DirectX 11 on Windows 10 Tutorials 2](https://www.rastertek.com/tutdx11s2ter.html)

- [rastertek (DirectX 11 on Windows 10 Tutorials) - Tutorial 31: Water](https://www.rastertek.com/dx11win10tut31.html)
- [rastertek (DirectX 11 Terrain Tutorials) - Tutorial 5: Quad Trees](https://www.rastertek.com/tertut05.html)
- [rastertek (DirectX 11 Terrain Tutorials) - Tutorial 16: Small Body Water](https://www.rastertek.com/tertut16.html)

### 환경 셋팅 관련

- [stack overflow - Basic example for a CMake project definition using DirectX 11 on Windows 10?](https://stackoverflow.com/questions/73986114/basic-example-for-a-cmake-project-definition-using-directx-11-on-windows-10#:~:text=cmake_minimum_required%20(VERSION%203.13)%20project(CMakeDirect3DGame%20DESCRIPTION%20%22CMake%20example,_WIN32_WINNT=0x0601)%20endif()%20endif()%20set_property(DIRECTORY%20PROPERTY%20VS_STARTUP_PROJECT%20$%7BPROJECT_NAME%7D))

- [티스토리 참고 : 나는 뉴비다 개발자편 - DirectX 11 최초 실행 환경 세팅하기](https://dev-nicitis.tistory.com/43)

- [유튜브 - 환경 설정 | 홍정모, 프로그래밍 기초, 입문 프로그래밍, 게임 프로그래밍, 프로그래밍 강의, 컴퓨터 그래픽스, 게임 개발, 컴퓨터 작동원리, 코딩](https://www.youtube.com/watch?v=CcFtxKg55fM&list=PLNfg4W25Tapy3qmKqftWndwrIaQRzUmEg&index=9)

- [벨로그 참고 - Imgui 사용법(charlese_note)](https://velog.io/@charlese_note/ImGUI)

- [네이버 블로그 참고 - Dear ImGui를 공부해보자! 시작단계(AROMA)](https://m.blog.naver.com/sweetsmell9/221618574623)

- [티스토리 참고 - Blender AO map Node | 엠비언트 맵 연결하기(lioicreim)](https://lioicreim.tistory.com/615)

### 기본

- [티스토리 참고 - DirectX 11 도형 출력(버텍스 버퍼, 버텍스 쉐이더, 픽셀 쉐이더 설정)(pub-repository)](https://pub-repository.tistory.com/97)

- [티스토리 참고 - [DirectX11] 모델 가져오기(Model Import)(유니얼)](https://unialgames.tistory.com/entry/DirectX11ModelImport)

- [티스토리 참고 - FBX SDK, FBX Importer, FBX 파일 불러오기, 3ds Max 등(최디디)](https://dlemrcnd.tistory.com/85)

- [티스토리 참고 - 070 OBB(Oriented Bounding Box) collision(designerd)](https://designerd.tistory.com/entry/DirectX11-070-OBBOriented-Bounding-Box-collision#obb-%EC%B6%A9%EB%8F%8C---%EB%B6%84%EB%A6%AC%EC%B6%95-%EC%9D%B4%EB%A1%A0)

- [티스토리 참고 - Collision과 SphereCollider(유니얼)](https://unialgames.tistory.com/entry/DirectX11CollisionAndSphereCollider)

- [티스토리 참고 - Terrain 21 - 지형셀(copynull)](https://copynull.tistory.com/324)

- [Velog 참고 - Terrain 지형 툴 연습(hoya1215)](https://velog.io/@hoya1215/Directx11-Terrain-%EC%A7%80%ED%98%95-%ED%88%B4-%EC%97%B0%EC%8A%B5)

- [티스토리 참고 - DirectX11 3D(designerd)](https://designerd.tistory.com/category/%E2%AD%90%20DirectX/DirectX11%20%203D)

- [네이버 블로그 참고 - DirectX 트리 ( 쿼드트리, 옥트리 ) 기본 설명(로기)](https://m.blog.naver.com/PostView.nhn?isHttpsRedirect=true&blogId=jsjhahi&logNo=201309282)

- [티스토리 참고 - 쿼드트리(copynull)](https://copynull.tistory.com/308)

- [블로그 참고 - 1인칭 카메라 조작(mstone8370)](https://mstone8370.tistory.com/26)

- [네이버 블로그(박경준) - DirectX11  파이프라인으로 보는  구조](https://m.blog.naver.com/danhana123/221962753174)

- [reddit - DX11으로 바꾸면 실제로 뭐가 달라지는 거야?](https://www.reddit.com/r/Guildwars2/comments/psd9a0/what_does_change_to_dx11_actually_do/?tl=ko)

- [티스토리 참고 - [알콜코더의 미리 배워보는DX11 입문편] DirectX 11의 특징들(Visual Studio 2010 공식 팀 블로그 @vsts2010:티스토리)](https://vsts2010.tistory.com/510#:~:text=Direct3D%2011%EC%9D%80%20%EC%9C%88%EB%8F%84%EC%9A%B0%20%EB%B9%84%EC%8A%A4%ED%83%80%2C%20%EC%9C%88%EB%8F%84%EC%9A%B0%207%EC%97%90%EC%84%9C%20%EC%A7%80%EC%9B%90%EB%90%98%EB%8A%94,%EB%B3%B4%EB%8B%A4%20%EC%9C%A0%EC%97%B0%ED%95%98%EA%B3%A0%20%EB%9B%B0%EC%96%B4%EB%82%9C%20%EA%B8%B0%EB%8A%A5%EB%93%A4%EC%9D%B4%20%EC%8B%AC%ED%94%8C%ED%95%9C%20API%EC%85%8B%ED%8A%B8%EB%A1%9C%20%EC%A0%9C%EA%B3%B5%EB%90%A9%EB%8B%88%EB%8B%A4.)

- [네이버 블로그 참고(풍풍풍) -Rasterizer](https://blog.naver.com/sorkelf/40162947758)

- [블로그 참고(titathecheese) - Rasterizer](https://titathecheese.tistory.com/category/DirectX%2011)

- [Gaem Development: GLSL Shader - Change Hue/Saturation/Brightness](http://gamedev.stackexchange.com/questions/59797/glsl-shader-change-hue-saturation-brightness)

- [wolfram alpha - 파형확인](https://www.wolframalpha.com/input)

- [Github (jamesscully) - OpenGL Beach Scene](https://github.com/jamesscully/OpenGL-Beach-Scene)

- [네이버 블로그( 프로그래머의 인생, 강동훈 ) - DirectX 11 : Lake from Habib's Water Shader](https://blog.naver.com/fah204)

### Ray

- [블로그 참고 - 불륨마칭(maximeheckel)](https://blog.maximeheckel.com/posts/real-time-cloudscapes-with-volumetric-raymarching/)

- [hree.js forum 참고 - volumetric clouds - game ready](https://discourse.threejs.org/t/volumetric-clouds-game-ready/86598)

- [goto Shader - cloud-material](https://godotshaders.com/shader/cloud-material/)

- [github 참고 - Volumetric-Rendering(Chris' Graphics Blog)](https://wallisc.github.io/rendering/2020/05/02/Volumetric-Rendering-Part-1.html)

- [티스토리 참고 - DirectX 11 Tone Mapping(kyuhwang)](https://kyuhwang.tistory.com/36)

- [stack overflow - Random / noise functions for GLSL](https://stackoverflow.com/questions/4200224/random-noise-functions-for-glsl)

### 에셋 출처

- [노이즈 텍스처 출처 - realtimevfx](https://realtimevfx.com/t/realistic-cloud-texture/19207)

- [Stone 이용: sketchfab - Ground//Stone Sphere(Madee)](https://sketchfab.com/3d-models/groundstone-sphere-1c0f2b2e213348e6a760743a546dc7a6)

- [normalMap 이용: filterforge `Cloud_normal.jpg`](https://www.filterforge.com/filters/6395-normal.html)

- [Mountain 이용: sketchfab - Cloudy Mountain(Pers Scans)](https://sketchfab.com/3d-models/cloudy-mountain-f61d84c6534d4007accdeac35f433119)
