// Framework/Physics/Phusics.cpp
#include "Physics.h"
// Common
#include "MathHelper.h"
/* default */
///////////////////////////////////////////////////////////

Physics::Physics()
    : m_velocity(0.0f, 0.0f, 0.0f),
    m_acceleration(0.0f, 0.0f, 0.0f),
    m_friction(0.98f),
    m_gravityScale(1.0f)
{}


Physics::~Physics()
{}



void Physics::Frame(float frameTime)
{
    m_velocity.x += m_acceleration.x * frameTime;
    m_velocity.y += m_acceleration.y * frameTime;
    m_velocity.z += m_acceleration.z * frameTime;

    m_acceleration = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);


    m_velocity.x *= m_friction;
    m_velocity.y *= m_friction;
    m_velocity.z *= m_friction;

    if (fabs(m_velocity.x) < 0.001f) m_velocity.x = 0.0f;
    if (fabs(m_velocity.y) < 0.001f) m_velocity.y = 0.0f;
    if (fabs(m_velocity.z) < 0.001f) m_velocity.z = 0.0f;

    m_velocity.y += MathHelper::GRAVITY * frameTime;
} // Frame


///////////////////////////////////////////////////////////


/* public */
///////////////////////////////////////////////////////////


void Physics::AddForce(const DirectX::XMFLOAT3& force)
{
    // F = ma
    m_acceleration.x += force.x;
    m_acceleration.y += force.y;
    m_acceleration.z += force.z;
} // AddForce