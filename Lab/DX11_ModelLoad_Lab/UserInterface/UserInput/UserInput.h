// UserInterface/UserInput/UserInput.h
#pragma once

#include<memory>
#include <Input/Input.h>

class Position;
class ActorObject;
class Camera;


class UserInput {
public:
    UserInput();
    bool Init(std::shared_ptr<Input> input);
    void Frame();

public:
    // 키보드 상태 직접 연결용
    bool IsEscapePressed() const { return m_Input->IsEscapePressed(); }
    bool IsF1Toggled() const { return m_Input->IsF1Toggled(); }

    bool IsWPressed() const { return m_Input->IsWPressed(); }
    bool IsSPressed() const { return m_Input->IsSPressed(); }
    bool IsAPressed() const { return m_Input->IsAPressed(); }
    bool IsDPressed() const { return m_Input->IsDPressed(); }

public:
    bool IsMouseLPressed();
    void GetMouseDelta(float&, float&);
    float GetZoom() const;

    void ProcessInput(float, Position*);
    void ProcessActorInput(float, ActorObject*, Camera*);

private:
    std::shared_ptr<Input> m_Input; 
    float m_deltaX, m_deltaY;
}; // UserInput