// Base/Position.h
#pragma once

#include <directxmath.h>
#include "Common/MathHelper.h"

class Position
{
public:
    Position();
    Position(const Position&) = delete;
    ~Position();

public:
    void SetFrameTime(float);
    void SetPosition(float, float, float);
    void SetRotation(float, float, float);
    void SetScale(float);

    DirectX::XMFLOAT3 GetPosition() const { return m_position; }
    DirectX::XMFLOAT3 GetRotation() const { return m_rotation; }
    DirectX::XMFLOAT3 GetScale() const { return m_scale; }
    DirectX::XMMATRIX GetWorldMatrix();

public:
    void MoveForward(bool);
    void MoveBackward(bool);
    void MoveUpward(bool);
    void MoveDownward(bool);
    void TurnLeft(bool);
    void TurnRight(bool);
    void LookUpward(bool);
    void LookDownward(bool);

private:
    void UpdateWorldMatrix();

private:
    DirectX::XMFLOAT3 m_position;
    DirectX::XMFLOAT3 m_rotation;
    DirectX::XMFLOAT3 m_scale;
    DirectX::XMMATRIX m_worldMatrix;

    bool m_isDirty;

    float m_frameTime;

    float m_forwardSpeed, m_backwardSpeed;
    float m_upwardSpeed, m_downwardSpeed;
    float m_leftTurnSpeed, m_rightTurnSpeed;
    float m_lookUpSpeed, m_lookDownSpeed;
}; // Position