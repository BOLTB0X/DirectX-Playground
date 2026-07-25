// Framework/Physics/Phusics.h
#pragma once
#include <DirectXMath.h>

struct Ray {
    DirectX::XMFLOAT3 origin;
    DirectX::XMFLOAT3 direction;
}; // Ray

struct PhysicsState {
    DirectX::XMFLOAT3 velocity = { 0, 0, 0 };
    float acceleration = 10.0f;
    float friction = 0.98f; // 마찰력
    bool isOnCliff = false; // 접촉 여부
}; // PhysicsState


class Physics {
public:
    Physics();
    Physics(const Physics& other) = delete;
    ~Physics();

    void Frame(float frameTime);

public:
    // 힘 가하기 (가속도 전달)
    void AddForce(const DirectX::XMFLOAT3& force);

    DirectX::XMFLOAT3 GetVelocity() const {return m_velocity; }

    void SetVelocity(const DirectX::XMFLOAT3& vel) { m_velocity = vel; }
    void SetGravityScale(float scale) { m_gravityScale = scale; }
    void SetFriction(float friction) { m_friction = friction; }

private:
    DirectX::XMFLOAT3 m_velocity;
    DirectX::XMFLOAT3 m_acceleration;
    float m_friction;
    float m_gravityScale;
}; // Physics