#pragma once
#include <DirectXMath.h>
#include "Model/Mesh/VertexTypes.h"


namespace MathHelper { // 물리 및 이동 관련 상수

    // 수학 상수
    const float PI = 3.1415926535f;
    const float DEG_TO_RAD = PI / 180.0f;
    const float RAD_TO_DEG = 180.0f / PI;

    // 물리/이동 기본 설정
    const float DEFAULT_MOVE_SPEED = 50.0f;
    const float DEFAULT_TURN_SPEED = 150.0f;
    const float DEFAULT_LOOK_SPEED = 75.0f;
    const float FRICTION_RATIO = 0.5f;

    const float GRAVITY = -9.8f * 3.0f;

    const float MOVE_ACCEL = 1.0f; // 이동 가속도
    const float MOVE_BRAKE = 0.5f; // 이동 감속도 (마찰력)
    const float VERTICAL_MOVE_SPEED = 15.0f;

    const float TURN_ACCEL = 5.0f; // 회전 가속도
    const float TURN_BRAKE = 3.5f; // 회전 감속도
    const float LOOK_ACCEL = 7.5f; // 상하 시선 가속도
    const float LOOK_BRAKE = 2.0f; // 상하 시선 감속도
} // // 물리 및 이동 관련 상수


namespace MathHelper { // 단위 변환 및 기본 수학 유틸리티 함수

    // 변환 함수
    inline float ToRadians(float degrees) { return degrees * DEG_TO_RAD; }
    inline float ToDegrees(float radians) { return radians * RAD_TO_DEG; }

    inline float Frac(float x) { return x - floorf(x); } // Frac
    inline float Lerp(float a, float b, float t) { return a + t * (b - a); } // Lerp

    template <typename T>
    T clamp(T value, T min_val, T max_val)
    {
        if (value < min_val)
            return min_val;

        else if (value > max_val)
            return max_val;

        else
            return value;
    } // clamp

} // 단위 변환 및 기본 수학 유틸리티 함수


namespace MathHelper { // 셰이더 관련 함수

    // frac(sin(dot(p, float2(12.9898, 78.233))) * 43758.5453)
    inline float Hash(DirectX::XMFLOAT2 p)
    {
        float dotProduct = p.x * 12.9898f + p.y * 78.233f;
        return Frac(sinf(dotProduct) * 43758.5453f);
    } // Hash


    // 2D Value Noise (HLSL의 noise 함수 이식)
    inline float Noise(DirectX::XMFLOAT2 p)
    {
        DirectX::XMFLOAT2 i = { floorf(p.x), floorf(p.y) };
        DirectX::XMFLOAT2 f = { p.x - i.x, p.y - i.y };

        float ux = f.x * f.x * (3.0f - 2.0f * f.x);
        float uy = f.y * f.y * (3.0f - 2.0f * f.y);

        float a = Hash({ i.x, i.y });
        float b = Hash({ i.x + 1.0f, i.y });
        float c = Hash({ i.x, i.y + 1.0f });
        float d = Hash({ i.x + 1.0f, i.y + 1.0f });

        return Lerp(Lerp(a, b, ux), Lerp(c, d, ux), uy);
    } // Noise


    // CPU용 FBM 높이 계산 함수
    inline float GetFBMHeight(float x, float z, float time)
    {
        float height = 0.0f;
        float amplitude = 0.5f;
        float frequency = 0.05f;

        for (int i = 0; i < 6; i++)
        {
            height += Noise({ x * frequency + time * 0.2f,  z * frequency + time * 0.2f }) * amplitude;
            amplitude *= 0.45f;
            frequency *= 2.0f;
        }

        return height * 15.0f;
    } // MathHelper

} // 셰이더 관련 함수


namespace MathHelper { // Terrain

    inline DirectX::XMFLOAT3 GetTerrainNormal(float x, float z, float time)
    {
        float d = 0.1f; // 샘플링 간격
        float h = GetFBMHeight(x, z, time);
        float hX = GetFBMHeight(x + d, z, time);
        float hZ = GetFBMHeight(x, z + d, time);

        // 두 개의 접선 벡터를 외적하여 법선 계산
        DirectX::XMVECTOR v1 = DirectX::XMVectorSet(d, hX - h, 0.0f, 0.0f);
        DirectX::XMVECTOR v2 = DirectX::XMVectorSet(0.0f, hZ - h, d, 0.0f);
        DirectX::XMVECTOR n = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(v2, v1));

        DirectX::XMFLOAT3 normal;
        DirectX::XMStoreFloat3(&normal, n);
        return normal;
    } // GetTerrainNormal


    inline void CalculateTangentBinormal(
        const ModelVertex& v1, const ModelVertex& v2, const ModelVertex& v3,
        DirectX::XMFLOAT3& tangent, DirectX::XMFLOAT3& binormal)
    {
        float vector1[3], vector2[3];
        float tuVector[2], tvVector[2];

        // 두 변의 벡터 계산
        vector1[0] = v2.position.x - v1.position.x;
        vector1[1] = v2.position.y - v1.position.y;
        vector1[2] = v2.position.z - v1.position.z;

        vector2[0] = v3.position.x - v1.position.x;
        vector2[1] = v3.position.y - v1.position.y;
        vector2[2] = v3.position.z - v1.position.z;

        // UV 좌표 차이 계산
        tuVector[0] = v2.texture.x - v1.texture.x;
        tvVector[0] = v2.texture.y - v1.texture.y;

        tuVector[1] = v3.texture.x - v1.texture.x;
        tvVector[1] = v3.texture.y - v1.texture.y;

        // 접선/종법선 공식 적용
        float den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

        tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
        tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
        tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

        binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
        binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
        binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

        // 정규화
        DirectX::XMVECTOR t = DirectX::XMLoadFloat3(&tangent);
        DirectX::XMVECTOR b = DirectX::XMLoadFloat3(&binormal);
        DirectX::XMStoreFloat3(&tangent, DirectX::XMVector3Normalize(t));
        DirectX::XMStoreFloat3(&binormal, DirectX::XMVector3Normalize(b));

        return;
    } // CalculateTangentBinormal

} // Terrain


namespace MathHelper { // 벡터 산술 연산

    inline DirectX::XMFLOAT3 Add(const DirectX::XMFLOAT3& a, const DirectX::XMFLOAT3& b)
    {
        return { a.x + b.x, a.y + b.y, a.z + b.z };
    } // Add


    inline DirectX::XMFLOAT3 Multiply(const DirectX::XMFLOAT3& v, float s)
    {
        return { v.x * s, v.y * s, v.z * s };
    } // Multiply


    inline float Distance(float x1, float z1, float x2, float z2)
    {
        return sqrtf(powf(x2 - x1, 2) + powf(z2 - z1, 2));
    } // Distance


    inline void GetCameraVectors(const DirectX::XMMATRIX& viewMatrix, DirectX::XMFLOAT3& right, DirectX::XMFLOAT3& up) {
        DirectX::XMMATRIX invView = DirectX::XMMatrixInverse(nullptr, viewMatrix);

        DirectX::XMFLOAT4X4 m;
        DirectX::XMStoreFloat4x4(&m, invView);

        right = { m._11, m._12, m._13 }; // 1행은 Right 벡터
        up = { m._21, m._22, m._23 }; // 2행은 Up 벡터
    } // GetCameraVectors

} // 벡터 산술 연산