#pragma once
#include <DirectXMath.h>
#include "Model/VertexTypes.h"


namespace MathHelper { // 수학 상수

    // 수학 상수
    const float PI = 3.1415926535f;
    const float DEG_TO_RAD = PI / 180.0f;
    const float RAD_TO_DEG = 180.0f / PI;

    const float DEFAULT_MOVE_SPEED = 50.0f;
    const float DEFAULT_TURN_SPEED = 150.0f;
    const float DEFAULT_LOOK_SPEED = 75.0f;
    const float FRICTION_RATIO = 0.5f;

    const float MOVE_ACCEL = 0.5f; // 이동 가속도
    const float MOVE_BRAKE = 0.5f; // 이동 감속도 (마찰력)
    const float VERTICAL_MOVE_SPEED = 15.0f;

    const float TURN_ACCEL = 1.5f; // 회전 가속도
    const float TURN_BRAKE = 1.5f; // 회전 감속도
    const float LOOK_ACCEL = 1.5f; // 상하 시선 가속도
    const float LOOK_BRAKE = 1.5f; // 상하 시선 감속도

} // 물리 및 이동 관련 상수


namespace MathHelper { // 단위 변환 및 기본 수학 유틸리티 함수

    // 변환 함수
    inline float ToRadians(float degrees) { return degrees * DEG_TO_RAD; }
    inline float ToDegrees(float radians) { return radians * RAD_TO_DEG; }

    inline float Frac(float x) { return x - floorf(x); } // Frac
    inline float Lerp(float a, float b, float t) { return a + t * (b - a); } // Lerp


    template<typename T>
    inline T Min(T a, T b) { return (a < b) ? a : b; } // Min
    template<typename T>
    inline T Max(T a, T b) { return (a > b) ? a : b; } // Max


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


namespace MathHelper { // Graph

   
    inline void CalculateTangentBinormal(
        const MeshVertex& v1, const MeshVertex& v2, const MeshVertex& v3,
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


namespace MathHelper { // 벡터 연산

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


    inline float RotationWrap(float value)
    {
        value = fmod(value, 360.0);

        if (value < -180.0)
            value += 360.0;
        else if (value > 180.0)
            value -= 360.0;

        return value;
    } // RotationWrap


    inline DirectX::XMVECTOR RotationToVector(DirectX::XMFLOAT3 rot)
    {
        using namespace DirectX;

        // rotation값으로 회전 행렬을 만들고
        // 기본 forward 벡터와 곱해서 회전된 벡터를 계산
        float pitch = XMConvertToRadians(rot.x);
        float yaw = XMConvertToRadians(rot.y);
        float roll = XMConvertToRadians(rot.z);

        XMMATRIX rotationMat = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

        XMVECTOR direction = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f); // +Z forward
        direction = XMVector3Transform(direction, rotationMat);
        return direction;
    } // RotationToVector


    inline DirectX::XMFLOAT3 VectorToRotation(DirectX::XMFLOAT3 vec)
    {
        float pitch = -asinf(clamp<float>(vec.y, -1.0f, 1.0f));
        float yaw = atan2f(vec.x, vec.z);

        return DirectX::XMFLOAT3(DirectX::XMConvertToDegrees(pitch), DirectX::XMConvertToDegrees(yaw), 0.0f);
    } // VectorToRotation


    inline DirectX::XMMATRIX TransformUVRotationMatrix(float radians)
    {
        return DirectX::XMMatrixTranslation(-0.5f, -0.5f, 0.0f) * DirectX::XMMatrixRotationZ(radians) * DirectX::XMMatrixTranslation(0.5f, 0.5f, 0.0f);
    } // TransformUVRotationMatrix


    inline DirectX::XMMATRIX GetUVRotationMatrix(const DirectX::XMMATRIX& view)
    {
        using namespace DirectX;

        float m00 = XMVectorGetX(view.r[0]);
        float m01 = XMVectorGetY(view.r[0]);
        float camRot = atan2(m01, m00);

        return XMMatrixTranspose(TransformUVRotationMatrix(camRot));
    } // GetUVRotationMatrix


    inline DirectX::XMMATRIX GetReflectionMatrixFromPlane(DirectX::XMFLOAT3 camPos, float height)
    {
        using namespace DirectX;
        float reflectionY = -camPos.y + (height * 2.0f);
        XMVECTOR reflectionPos = XMVectorSet(camPos.x, reflectionY, camPos.z, 1.0f);
        XMMATRIX reflectionMatrix = XMMatrixReflect(XMVectorSet(0, 1, 0, -height));

        return reflectionMatrix;
    } // GetReflectionMatrixFromPlane
    
} // 벡터 연산


namespace MathHelper {

}