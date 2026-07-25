# Ray Marching

> “광선을 쏘고, SDF 거리만큼 점프하면서 물체를 찾는 방법”


Mesh를 사용하지 않고 수학적 함수나 SDF(Signed Distance Field)을 이용해 장면을 렌더링하는 기법

cf. *볼륨 렌더링이나 거리 기반 쉐이딩 등에서 매우 유용하게 사용*

## Ray Marching 란?

<p align="center">
  <table style="width:100%; text-align:center; border-spacing:20px;">
    <tr>
        <td style="text-align:center; vertical-align:middle;">
        <p align="center">
        <img src="https://github.com/BOLTB0X/DirectX11-Draw/blob/main/DemoGIF/%EB%A0%88%EC%9D%B4%EB%A7%88%EC%B9%AD%EA%B8%B0%EB%B3%B8%EC%9D%B4%EB%AF%B8%EC%A7%801.png?raw=true" 
             alt="image 2" 
             style="; object-fit:contain; border:1px solid #ddd; border-radius:4px;"/>
        </p>
      </td>
    </tr>
    <tr>
      <td style="text-align:center; font-size:14px; font-weight:bold;">
      <p align="center">
      <a></a>Ray Marching</a>
      </p>
      </td>
    </tr>
  </table>
</p>

카메라로부터 스크린 픽셀들을 향해 **Ray** 를 **전진시키고(Ray Marching)**

해당 픽셀의 **Ray** 가 *Object 표면* 에 닿으면 그 픽셀에 *Object 표면* 을 렌더링하는 방식

---

## Ray는 어디서 어떻게 나오냐 (Screen / View Space)

> 각 픽셀마다 하나의 **Ray** 를 생성하고<br/> 해당 **Ray** 가 공간을 따라 전진하며 **SDF** 로부터 ‘안전한 이동 거리’를 반복적으로 얻어 물체의 표면에 도달했는지를 판단

**Ray** 는 임의로 공간에 던져지는 것이 아니라,

화면의 각 픽셀을 기준으로 정확하게 정의 되는 것

즉 **Ray는 방향만 알고 있을 뿐이며,
SDF가 제공하는 거리 정보가 Ray의 발걸음을 결정**

### 1. Screen Space – 화면은 픽셀의 집합

```
Screen = { Pixel(x, y) | 0 ≤ x < width, 0 ≤ y < height }
```

Ray Marching에서는 각 픽셀마다 하나의 Ray를 생성하며,

이 Ray의 결과가 곧 해당 픽셀의 최종 색상이 되는 것

즉, *“Ray 하나 = 픽셀 하나”* 의 관계를 가짐

### 2. Pixel -> UV 변환

픽셀 좌표 `(x, y)` 는 정규화된 UV 좌표 `(0~1)` 로 변환

```
u = x / width
v = y / height
```

이 UV 좌표는 이후 **View Space** 에서 **Ray** 방향을 계산하는 기준이 되는 것

### 3. Ray Origin – 카메라 위치

> 하나의 카메라 위치에서,
수많은 방향의 Ray가 화면을 향해 발사

Ray Marching이 카메라 기준 렌더링

### 4. Ray Direction – 픽셀이 가리키는 View 방향

<p align="center">
  <table style="width:100%; text-align:center; border-spacing:20px;">
    <tr>
      <td style="text-align:center; vertical-align:middle;">
        <p align="center">
        <img src="https://github.com/BOLTB0X/DirectX11-Draw/blob/main/DemoGIF/%EB%A0%88%EC%9D%B4%EB%A7%88%EC%B9%AD%EA%B8%B0%EB%B3%B8%EC%9D%B4%EB%AF%B8%EC%A7%802.png?raw=true" 
             alt="image 2" 
             style="; object-fit:contain; border:1px solid #ddd; border-radius:4px;"/>
        </p>
      </td>
    </tr>
    <tr>
      <td style="text-align:center; font-size:14px; font-weight:bold;">
      <p align="center">
      <a></a>Ray Marching</a>
      </p>
      </td>
    </tr>
  </table>
</p>

**Ray** 의 방향은 단순히 `(0,0,1)` 같은 고정 벡터가 아니라,

해당 픽셀이 바라보는 **View Space** 방향으로 결정

```cpp
Ray Direction = normalize(
    ViewSpacePosition(uv) - CameraPosition
)
```


- 화면 중앙 픽셀 -> 정면 방향 Ray

- 화면 가장자리 픽셀 -> 시야각(FOV)에 따라 퍼지는 Ray

즉, Ray는 이미지를 “통과”하는 것이 아니라,
**픽셀이 곧 Ray의 방향을 정의**

---

## SDF는 Ray에게 무엇을 알려주는가

Ray Marching에서 Ray는 어디로 이동할지 스스로 알지 못하기에,

Ray가 다음에 얼마나 전진할 수 있는지는
**SDF(Signed Distance Field)가 제공하는 ‘거리 정보’** 에 의해 결정

### 1. SDF (Signed Distance Field)란?

공간상의 한 점 `p` 에 대해
가장 가까운 물체 표면까지의 최소 거리를 반환하는 함수

<p align="center">
  <table style="width:100%; text-align:center; border-spacing:20px;">
    <tr>
      <td style="text-align:center; vertical-align:middle;">
        <p align="center">
        <img src="https://github.com/BOLTB0X/DirectX11-Draw/blob/main/DemoGIF/sdf%EA%B5%AC%EC%B2%B4.png?raw=true" 
             alt="image 2" 
             style="; object-fit:contain; border:1px solid #ddd; border-radius:4px;"/>
        </p>
      </td>
    </tr>
    <tr>
      <td style="text-align:center; font-size:14px; font-weight:bold;">
      <p align="center">
      <a></a>SDF 구체 Ex</a>
      </p>
      </td>
    </tr>
  </table>
</p>

```
d > 0  : 물체 외부
d = 0  : 물체 표면
d < 0  : 물체 내부
```

“이 위치에서 표면까지 얼마나 떨어져 있는가”를 알려주는 것

### 2. SDF는 Ray에게 ‘안전한 이동 거리’를 제공

현재 위치에서 SDF가 알려주는 거리만큼 이동해도
물체를 절대 관통하지 않음

```
p_next = p_current + rayDir * SDF(p_current)
```

이 성질 덕분에 Ray는:

아주 작은 스텝으로 조금씩 이동하지 않고

최대 안전 거리만큼 점프하며 전진할 수 있음

이러한 방식이 바로 **Ray Marching**

### 3. 반복 과정 (Ray March Loop)

Ray는 다음 과정을 반복

<p align="center">
  <table style="width:100%; text-align:center; border-spacing:20px;">
    <tr>
      <td style="text-align:center; vertical-align:middle;">
        <p align="center">
        <img src="https://github.com/BOLTB0X/DirectX11-Draw/blob/main/DemoGIF/%EB%A0%88%EC%9D%B4%EB%A7%88%EC%B9%ADgpt%EC%9D%B4%EC%9A%A9%EC%9D%B4%EB%AF%B8%EC%A7%80%EC%83%9D%EC%84%B1.png?raw=true" 
             alt="image 2" 
             style="; object-fit:contain; border:1px solid #ddd; border-radius:4px;"/>
        </p>
      </td>
    </tr>
    <tr>
      <td style="text-align:center; font-size:14px; font-weight:bold;">
      <p align="center">
      <a></a>반복 과정</a>
      </p>
      </td>
    </tr>
  </table>
</p>

1 현재 위치 `p`에서 `SDF(p`) 계산

2 `SDF(p) < e` 이면 표면에 도달 (Hit)

3 아니라면 `SDF(p)` 만큼 `Ray` 방향으로 이동


```cpp
for each step:
    d = SDF(p)
    if d < epsilon:
        hit surface
    p += rayDir * d
```

이 과정을 최대 반복 횟수 또는 최대 거리에 도달할 때까지 수행

### 4. 왜 Ray Marching이 효율적인가

- 물체와의 충돌 여부를 거리 계산 하나로 판단

- 복잡한 메시 충돌 검사 불필요

- 반복 횟수가 적어짐

- 수학적 정의만으로 형태 구성 가능

Ray는 “어둠 속을 더듬는 존재”가 아니라,
SDF가 제공하는 거리 정보를 신뢰하며 전진

### cf. SDF와 형태 정의

단순한 구체부터 복잡한 장면까지 표현 가능

- Sphere : `length(p - center) - radius`

- Box : `distance to AABB`

- Plane : `dot(p, normal) + offset`

여러 SDF를 조합하면: `Union`, `Subtraction`, `Intersection`, `Boolean` 연산도 가능

---

## UV / Pixel 기준 -> Ray 생성 -> SDF Ray Marching (Shader 기준)

Pixel Shader 실행 순서

1. UV 획득
2. UV -> Ray Direction 계산
3. Ray March Loop & SDF Hit 여부 판단
4. 픽셀 색상 결정

### 1. UV 획득

```cpp
struct PSInput
{
    float4 pos : SV_POSITION; // Screen Space
    float2 uv  : TEXCOORD0;   // 0~1
};

// ...

float4 main(PSInput input) : SV_TARGET
{
    // ...
}
```


### 2. UV -> Ray Direction 계산

```cpp
// UV (0~1) → NDC (-1~1)
float2 ndc;
ndc.x = input.uv.x * 2.0f - 1.0f;
ndc.y = 1.0f - input.uv.y * 2.0f;

// NDC → World Space Position
float4 clip = float4(ndc, 0.0f, 1.0f);
float4 world = mul(clip, invViewProj);
world.xyz /= world.w;
```

- 화면 중앙 -> `(0, 0)`

- 화면 가장자리 -> 시야각(FOV) 반영된 방향

```cpp
// Ray 정의
float3 rayOrigin = cameraPos;
float3 rayDir = normalize(world.xyz - cameraPos);
```

픽셀은 `World Space` 상의 한 점으로 변환되고

카메라 위치에서 그 점을 향하는 벡터가 **Ray Direction** 이 됌

- `Ray Origin` = `Camera Position`

- `Ray Direction` = `normalize(WorldPos - CameraPos)`


### 3. Ray March Loop & SDF Hit 여부 판단

```cpp
float SDF(float3 p)
{
    return SDF_Sphere(p, 1.0f);
}

bool RayMarch(float3 rayOrigin, float3 rayDir, out float3 hitPos)
{
    float totalDist = 0.0f;

    for (int i = 0; i < 64; i++)
    {
        float3 p = rayOrigin + rayDir * totalDist;
        float d = SceneSDF(p);

        if (d < 0.001f)
        {
            hitPos = p;
            return true; // Hit
        }

        totalDist += d;

        if (totalDist > 100.0f)
            break;
    }

    return false;
}
```

```cpp
bool RayMarch(float3 rayOrigin, float3 rayDir, out float3 hitPos)
{
    // ...

    p = rayOrigin + rayDir * totalDist;
    d = SceneSDF(p);

    // ...
    totalDist += d;

    // ...

}
```

현재 위치에서 `SDF`가 알려준 안전 거리만큼 **Ray** 를 전진시킴

```cpp
bool RayMarch(float3 rayOrigin, float3 rayDir, out float3 hitPos)
{
    // ...
    
    if (d < 0.001f)
    {
        hitPos = p;
        return true; // Hit
    }

    // ...

}
```

Ray가 물체 표면에 충분히 가까워졌다고 판단

이 픽셀이 물체를 “본 것” 으로 처리

### 4. 픽셀 색상 결정

```cpp
if (RayMarch(rayOrigin, rayDir, hitPos))
{
    return float4(1, 1, 1, 1); // Hit -> 흰색
}

return float4(0, 0, 0, 1); // Miss -> 검정
```

### 정리

Ray는 픽셀마다 생성되면 `Screen` / `View` / `World Space` 변환을 이해할 수 있음

Ray Marching은 단순한 효과가 아니라 공간 탐색 알고리즘이며 메시 없이도 형태와 충돌을 정의할 수 있

---

## 참고

- [Shader Toy - Ray Marching - Basics(bbateni)](https://www.shadertoy.com/view/clfSWM)

- [Rito15 참고 - 레이 마칭(Ray Marching)](https://rito15.github.io/posts/ray-marching/)

- [김굴치의 코딩세상 참고 - 레이마칭(Ray marching)을 만들어보았다.](https://chaeso0.tistory.com/entry/%F0%9F%93%92%EA%B0%9C%EB%B0%9C%EC%9D%BC%EC%A7%80-14-%EB%A0%88%EC%9D%B4%EB%A7%88%EC%B9%ADRay-marching%EC%9D%84-%EA%B5%AC%ED%98%84%ED%95%B4%EB%B3%B4%EC%9E%90)

- [유튜브 유니티 채널 -  Volumetrics: Introduction to ray marching | Tutorial](https://www.youtube.com/watch?v=hXYOlXVRRL8&t=88)

- [Michael Walczyk - Ray Marching](https://michaelwalczyk.com/blog-ray-marching.html)