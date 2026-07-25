# Error 및 궁금증들 기록장

에러는 `E숫자` , 궁금해서 찾아본건 `Q숫자`로 구분

주로 gemini, Gpt 등 ~~(무료버전)~~ 과 구글링으로 나름대로 해결을 기록 

## Error

### E1. 삼각형이 사방팔방으로 튀기다 꺼지는 문제

> **화면이 깨지는 현상** 은 GPU가 버텍스 데이터를 읽을 때,<br/> **좌표값을 읽어야 할 자리에서 텍스처 좌표나 노멀 값을 읽어버려서 삼각형이 사방팔방으로 튀기 때문에 발생**

GPU에게 이 구조체의 생김새를 알려주는(Input Layout) 과정 사이의 미세한 불일치 때문

-> 나의 해결법: 기존 셰이더 관련 구조체나 바인딩 부분을 셰이더에 맞게 했는지 확인 및 버퍼 설정등을 확인해봐야함

ex. *삼각형 리스트(Triangle List)로 그리기로 변경하면서 한 사각형 당 정점이 6개가 되었으나, `InitBuffers` 내부 계산식에는 예전 방식인 8개(Line List용)가 그대로 남아 있었음.*

---

### E2. 이중 해제(Double Free)

프로그램 종료 시 `ucrtbased.dll`에서 액세스 위반이 발생했던 문제

- **원인**

    `Texture::Init` 함수 끝에서 `stbi_image_free(imageData)`를 호출해 메모리를 해제했지만,
    
    포인터 변수(`imageData`)를 `nullptr`로 초기화하지 않음

    프로그램 종료 시 `Texture::Shutdown` 에서 `if (imageData)` 조건문을 통과하게 되고
    
    이미 해제된 주소를 다시 `stbi_image_free`하려 시도하면서 런타임 에러 발생

- **해결**

`stbi_image_free(imageData)` 호출 직후 반드시 `imageData = nullptr;` 를 대입

이렇게 하면 `Shutdown` 시점에 조건문(`if (imageData)`)을 통과하지 못하게 되어 안전하게 중복 해제를 방지함

~~현재는 그냥 스마트포인터 사용~~

---

### E3. 깨진 fbx 파일

이틀동안 화면에 그린 모델이 보이질 않아 별에별 짓을 다해보다 기존 잘 되는 모델과 동시에 그려보니 그것만 안나오는 것을 발견

~~그냥 모델 교체~~

---


### E4. 백터 범위 초가

![](https://github.com/BOLTB0X/Graphics-Draw/blob/main/DemoGIF/%ED%84%B0%EB%A0%88%EC%9D%B8%EB%A0%8C%EB%8D%94%EB%8F%84%EC%A4%91_%EB%B2%94%EC%9C%84%EC%B4%88%EA%B3%BC%EC%97%90%EB%9F%AC.png?raw=true)

자동할당이라 맘 편히 쓰다, `TerrainModel`에서 정규화 하는 부분에서 기존 코드로 호출해서 마주했음

---

### E5. 카메라로 보는데 엔진에서 절두체 컬링을 적용해버리는 경우

![스크린샷 하나가 어디있는지](https://github.com/BOLTB0X/Graphics-Draw/blob/main/DemoGIF/%EC%B9%B4%EB%A9%94%EB%9D%BC_%EC%A0%88%EB%91%90%EC%B2%B4%ED%99%95%EC%9D%B8.png?raw=true)

`Camera::Render` 가 호출될 때마다 `BuildFrustum` 을 수행하는데,

이때 사용되는 `m_viewMatrix`가 최신 **카메라 상태(`Position`)** 를 제대로 반영하지 못하고 있다면 절두체는 엉뚱한 곳을 검사하게 됌

로컬공간 -> 월드공간 변환 행렬 보정으로 해결
더 정확하게 하려면 쿼드트리를 써야한다고 함

[tistory 참조 - devshovelinglife](https://devshovelinglife.tistory.com/110#:~:text=%EC%BF%BC%EB%93%9C%20%ED%8A%B8%EB%A6%AC%20%EC%BB%AC%EB%A7%81%EC%9D%80%20%EC%A3%BC%EB%A1%9C%20%EC%A7%80%ED%98%95%EC%97%90%20%EB%8C%80%ED%95%B4%20%EB%A7%8E%EC%9D%B4%20%EC%82%AC%EC%9A%A9%EB%90%98%EB%8A%94%20%EC%BB%AC%EB%A7%81%20%EB%B0%A9%EC%8B%9D%EC%9D%B4%EB%8B%A4.%20%EC%A7%80%ED%98%95&text=%EB%B6%80%EB%AA%A8%EA%B0%80%20%EC%A0%88%EB%91%90%EC%B2%B4%20%EB%B0%96%EC%97%90%20%EC%9E%88%EB%8B%A4%EB%A9%B4%2C%20%EC%A0%88%EB%91%90%EC%B2%B4%EA%B0%80%20%EB%AC%B4%ED%95%9C%20%ED%8F%89%EB%A9%B4%EC%9C%BC%EB%A1%9C%20%EC%9D%B4%EB%A3%A8%EC%96%B4%EC%A7%84%20%EB%A7%8C%ED%81%BC)

### E6. Imgui 화면에 마우스가 잘 적용이 안되는 경우

실제 내 마우스 커서 위치와 Imgui가 인식하는 지점이 달라 상당히 애를 먹었음

구글링을 통해서 어떤 [블로그 포스팅 - Imgui 사용도중 마우스 위치가 안맞는 버그](https://develop-dream.tistory.com/113)을 발견

화면 비율을 내가 설정한 가로, 세로로 변경했는데 Imgui는 이를 반영하지 못해서 일어나는 거였음

```cpp
    RECT clientRect;
    GetClientRect(m_hwnd, &clientRect);
    ConstantHelper::SCREEN_WIDTH = clientRect.right - clientRect.left;
    ConstantHelper::SCREEN_HEIGHT = clientRect.bottom - clientRect.top;
```

위 코드를 `Window` 생성 에 추가하니 해결

----

## 궁금했던 것들

### Q1. 그래픽스 분야에선 상속/추상화보다 의존성(Composition) 관리를 선호하나?

> 그래픽스 및 게임 엔진 개발에서는 "Composition over Inheritance(상속보다는 합성)" 원칙을 훨씬 강력하게 적용함

- **상속의 위험성**

    ```
    WorldObject → PhysicalObject → Terrain
    ```

    위처럼 상속 계층이 깊어지면, 나중에 "물리는 없는데 렌더링만 되는 객체"
    
    혹은 "렌더링은 없는데 소리만 나는 객체"를 만들 때 구조가 꼬여버림
    
    (이를 다이아몬드 상속 문제라 함)

    ---

- **컴포넌트 방식**

    현대 엔진(Unity, Unreal)은 객체를 상속하지 않고,
    
    `GameObject` 라는 빈 껍데기에 `MeshComponent`, `PhysicsComponent`, `LightComponent`를 **포함(Include/Member)**시켜서 기능을 조립

    ---

- **성능(캐시 효율성)**

    너무 잦은 상속을 통한 `virtual` 호출은 **V-Table 오버헤드** 를 발생시킬수 있음
    
    수천 개의 객체를 렌더링해야 하는 그래픽스 분야에서는 멤버 변수로 클래스를 가지고 있거나,
    
    아예 독립적인 시스템이 데이터를 순차적으로 처리하는 방식을 선호

    ---

- 그럼 상속이나 맴버로 언제 쓰나

    - **상속을 써도 되는 경우 (아주 제한적)**
    
        “이것은 이것이다 (is-a)” 가 절대적으로 성립할 때

    - **멤버로 둬야 하는 경우**
    
        “이것은 이것을 사용한다 (has-a)”

---

### Q2. Shader는 컴포넌트보다는 '참조'가 적합한가?

Shader는 *"행동이나 상태를 가진 존재”* 가 아니라

*“데이터를 해석하는 규칙”* 이기 때문에 컴포넌트가 아니라 참조가 맞다고 함

- 컴포넌트의 공통 속성

    - Actor에 속함 (`Transform / SceneComponent`)

    - Actor의 생명주기를 따름 (`RigidBody / Collider`)

    - Actor마다 개별 인스턴스를 가질 수 있음(`CameraComponent`)

    - 보통 월드 상태를 직접 바꾸기 가능(`Script` / `Behaviour`)

    ---

Shader는 Actor의 상태를 바꾸지 않고 단지 GPU가 데이터를 해석하는 방법만 제공함

```
//Shader 구조
Material Function / Render Rule / Interpretation Logic
```

-> **"이 데이터를 어떻게 화면에 그릴 것인가"** 가 관건

---

### Q3. 리소스와 생명주기

Q. *컴포넌트의 공통 특징 상 마야모델, 텍스처, 셰이더를 `Application`에서 로드 하고 포인터로 `World`로 보내어 `WorldObject`에 전달해 주면 `Application` 상의 내용들과 생명주기가 달라서 문제가 되질않나?*

#### 1 "생명주기를 따른다"는 규칙은 `Position` 같은 순수 컴포넌트에 해당

-> `Model`은 컴포넌트가 아니라 액터가 사용하는 외형 리소스

- **순수 컴포넌트** (ex. `Position`)    

    - `ActorObject`가 `unique_ptr`로 소유

    - 액터가 죽으면 같이 죽음 (생명주기 일치)

- **리소스 참조 (Model)**

    - 액터는 주소값(Raw Pointer)만 들고 있는 것
    
    - 액터가 죽어도 원본 데이터는 ModelManager 안에 살아있음(생명주기 독립)

    ---

#### 2. 그럼 왜 이렇게 해야 하나?

-> 내가 생각했던 것처럼 데이터를 캐시(Cache) 처리해 두었기 때문에 괜찮은 것

- **재사용성**

    만약 액터가 죽을 때 모델 데이터까지 날려버리면,
    
    잠시 후 똑같은 `Stone`을 소환할 때 Assimp로 다시 파일을 읽고 GPU 메모리에 올리는 비싼 비용이 발생

    ---

- **안전성**

    `ModelManager`가 `unique_ptr 맵`으로 모델을 꽉 잡고 있기 때문에, `WorldObject`가 수만 번 생성되고 해제되어도 실제 메모리에 로드된 정점 데이터는 하나뿐이며 절대 유실되지 않음

#### 3. "Render 취소"에 대한 걱정

*"Actor가 소멸될 때 m_model.render를 취소해야 하나?"*

렌더링은 매 프레임 살아있는 객체들만 모아서 수행되기 때문에 그럴 필요가 X

`World`가 소유한 `m_actors` 리스트에서 특정 액터가 제거(Delete)됨

다음 프레임의 `World::Render` 루프에서는 그 액터가 리스트에 없으므로, 자연스럽게 해당 모델의 `Render()` 함수 자체가 호출되지 않음

데이터는 메모리에 남아있지만(캐시), GPU는 명령을 받지 않으므로 그리기를 멈춤

#### 4. Smart Pointer 활용

- `std::shared_ptr<Model>`: 매니저와 액터가 소유권을 공유

- `std::weak_ptr<Model>`: 매니저는 가지고만 있고, 액터는 필요할 때만 체크해서 씀

이런 방식을 잘 조합해서 사용해야함

---

### Q4. `io.WantCaptureMouse` 의 의미

> Q: UI가 켜져 있을 때 마우스 상태만 나타내는 건지, UI 창 위에 마우스가 있으면 활상화가 되는 건지

`io.WantCaptureMouse` 는 단순히 **"UI가 켜져 있는가(Visible)"** 를 뜻하는 게 아닌,

**"현재 마우스 커서가 ImGui 창 영역 안에 들어와 있는가?"** 를 뜻함

- UI가 켜져 있을 경우

    > 마우스가 UI 창 밖(배경의 3D 월드)에 있으면 `false`가 됩니다. 이때는 월드의 돌을 클릭하거나 화면을 돌릴 수 있습니다.

- UI 창 위에 마우스가 있으면

    > `true`가 됩니다. 이때 클릭하면 월드에 신호가 가지 않고 UI 버튼만 눌립니다.

---

### Q5. 높이맵 적용하려면, 무조건 모델안에 height맵이 있어야하나?

> ❌ 모델이 height map 정보를 ‘가지고 있을 필요는 없다’<br/>
⭕ Height map은 텍스처(외부 데이터) 이고<br/>
⭕ Shader 또는 CPU 로직이 그걸 어떻게 쓰느냐의 문제다

어디까지가 버텍스만으로 가능하고, 어디서부터 다른 리소스가 필요한지를 정확히 구분하는 게 중요성을 알게 됌

1. 기본 전제: GPU는 결국 “버텍스 스트림”으로 시작

    ```
    Vertex Buffer + Input Layout -> Vertex Shader
    ```

    ---

2. “버텍스 정보로 처리한다”의 정확한 의미

    - 버텍스 위치 변환: `World * View * Projection`

    - 기본 라이팅 계산 (Vertex Lighting)

    - 노멀 기반 조명

    - 스켈레탈 애니메이션 (Bone Weight / Index)

    - Height Map 기반 Vertex Displacement

    - LOD 처리 (정점 수 차이)

기하 구조의 변화는 전부 버텍스 단계에서 처리 가능

---

### ~~Q6. 레스터택 vs 내가 한거~~

~~레스터택은 주로 비트맵(RAW, BMP) 파일을 직접 읽어 높이맵을 생성~~

~~내가 한것은 FBX 메쉬를 높이맵의 원천으로 사용~~

| 구분            | Rastertek 방식                         | Me                              |
|-----------------|----------------------------------------|----------------------------------|
| 데이터 소스     | RAW / Bitmap 이미지 파일               | FBX 모델 (Assimp)                |
| 데이터 추출     | 이미지 픽셀 밝기 -> Y값                 | FBX Vertices -> Y값               |
| 법선 (Normal)   | 직접 계산 (CalculateNormals)           | Assimp 로드 시 제공              |
| 접선 (Tangent)  | 직접 계산 (CalculateTangentBinormal)   | Assimp 로드 시 제공              |
| 물리 판정       | `CheckHeightOfTriangle` 사용             | 동일하게 사용 (필수)             |

----

### Q7. Mesh와 그리드(Cell) 차이?

`MeshModel`은 여러 개의 메시가 각각 다른 머터리얼을 가질 수 있어 루프 안에서 바인딩하지만,

```
MeshModel
 ├─ Mesh 0 → Material 0
 ├─ Mesh 1 → Material 2
 ├─ Mesh 2 → Material 1
```

`TerrainModel`은 지형 전체가 하나의 큰 머터리얼(또는 텍스처 셋)을 공유하는 구조

```
TerrainModel
 ├─ Material (1개)
 ├─ Cell 0
 ├─ Cell 1
 ├─ Cell 2
 ```

즉

- `MeshModel`
   
   > "이 조각은 가죽이야(바인딩) -> 그려!", "이 조각은 쇠야(바인딩) -> 그려!" (반복)

- `TerrainModel`

    > "자, 지금부터 그리는 건 전부 다 이 지형 텍스처야(바인딩)!" -> "1번 셀 보여?(Culling) -> 그려!", "2번 셀 보여? -> 안 보이면 패스!", "3번 셀 보여? -> 그려!" (반복)

----

### Q8. 언제 `전방 선언` 을 쓰고 언제 `include` 하나?

- 전방 선언(class A;) 가능

    멤버 변수가 **포인터(`A*`)** 나 **스마트 포인터(`unique_ptr<A>`)** 인 경우 (포인터 크기는 8바이트로 일정)

- `Include(#include "A.h")` 필수

    멤버 변수가 **값(`A a;`)** 일 때, or **해당 클래스의 내부 멤버 함수를 직접 호출할 때**

---

### Q9. 각 셰이더 종류별 역할

| 셰이더 종류 | 입력 (Input)          | 출력 (Output)        | 핵심 역할 |
|-------------|-----------------------|----------------------|-----------|
| Vertex Shader (VS) | 점(Point) 1개 | 점(Point) 1개 | CPU에서 보낸 위치 정보를 단순히 전달만 함 |
| Geometry Shader (GS) | 점(Point) 1개 | 사각형(Quad) 1개 | 점 1개를 받아 실시간으로 정점 4개를 생성하여 면으로 확장 |
| Pixel Shader (PS) | 보간된 정점 데이터 | 색상(Color) | 확장된 사각형 면에 구름 텍스처를 입히고 알파 값을 계산 |

---- 

### Q.10 노이즈(Noise)란 무엇인가?

**“의도적으로 만든 무작위 함수”**

| 랜덤       | 노이즈            |
| -------- | -------------- |
| 픽셀마다 불연속 | 위치가 가까우면 값도 비슷 |
| 깜빡거림     | 부드러운 변화        |
| 자연스러움 X  | 자연물 표현 O       |


- 그래픽스에서 노이즈를 쓰는 이유

    - 구름

    - 지형 높낮이

    - 바위 표면

    - 물결

    - 연기

    - 불꽃
    
    **“자연은 규칙 없는 패턴”**

### Q.10 볼류매트릭에서, 화면 쿼드(Quad) vs 거대한 큐브(Bounding Box) 방식

기존에 내가 했던 방식(화면 전체에 쿼드를 그리고 레이마칭)은 포스트 프로세싱(후처리)에 가깝다고 함 그런데 **최신 게임 엔진들이 월드 공간에 거대한 큐브(가상의 영역)를 그리는 이유가 있는데,**

- **성능 최적화**:

    - **쿼드 방식** : 화면의 모든 픽셀에서 카메라 위치부터 무한대까지 `Ray` 를 쏴야 함 구름이 없는 하늘 공간에서도 쓸데없이 루프를 돌며 검사를 해야 하므로 GPU 낭비가 심함

    - **큐브 방식 (Bounding Box)** : 하늘 특정 고도에 거대한 육면체 메쉬를 두고 래스터라이저가 이 큐브를 화면에 그릴 때, 큐브가 차지하는 픽셀에서만 픽셀 셰이더(레이마칭)가 실행되게 함

- 정확한 시작점과 끝점 (Entry & Exit Points):

    - 큐브 메쉬를 렌더링하면, 카메라 `Ray` 가 큐브의 *'앞면'* 을 뚫고 **들어가는 위치(Entry)** 와 *'뒷면'* 을 **뚫고 나오는 위치(Exit)** 를 수학적으로 아주 쉽게 구할 수 있음

    - 즉, Ray가 정확히 구름이 존재할 수 있는 고도 내에서만 전진(Marching)하게 되므로 연산량이 획기적으로 줄어듬

### Q.11 펄린 노이즈(Perlin Noise)란?

일반적인 무작위(Random) 노이즈와 달리, 인접한 값들이 서로 부드럽게 연결되는 연속성(Coherence)을 가져 자연스러운 지형, 구름, 텍스처를 생성하는 데 최적화된 유기적 노이즈

----
