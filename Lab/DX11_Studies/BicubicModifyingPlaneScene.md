# 포스트 프로세싱이 잘못된걸 고치는 이야기


**핵심이자 나의 사소한 실수는**

- **Ray Marching** 결과는 그냥 하나의 “컬러 텍스처” 여야 했는데

- **Bicubic** 은 그 결과를 다시 계산하면 안 되고 **샘플링만 해야 했는데**

- **리소스 충돌**

## 왜 planeScene 수정 후 Bicubic이 깨졌을까?

구름지대로 변경해서 구름을 화면 전체가 아닌 일부분만 **Quad** 로 가렸다

어차피 Bicubic 업스케일은 **디지털 이미지 확대 시 주변 16개(4x4) 픽셀의 3차 함수 기반 값을 가중 평균하여 부드럽고 선명한 결과를 얻는 고급 픽셀 재샘플링 방식** 이니

화면 전체를 수정하고 **Quad** 를 화면 전체로 늘렸는데 적용이 안되는 것을 Test로 회색필터를 적용이 안되는 것을 보고 문제를 알아차림

### W)1. planeScene 변경이 끼치는 영향

```cpp
float distance = abs(p.y - height) - thickness;
float f = fbm(p, iTime);
return -distance + f;
```

이 코드자체는 문제 없지만, 문제는 이 값이 어디까지 영향을 미치느냐임

현재 구조에서 **Ray Marching** 결과를 *스크린 공간* 이 아니라 *월드 / 뷰 좌표* 기준으로 다시 계산

**Post Process Shader** 에서도 다시 **Ray Marching/SDF** 를 계산해 버리니 **Bicubic** 이 **“보간할 대상”을 잃어버림**


### W)2. 잘못된 렌더타겟

```cpp
class RenderTarget {
public:
    // ...

    bool Init(ID3D11Device*, IDXGISwapChain*, int, int);
    bool Init(ID3D11Device*, int, int);

    // ...
}; // RenderTarget
```

현재 진행중인 내 프로젝트는 `RenderTarget` 객체로 분리해서 관리하였고, 2개의 `Init` 메서드를 오버라이딩 해서 사용했는데

```cpp
bool RenderTarget::InitDethStencil(ID3D11Device* device, IDXGISwapChain* swapChain, int width, int height)
{
    ComPtr<ID3D11Texture2D> backBufferPtr;
    if (SuccessCheck(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
        &backBufferPtr),
        "스왑 체인에서 백 버퍼 획득 실패")
        == false) return false;

    if (SuccessCheck(device->CreateRenderTargetView(backBufferPtr.Get(),
        nullptr, &m_renderTargetView),
        "RenderTargetView 생성 실패") == false) return false;

    // DepthStencilBuffer (2D 텍스처) 설정 및 생성
    D3D11_TEXTURE2D_DESC depthBufferDesc = {};
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

    depthBufferDesc.Width = width;
    depthBufferDesc.Height = height;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    // 깊이 버퍼용 텍스처를 생성
    if (SuccessCheck(
        device->CreateTexture2D(&depthBufferDesc, nullptr, &m_depthStencilBuffer),
        "깊이 버퍼 텍스처 생성 실패")
        == false) return false;

    // DepthStencilView 생성
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    if (SuccessCheck(device->CreateDepthStencilView(m_depthStencilBuffer.Get(), &depthStencilViewDesc, &m_depthStencilView),
        "DepthStencilView 생성 실패") == false) return false;
    return true;
} // InitDethStencil
```

```cpp
bool RenderTarget::InitLowResolution(ID3D11Device* device, int width, int height)
{
    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    ComPtr<ID3D11Texture2D> renderTexture;
    if (SuccessCheck(device->CreateTexture2D(&texDesc, nullptr, &renderTexture),
        "InitLowResolution: CreateTexture2D 실패") == false)
        return false;
    if (SuccessCheck(device->CreateRenderTargetView(renderTexture.Get(), nullptr, &m_renderTargetView),
        "InitLowResolution: CreateRenderTargetView") == false)
        return false;
    if (SuccessCheck(device->CreateShaderResourceView(renderTexture.Get(), nullptr, &m_shaderResourceView),
        "InitLowResolution: CreateShaderResourceView") == false)
        return false;

    D3D11_TEXTURE2D_DESC depthDesc = texDesc;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    if (FAILED(device->CreateTexture2D(&depthDesc, nullptr, &m_depthStencilBuffer)))
        return false;
    if (FAILED(device->CreateDepthStencilView(m_depthStencilBuffer.Get(), nullptr, &m_depthStencilView)))
        return false;

    return true;
} // InitLowResolution
```

`InitLowResolution`을 사용하지 않고 계속 `InitDethStencil` 을 사용했었음

### W)3. 리소스 충돌

```cpp
void Renderer::SetLowResolutionRenderTarget()
{
    m_LowResRenderTarget->Clear(context, 0, 0, 0, 1);
} // SetLowResolutionRenderTarget
```

저해상도 타겟에 그리기 직전에, 이전 프레임에서 사용했던 **SRV** 를 반드시 해제해야 했는데 안하고 있었음

---

## Sol

### Sol)1. 리소스 충돌 방지

**각 렌더타켓을 물리적으로 다른 공간으로 분리**

```cpp
if (m_MainRenderTarget->Init(
    m_DX11Device->GetDevice(),
    m_DX11Device->GetSwapChain(),
    SCREEN_WIDTH, SCREEN_HEIGHT)
    == false)
    return false;

if (m_LowResRenderTarget->Init(
    m_DX11Device->GetDevice(),
    SCREEN_WIDTH, SCREEN_HEIGHT)
    == false)
    return false;
```

- `m_MainRenderTarget`은 스왑체인의 메모리를 쓰고,

- `m_LowResRenderTarget`은 GPU 메모리에 새로 만든 텍스처를 사용

두 개의 렌더 타겟이 같은 **'백버퍼'** 를 가리켰던 것을 수정

### Sol)2. SRV 해제 추가

DX11은 파이프라인 슬롯에 한번 꽂힌 SRV를 명시적으로 빼주지 않으면 계속 잡고 있음

1. **Frame N** : `ApplyBicubicUpscale`에서 저해상도 텍스처를 `PSSetShaderResources(0, ...)` 에 꽂음

2. **Frame N+1** : `SetLowResolutionRenderTarget`에서 그 텍스처를 다시 그리려 함(RTV 바인딩)

3. **충돌** : *"이미 0번 슬롯에서 읽기용(SRV)으로 쓰고 있는데 어떻게 여기다 그리냐?* "며 바인딩 실패

```cpp
void RenderingEngine::ApplyBicubicUpscale(ID3D11DeviceContext* context)
{
    m_ShaderManager->SetShaders(ShaderKeys::Bicubic, context);
    m_Renderer->SetLowResolutionShaderResources(0);

    m_Renderer->SetSampler(0);

    m_ShaderManager->UpdateMatrixBuffer(
        ShaderKeys::Bicubic, context,
        XMMatrixIdentity(), XMMatrixIdentity(), XMMatrixIdentity());

    m_Quad->Render(context);
} // ApplyBicubicUpscale
```

```cpp
void Renderer::SetLowResolutionRenderTarget()
{
    ID3D11DeviceContext* context = m_DX11Device->GetDeviceContext();
    ClearShaderResources(0);

    m_LowResRenderTarget->Clear(context, 0, 0, 0, 1);
} // SetLowResolutionRenderTarget
```

```cpp
void Renderer::ClearShaderResources(UINT slot)
{
    ID3D11ShaderResourceView* nullSRV = nullptr;
    m_DX11Device->GetDeviceContext()->PSSetShaderResources(slot, 1, &nullSRV);
} // ClearShaderResources
```

`SetLowResolutionRenderTarget` 메서드 내부에 `ClearShaderResources(0)` 추가 

```cpp
void RenderingEngine::Draw(
    float totalTime,
    Property<XMMATRIX> viewProp,
    Property<XMMATRIX> projProp,
    Property<XMFLOAT3> camPosProp)
{
    ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();
    XMFLOAT3 camPos = camPosProp.Get();
    XMMATRIX view = viewProp.Get();
    XMMATRIX proj = projProp.Get();

    m_Renderer->SetLowResolutionRenderTarget();
    m_Renderer->SetAlphaBlending(true);
    m_Renderer->SetDepthBuffer(false);

    // 스카이
    DrawSky(context, totalTime, camPos, view, proj);

    // Back-face Culling
    m_Renderer->SetMode(false, true);

    // 구름
    DrawCloud(context, totalTime, camPos, view, proj);

	// 포스트 프로세싱
    m_Renderer->SetBackBufferRenderTarget();
    m_Renderer->SetAlphaBlending(false);

    ApplyLensFlare(context, view, proj, camPos);
    ApplyBicubicUpscale(context);

    m_Renderer->ClearShaderResources(0); // 여기도
    m_Renderer->SetDepthBuffer(true);
  
    m_frameCount++;
} // Draw
```

`RenderingEngine::Draw` 에도 깊이버퍼로 전환전 에도 추가


### Sol)3. Vertex Shader  NDC 공간에 맞게 수정

```cpp
PixelInput main(QuadInput input)
{
    PixelInput output;
    
    output.position = float4(input.position.xy, 0.0f, 1.0f);
    output.tex = input.tex;
    
    return output;
} // main
```

기존 월드공간 연산 삭제


### cf. 이런 수정 과정이 잘 적용되었는지 확인을 위해 회색필터 적용

<p align="center">
  <table style="width:100%; text-align:center; border-spacing:20px;">
    <tr>
        <td style="text-align:center; vertical-align:middle;">
        <p align="center">
        <img src="https://github.com/BOLTB0X/DirectX11-Draw/blob/main/DemoGIF/volumetric-raymarching/t_%ED%9B%84%EC%B2%98%EB%A6%AC_%ED%9A%8C%EC%83%89%ED%95%84%ED%84%B0_%EC%A0%81%EC%9A%A9.png?raw=true" 
             alt="image 2" 
             style="; object-fit:contain; border:1px solid #ddd; border-radius:4px;"/>
        </p>
      </td>
    </tr>
    <tr>
      <td style="text-align:center; font-size:14px; font-weight:bold;">
      <p align="center">
      <a></a>Gray 필터</a>
      </p>
      </td>
    </tr>
  </table>
</p>

---

## 요약

1. 후처리 렌더타겟을 백버퍼로 설정한 실수

2. 구름지대로 변경, Quad의 범위를 전체 스크린으로 했는데 월드공간에 Quad로 연산했음

    - 기존 단일 구름일 때 범위가 적어 특정 위치에 단일 구름을 Quad가 가릴수 있었으나 구름 지대로 변경하고 코드 수정 과정에서 누적된 코드들이 꼬여버림

3. SRV 초기화 실수
