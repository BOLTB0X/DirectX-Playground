#include "Pch.h"
#include "Position.h"
// Common
#include "MathHelper.h"


using namespace DirectX;


Position::Position()
	:m_position(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_rotation(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_scale(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	m_worldMatrix(XMMatrixIdentity()),
	m_isDirty(true),
	m_frameTime(0),
	m_forwardSpeed(0),
	m_backwardSpeed(0),
	m_upwardSpeed(0),
	m_downwardSpeed(0),
	m_leftTurnSpeed(0),
	m_rightTurnSpeed(0),
	m_lookUpSpeed(0),
	m_lookDownSpeed(0)
{
} // Position


Position::~Position() {} // ~Position


void Position::SetFrameTime(float time)
{
	m_frameTime = time;
} // SetFrameTime


void Position::SetPosition(XMFLOAT3 pos)
{
	m_position = pos;
	m_isDirty = true;
} // SetPosition


void Position::SetPosition(float x, float y, float z)
{
	m_position = XMFLOAT3(x, y, z);
	m_isDirty = true;
} // SetPosition


void Position::SetRotation(XMFLOAT3 rot)
{
	m_rotation = rot;
	m_isDirty = true;
} // SetPosition


void Position::SetRotation(float x, float y, float z)
{
	m_rotation = XMFLOAT3(x, y, z);
	m_isDirty = true;
} // SetRotation


void Position::SetScale(float s)
{
	m_scale = XMFLOAT3(s, s, s);
	m_isDirty = true;
} // SetScale


void Position::SetScale(float x, float y, float z)
{
	m_scale = XMFLOAT3(x, y, z);
	m_isDirty = true;
} // SetScale


void Position::SetScale(XMFLOAT3 s)
{
	m_scale = s;
	m_isDirty = true;
} // SetScale


XMMATRIX Position::GetWorldMatrix()
{
	if (m_isDirty)
		UpdateWorldMatrix();
	
	return m_worldMatrix;
} // GetWorldMatrix



void Position::MoveForward(bool keydown)
{
	if (keydown)
	{
		m_forwardSpeed += m_frameTime;
		if (m_forwardSpeed > (m_frameTime * MathHelper::DEFAULT_MOVE_SPEED))
			m_forwardSpeed = m_frameTime * MathHelper::DEFAULT_MOVE_SPEED;
	}
	else
	{
		m_forwardSpeed -= m_frameTime * MathHelper::MOVE_BRAKE;
		if (m_forwardSpeed < 0.0f) m_forwardSpeed = 0.0f;
	}

	float radians = MathHelper::ToRadians(m_rotation.y);
	m_position.x += sinf(radians) * m_forwardSpeed * 0.1f;
	m_position.z += cosf(radians) * m_forwardSpeed * 0.1f;
	m_isDirty = true;
} // MoveForward

void Position::MoveBackward(bool keydown)
{
	if (keydown)
	{
		m_backwardSpeed += m_frameTime;
		if (m_backwardSpeed > (m_frameTime * MathHelper::DEFAULT_MOVE_SPEED))
			m_backwardSpeed = m_frameTime * MathHelper::DEFAULT_MOVE_SPEED;
	}
	else
	{
		m_backwardSpeed -= m_frameTime * MathHelper::MOVE_BRAKE;
		if (m_backwardSpeed < 0.0f) m_backwardSpeed = 0.0f;
	}

	float radians = MathHelper::ToRadians(m_rotation.y);
	m_position.x -= sinf(radians) * m_backwardSpeed * 0.1f;
	m_position.z -= cosf(radians) * m_backwardSpeed * 0.1f;
	m_isDirty = true;
} // MoveBackward

void Position::MoveUpward(bool keydown)
{
	if (keydown)
	{
		m_upwardSpeed += m_frameTime;
		if (m_upwardSpeed > (m_frameTime * MathHelper::VERTICAL_MOVE_SPEED))
			m_upwardSpeed = m_frameTime * MathHelper::VERTICAL_MOVE_SPEED;
	}
	else
	{
		m_upwardSpeed -= m_frameTime * MathHelper::MOVE_BRAKE;
		if (m_upwardSpeed < 0.0f) m_upwardSpeed = 0.0f;
	}

	m_position.y += m_upwardSpeed;
	m_isDirty = true;
} // MoveUpward


void Position::MoveDownward(bool keydown)
{
	if (keydown)
	{
		m_downwardSpeed += m_frameTime;
		if (m_downwardSpeed > (m_frameTime * MathHelper::VERTICAL_MOVE_SPEED))
			m_downwardSpeed = m_frameTime * MathHelper::VERTICAL_MOVE_SPEED;
	}
	else
	{
		m_downwardSpeed -= m_frameTime * MathHelper::MOVE_BRAKE;
		if (m_downwardSpeed < 0.0f) m_downwardSpeed = 0.0f;
	}

	m_position.y -= m_downwardSpeed;
	m_isDirty = true;
} // MoveDownward


void Position::TurnLeft(bool keydown)
{
	if (keydown)
	{
		m_leftTurnSpeed += m_frameTime;
		if (m_leftTurnSpeed > (m_frameTime * MathHelper::DEFAULT_TURN_SPEED))
			m_leftTurnSpeed = m_frameTime * MathHelper::DEFAULT_TURN_SPEED;
	}
	else
	{
		m_leftTurnSpeed -= m_frameTime * MathHelper::TURN_BRAKE;
		if (m_leftTurnSpeed < 0.0f) m_leftTurnSpeed = 0.0f;
	}

	m_rotation.y -= m_leftTurnSpeed;
	if (m_rotation.y < 0.0f) m_rotation.y += 360.0f;

	m_isDirty = true;
} // TurnLeft


void Position::TurnRight(bool keydown)
{
	if (keydown)
	{
		m_rightTurnSpeed += m_frameTime * MathHelper::TURN_ACCEL;
		if (m_rightTurnSpeed > (m_frameTime * MathHelper::DEFAULT_TURN_SPEED))
			m_rightTurnSpeed = m_frameTime * MathHelper::DEFAULT_TURN_SPEED;
	}
	else
	{
		m_rightTurnSpeed -= m_frameTime * MathHelper::TURN_BRAKE;
		if (m_rightTurnSpeed < 0.0f) m_rightTurnSpeed = 0.0f;
	}

	m_rotation.y += m_rightTurnSpeed;
	if (m_rotation.y > 360.0f) m_rotation.y -= 360.0f;

	m_isDirty = true;
} // TurnRight


void Position::LookUpward(bool keydown)
{
	if (keydown)
	{
		m_lookUpSpeed += m_frameTime * MathHelper::LOOK_ACCEL;
		if (m_lookUpSpeed > (m_frameTime * MathHelper::DEFAULT_LOOK_SPEED))
			m_lookUpSpeed = m_frameTime * MathHelper::DEFAULT_LOOK_SPEED;
	}
	else
	{
		m_lookUpSpeed -= m_frameTime * MathHelper::LOOK_BRAKE;
		if (m_lookUpSpeed < 0.0f) m_lookUpSpeed = 0.0f;
	}

	m_rotation.x -= m_lookUpSpeed;
	if (m_rotation.x < -90.0f) m_rotation.x = -90.0f;

	m_isDirty = true;
} // LookUpward


void Position::LookDownward(bool keydown)
{
	if (keydown)
	{
		m_lookDownSpeed += m_frameTime * MathHelper::LOOK_ACCEL;
		if (m_lookDownSpeed > (m_frameTime * MathHelper::DEFAULT_LOOK_SPEED))
			m_lookDownSpeed = m_frameTime * MathHelper::DEFAULT_LOOK_SPEED;
	}
	else
	{
		m_lookDownSpeed -= m_frameTime * MathHelper::LOOK_BRAKE;
		if (m_lookDownSpeed < 0.0f) m_lookDownSpeed = 0.0f;
	}

	m_rotation.x += m_lookDownSpeed;
	if (m_rotation.x > 90.0f) m_rotation.x = 90.0f;

	m_isDirty = true;
} // LookDownward


void Position::UpdateWorldMatrix()
{
	XMMATRIX s = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);

	XMMATRIX r = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(m_rotation.x),
		XMConvertToRadians(m_rotation.y),
		XMConvertToRadians(m_rotation.z)
	);

	XMMATRIX t = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);

	m_worldMatrix = s * r * t;
	m_isDirty = false;
} // UpdateWorldMatrix