// UserInterface/UserInput/UserInput.cpp
#include "UserInput.h"
// Framework
#include "Position/Position.h"
#include "Actor/ActorObject.h"
#include "Physics/Physics.h"
// Graphics
#include "Camera/Camera.h"
// Common
#include "MathHelper.h"

/* default */
///////////////////////////////////////////////////////

UserInput::UserInput()
	: m_deltaX(0.0f),
	m_deltaY(0.0f)
{
} // UserInput


bool UserInput::Init(std::shared_ptr<Input> input)
{
    if (input == nullptr) return false;
    m_Input = input; // 참조
	return true;
} // Init


void UserInput::Frame()
{
    if (m_Input == nullptr) return;

    int rawX, rawY;
    m_Input->GetMouseDelta(rawX, rawY);

    const float sensitivity = 0.1f;
    m_deltaX = static_cast<float>(rawX) * sensitivity;
    m_deltaY = static_cast<float>(rawY) * sensitivity;
    return;
} // Frame


//////////////////////////////////////////////////////

/* publoc */
//////////////////////////////////////////////////////

bool UserInput::IsMouseLPressed()
{ 
    return m_Input->IsMouseLPressed();
} // IsMouseLPressed


void UserInput::GetMouseDelta(float& dx, float& dy)
{ 
    dx = m_deltaX;
    dy = m_deltaY;
} // GetMouseDelta


float UserInput::GetZoom() const
{
    return static_cast<float>(m_Input->GetMouseWheelDelta());
} // GetZoom


void UserInput::ProcessInput(float deltaTime, Position* camPos)
{
    if (m_Input == nullptr || camPos == nullptr) return;

    camPos->SetFrameTime(deltaTime);

    // 마우스 회전 처리
    DirectX::XMFLOAT3 rot = camPos->GetRotation();
    camPos->SetRotation(rot.x + m_deltaY, rot.y + m_deltaX, rot.z);

    // 키보드 이동 처리
    camPos->MoveForward(m_Input->IsWPressed());
    camPos->MoveBackward(m_Input->IsSPressed());

    camPos->MoveUpward(m_Input->IsZPressed());
    camPos->MoveDownward(m_Input->IsXPressed());

    // 좌우 이동
    camPos->TurnLeft(m_Input->IsAPressed());
    camPos->TurnRight(m_Input->IsDPressed());

    // 줌
    float wheelDelta = GetZoom();
    if (wheelDelta != 0.0f)
    {
        const float zoomSpeed = 0.1f;
        float moveAmount = wheelDelta * zoomSpeed;

        float radians = MathHelper::ToRadians(camPos->GetRotation().y);
        DirectX::XMFLOAT3 pos = camPos->GetPosition();

        pos.x += sinf(radians) * moveAmount;
        pos.z += cosf(radians) * moveAmount;

        camPos->SetPosition(pos.x, pos.y, pos.z);
    }
} // ProcessInput


void UserInput::ProcessActorInput(float deltaTime, ActorObject* actor, Camera* camera)
{
    if (m_Input == nullptr || actor == nullptr || camera == nullptr) return;

    float forceAmount = MathHelper::DEFAULT_MOVE_SPEED * 1.0f;
    float yaw = MathHelper::ToRadians(camera->GetPosition()->GetRotation().y);

    // 카메라가 바라보는 방향 벡터 (XZ 평면)
    float forwardX = sinf(yaw);
    float forwardZ = cosf(yaw);
    float rightX = cosf(yaw);
    float rightZ = -sinf(yaw);

    // 상/하: 카메라 정면/후면 방향
    if (m_Input->IsUpPressed())
        actor->AddForce({ forwardX * forceAmount, 0, forwardZ * forceAmount });
    if (m_Input->IsDownPressed())
        actor->AddForce({ -forwardX * forceAmount, 0, -forwardZ * forceAmount });

    // 좌/우: 카메라 측면 방향
    if (m_Input->IsLeftPressed())
        actor->AddForce({ -rightX * forceAmount, 0, -rightZ * forceAmount });
    if (m_Input->IsRightPressed())
        actor->AddForce({ rightX * forceAmount, 0, rightZ * forceAmount });

    return;
} // ProcessActorInput