# FBM(Fractional Brownian Motion)

> 자연스러운 무작위 패턴이나 질감을 만들어내기 위해 여러 층의 노이즈(`Noise`) 함수를 쌓아 올리는(Accumulate) 기법

컴퓨터 그래픽스 쪽에선, **절차적 생성(Procedural Generation)** 에서

산, 구름, 흙, 불, 물과 같이 불규칙적이고 유기적인 표면을 묘사하는 데 핵심적으로 사용되는 기술이라 함

**노이즈가 "무작위 패턴"이라면, FBM은 그 무작위성을 "자연스러운 디테일"로 바꾸는 마법**

---

## 핵심 개념

- *프랙탈(Fractal)* 

    전체적인 모양과 작은 부분의 모양이 유사한 **'자기 유사성(Self-similarity)'** 을 가짐

- **노이즈 누적(Summation of Noise)**

    - *단일 노이즈(ex: Perlin Noise)* 는 너무 매끄러워서 자연물 같지 않음
    
    - **FBM** 은 *주파수(Frequency)* 와 *진폭(Amplitude)* 을 조절하면서 여러 개의 노이즈 *층(Octaves)* 을 합쳐 더 세밀하고 복잡한 질감을 생성

----

## 작동 원리


1. **기본 노이즈**

    *펄린 노이즈(Perlin Noise)* 나 *심플렉스 노이즈(Simplex Noise)* 를 기반으로 시작

2. **옥타브(Octaves)**
    
    노이즈를 몇 번 쌓을지 결정하고 옥타브가 많을수록 디테일이 높아짐

3. **래큐너리티(Lacunarity)**

    옥타브가 증가할 때마다 노이즈의 주파수(공간적 크기)가 얼마나 빨리 커지는지 결정
    
    - 보통 2.0배를 사용하여 각 층이 이전 층의 두 배 세밀함을 가지게 함

4. **게인(Gain/Persistence)**

    옥타브가 증가할 때마다 노이즈의 진폭(영향력)이 얼마나 줄어드는지 결정
    
    - 세부적인 층일수록 영향력이 작아지도록 보통 0.5배를 곱해준다 함

----

## 주요 매개변수

- `Amplitude` (진폭): 각 층의 노이즈가 가지는 높이(밝기)의 세기

- `Frequency` (주파수): 각 층의 노이즈가 얼마나 밀집되어 있는지 결정

- `Octaves` (옥타브): 섞을 노이즈의 레이어 수

- `Persistence/Gain` (지속성/게인): 옥타브가 올라갈 때 진폭을 줄이는 비율

---

## ex

```cpp
float fbm(vec2 p)
{
    float value = 0.0;
    float amplitude = 0.5;
    float frequency = 0.0;
    
    // 옥타브 반복
    for (int i = 0; i < numOctaves; i++)
    {
        value += amplitude * noise(p);
        p *= 2.0; // 래큐너리티 (주파수 증가)
        amplitude *= 0.5; // 게인 (진폭 감소)
    }
    return value;
}
```

### cf. 바이큐빅 필터링(Bicubic Filtering or Bicubic Interpolation)

디지털 이미지 처리에서 이미지를 확대하거나 축소할 때,

주변 16개(4x4)의 픽셀 데이터를 사용하여 새로운 픽셀 값을 계산하는 고품질 보간법

*3D 그래픽스에서 텍스처를 확대할 때 더 높은 품질을 내기 위해 하드웨어 지원이 확대되는 추세*

- cf. 보간(Interpolation, 인터폴레이션)이란

    *이미지의 크기를 변경(확대/축소)하거나 회전/왜곡 변형을 가할 때, 원래 픽셀들 사이에 생기는 빈 공간을 주변 픽셀 값을 기반으로 계산하여 새로운 픽셀로 채워 넣는 수학적 기법*

---

## 참고

- [medium 참고  - Procedural Generation Using Fractional Brownian Motion](https://medium.com/@logan.margo314/procedural-generation-using-fractional-brownian-motion-b35b7231309f)

- [thebookofshaders - Fractal Brownian Motion](https://thebookofshaders.com/13/#:~:text=This%20technique%20is%20commonly%20used%20to%20construct,article%20by%20Inigo%20Quiles%20about%20advanced%20noise.)

- [learn.foundry - FBM](https://learn.foundry.com/modo/14.0/content/help/pages/shading_lighting/emodo_textures/fbm.html)

- [wallisc - Volumetric Rendering](https://wallisc.github.io/rendering/2020/05/02/Volumetric-Rendering-Part-1.html)

- [나무위키 - 이미지 보간](https://namu.wiki/w/%EC%9D%B4%EB%AF%B8%EC%A7%80%20%EB%B3%B4%EA%B0%84?uuid=604c4d0c-a487-4919-bef2-c29472a25879)