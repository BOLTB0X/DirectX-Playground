#ifndef NOISEGENERATOR_PARAMS_HLSLI
#define NOISEGENERATOR_PARAMS_HLSLI

cbuffer NoiseBuffer : register(b0)
{
    float3 nTextureSize; // 텍스처 해상도
    float  nPerlinFreq; // 기본 덩어리 Perlin 주파수

    float  nWorleyFreq; // 기본 덩어리 Worley 주파수
    float  nDetailFreqG; // G 채널 디테일 주파수
    float  nDetailFreqB; // B 채널 디테일 주파수
    float  nDetailFreqA; // A 채널 디테일 주파수

    int    nOctaves; // FBM 반복 횟수
    float  nRemapBias; // 구름 밀도 조절용 Bias
    float2 nPadding; // 16바이트 정렬을 위한 패딩
}; // NoiseBuffer

#endif