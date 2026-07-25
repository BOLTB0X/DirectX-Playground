# FBM을 통한 지형 렌더링

구글링 도중 [goto Shader - cloud-material](https://godotshaders.com/shader/cloud-material/) 관련 코드를 보고 왜 인지, 에셋 의존하지 않고 직접 그리는 것에 끌리게 됌

## 초기 내가 생각했던 Mesh 들로 Terrain 렌더링 방식

[rastertek - DirectX 11 on Windows 10 Tutorials 2](https://www.rastertek.com/tutdx11s2ter.html) 을 따라 해보다 한 번 다른 텍스처로 입혀볼 건데 이를 구름으로 해보면 어떻게 나오나가 궁금해져 여러 시도를 함

### Asset을 담는 MeshModel(rastertek) 을 하나의 Cell 로 구름지대(terrain)

아래는 코드들

```cpp
class MeshModel {
public:
    MeshModel();
	MeshModel(const MeshModel&) = delete;
	~MeshModel();

    void Render(ID3D11DeviceContext* context, Shader* shader, Frustum* frustum, DirectX::XMMATRIX worldMatrix);

public:
    void AddMesh(std::unique_ptr<Mesh>);
    void AddMaterial(const Material&);

    bool InitConstantBuffer(ID3D11Device*);
    void GetPhysicsData(std::vector<ModelVertex>& outVertices, std::vector<unsigned int>& outIndices) const;
    float GetBottomOffset();
    float GetMinWidth() const;
    float GetMaxWidth() const;
    float GetMinHeight() const;
    float GetMaxHeight() const;
    float GetMinDepth() const;
    float GetMaxDepth() const;

private:
    void GetBoundingBox(DirectX::XMFLOAT3&, DirectX::XMFLOAT3&) const;

public:
    std::vector<MeshData> GetFullMeshData() const;

private:
    std::vector<std::unique_ptr<Mesh>> m_meshes;
    std::vector<Material> m_materials;
    std::unique_ptr<ConstantBuffer<MaterialBuffer>> m_materialBuffer;
}; // MeshModel
```


보통 일반적인 게임의 딱딱한 땅(고정된 지형)을 만들 때 사용하는 표준 방식

---

## 이게 안되는 이유

### 1) 생각 자체가 잘못 되어버림

`MeshModel` 은 한 번 로드되면`Vertex` 의 위치를 매 프레임 CPU에서 수정하기가 매우 무거운 것을 넘어 하나의 Cell에 각 Mesh을 하기엔 GPU에서 *"어디서 뭘 하라고"* 가 되어버림

하나의 큰 Mesh에 세부적인 Cell을 그릴지 말지인데 이런 방식에 내가 사용할 공간을 잡고 그 공간 Cell 에 Mesh를 로드하고 렌더하려 했는데 아예 그려지지 못함 

### 2) 메모리 및 성능

`1024x1024` 같은 넓은 범위를 세밀하게 표현하려면 수만 개의 폴리곤이 필요함 근데 이를 `MeshModel` 로 관리하면 성능 저하가 심각함

---

## 전환: 함수 기반(FBM) 셰이더 방식 채택

구글링 도중 [goto Shader - cloud-material](https://godotshaders.com/shader/cloud-material/) 관련 코드를 보고 왜 인지, 에셋 의존하지 않고 직접 그리는 것에 끌리게 됌

### 1) Noise

```cpp
float noise(float2 x)
{
  float2 p = floor(x);
  float2 f = fract(x);
  f = f * f * (3.0 - 2.0 * f);
  float2 a = float2(1.0, 0.0);
  return mix(mix(hash(p + a.yy), hash(p + a.xy), f.x),
         mix(hash(p + a.yx), hash(p + a.xx), f.x), f.y);
}
```

### 2) `fbm` (Deterministic)

`fbm(x, z, time)` 으로 GPU(셰이더)에서 직접 높이값 계산과 Draw를 하게 됨

GPU에서 Vertex 을 직접 들어 올리므로 CPU에 부담을 주지 않음

```cpp
float fbm(float2 x, float time) {
  float height = 0.0;
  float amplitude = 0.5;
  float frequency = 0.5;
  for (int i = 0; i < 6; i++){
    height += noise(x * frequency + time * time_factor * wave_speed) * amplitude;
    amplitude *= 0.5;
    frequency *= 2.0;
  }
  return height;
}
```

### 3) 무한한 확장성

텍스처 데이터 없이 수식만으로 지형을 생성하므로,

메모리 사용량이 극히 적고 노이즈 수치만 조절하면 언제든 지형의 느낌(둥글게 혹은 뾰족하게)을 바꿀 수 있음

---

## Res

![흐엉어엉](https://github.com/BOLTB0X/DirectX11-Draw/blob/main/DemoGIF/terrain-fbm/%ED%9D%90%EC%96%B4%EC%97%89%EC%96%B4%EC%97%89%EC%96%B4%EC%96%B4.png?raw=true)

----

## 에셋 출처

- [노이즈 텍스처 출처 - realtimevfx](https://realtimevfx.com/t/realistic-cloud-texture/19207)

- [Stone 이용: sketchfab - Ground//Stone Sphere(Madee)](https://sketchfab.com/3d-models/groundstone-sphere-1c0f2b2e213348e6a760743a546dc7a6)

- [normalMap 이용: filterforge `Cloud_normal.jpg`](https://www.filterforge.com/filters/6395-normal.html)

- [Mountain 이용: sketchfab - Cloudy Mountain(Pers Scans)](https://sketchfab.com/3d-models/cloudy-mountain-f61d84c6534d4007accdeac35f433119)