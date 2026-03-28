// NoiseCS.hlsl
#include "FBM.hlsl"
#include "Remap.hlsl"

cbuffer NoiseBuffer : register(b0)
{
    // row1
    float3 textureSize;
    float  perlinFreq;  // 뭉게구름 기본 주파수
    // row2
    float  worleyFreq;  // 뭉게구름 디테일 주파수
    float  detailFreqG; // 침식용 노이즈 G 주파수
    float  detailFreqB; // 침식용 노이즈 B 주파수
    float  detailFreqA; // 침식용 노이즈 A 주파수
    // row3
    int    octaves; // FBM 반복 횟수
    float  remapBias;
    float2 padding;
}; // NoiseBuffer

RWTexture3D<float4> outVolume : register(u0);

[numthreads(8, 8, 8)]
void main(uint3 dispatchThreadId : SV_DispatchThreadID)
{
    if (any(dispatchThreadId >= (uint3) textureSize))
        return;

    // 0.0 ~ 1.0 범위의 3D 정규화 좌표
    float3 uvw = float3(dispatchThreadId) / textureSize;

    // ---------------------------------------------------------
    // R 채널: Base Cloud Shape (Perlin-Worley)
    // ---------------------------------------------------------
    // Perlin 노이즈(큰 덩어리)를 Worley 노이즈(몽글몽글한 질감)로 깎아냅니다.
    // 셰이더토이의 fbm_clouds 역할을 대체하는 AAA 게임의 표준 베이스 노이즈입니다.
    float perlin = perlinFbm(uvw, perlinFreq, octaves);
    float worley = WorleyPeriodic(uvw, worleyFreq);
    
    // Perlin의 하위 영역을 Worley로 Remap하여 볼륨감을 만듭니다.
    float baseCloud = RemapClamp(perlin, worley, 1.0, 0.0, 1.0) + remapBias;

    // ---------------------------------------------------------
    // G, B, A 채널: Detail Cloud Shape (Erosion)
    // ---------------------------------------------------------
    // 레이마칭 픽셀 셰이더에서 구름의 외곽선을 거칠게 파먹을(침식) 때 사용할 데이터입니다.
    // 각 채널마다 옥타브를 섞어(0.625 + 0.25 등) 깊이감을 줍니다.
    float detailG = WorleyPeriodic(uvw, detailFreqG) * 0.625 + WorleyPeriodic(uvw, detailFreqG * 2.0) * 0.25;
    float detailB = WorleyPeriodic(uvw, detailFreqB) * 0.625 + WorleyPeriodic(uvw, detailFreqB * 2.0) * 0.25;
    float detailA = WorleyPeriodic(uvw, detailFreqA) * 0.625 + WorleyPeriodic(uvw, detailFreqA * 2.0) * 0.25;

    // 최종 텍스처에 굽기 (0~1 범위로 안전하게 Saturate)
    outVolume[dispatchThreadId] = saturate(float4(baseCloud, detailG, detailB, detailA));
}