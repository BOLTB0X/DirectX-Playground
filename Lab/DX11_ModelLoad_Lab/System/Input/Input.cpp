// System/Input.cpp
#include "Input.h"
// Common
#include "EngineSettings.h"
#include "EngineHelper.h"

/* default */
/////////////////////////////////////////////////////////////////////

Input::Input()
	: m_directInput(nullptr),
	m_keyboard(nullptr),
	m_mouse(nullptr),
	m_mouseX(0),
	m_mouseY(0),
	m_F1_released(0),
	m_F2_released(0),
	m_F3_released(0),
	m_F4_released(0),
	m_prevMouseL(false)
{
	memset(m_keyboardState, 0, sizeof(m_keyboardState));
	m_mouseState = {};
} // Input


Input::~Input() {} // ~Input


bool Input::Init(HINSTANCE hinstance, HWND hwnd)
{
	HRESULT result;

	m_mouseX = 0;
	m_mouseY = 0;

	// DirectInput8 메인 객체 생성
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if (EngineHelper::CheckFailed(result, "DirectInput8Create 생성 실패")) return false;

	// 키보드 디바이스 생성
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if (EngineHelper::CheckFailed(result, "m_keyboard CreateDevice 생성 실패")) return false;

	// 키보드 데이터 형식 설정
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (EngineHelper::CheckFailed(result, "키보드 SetDataFormat 실패")) return false;

	// 키보드 협업 수준 설정
	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (EngineHelper::CheckFailed(result, "키보드 SetCooperativeLevel 실패")) return false;

	//  키보드 권한 획득
	result = m_keyboard->Acquire();
	if (EngineHelper::CheckFailed(result, "키보드 Acquire 실패")) {
	}

	// 마우스 디바이스 생성
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if (EngineHelper::CheckFailed(result, "마우스 CreateDevice 실패")) return false;

	// 마우스 데이터 형식 설정
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if (EngineHelper::CheckFailed(result, "마우스 SetDataFormat 실패")) return false;

	// 마우스 협업 수준 설정
	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (EngineHelper::CheckFailed(result, "마우스 SetCooperativeLevel 실패")) return false;

	// 마우스 권한 획득
	result = m_mouse->Acquire();
	if (EngineHelper::CheckFailed(result, "마우스 Acquire 실패")) {
	}

	return true;
} // Init


void Input::Shutdown()
{
	if (m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	if (m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	if (m_directInput)
	{
		m_directInput->Release();
		m_directInput = 0;
	}

	return;
} // Shutdown


bool Input::Frame()
{
	bool result;

	result = ReadKeyboard();
	if (result == false) return false;

	result = ReadMouse();
	if (result == false) return false;

	ProcessInput(); // 마우스와 키보드의 변경 사항을 처리

	m_prevMouseL = IsMouseLPressed() != 0;
	return true;
} // Frame
/////////////////////////////////////////////////////////////////////


/* 마우스 입력관련 */
/////////////////////////////////////////////////////////////////////

void Input::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = m_mouseX;
	mouseY = m_mouseY;
	return;
} // GetMouseLocation


bool Input::IsMouseLPressed()
{
	// 왼쪽 마우스 버튼의 상태를 확인
	if (m_mouseState.rgbButtons[0] & 0x80) return true;

	return false;
} // IsMouseLPressed


void Input::GetMouseDelta(int& x, int& y)
{
	x = m_mouseState.lX;
	y = m_mouseState.lY;
} // GetMouseDelta

int Input::GetMouseWheelDelta()
{
	return m_mouseState.lZ;
} // GetMouseWheelDelta

bool Input::IsLeftMouseDown()
{
	return (m_mouseState.rgbButtons[0] & 0x80) != 0;
} // IsLeftMouseDown


bool Input::IsRightMouseDown()
{
	return (m_mouseState.rgbButtons[1] & 0x80) != 0;
} // IsRightMouseDown


/////////////////////////////////////////////////////////////////////


/* 키보드 입력관련 */
bool Input::IsEscapePressed()
{
	if (m_keyboardState[DIK_ESCAPE] & 0x80) return true;

	return false;
} // IsEscapePressed


bool Input::IsLeftPressed()
{
	if (m_keyboardState[DIK_LEFT] & 0x80) return true;

	return false;
} // IsLeftPressed


bool Input::IsRightPressed()
{
	if (m_keyboardState[DIK_RIGHT] & 0x80) return true;

	return false;
} // IsRightPressed


bool Input::IsUpPressed()
{
	if (m_keyboardState[DIK_UP] & 0x80) return true;
	
	return false;
} // IsUpPressed


bool Input::IsDownPressed()
{
	if (m_keyboardState[DIK_DOWN] & 0x80) return true;
	
	return false;
} // IsDownPressed


bool Input::IsAPressed()
{
	if (m_keyboardState[DIK_A] & 0x80) return true;
	
	return false;
} // IsAPressed


bool Input::IsZPressed()
{
	if (m_keyboardState[DIK_Z] & 0x80) return true;

	return false;
} // IsZPressed


bool Input::IsWPressed()
{
	return (m_keyboardState[DIK_W] & 0x80);
} //  IsWPressed


bool Input::IsSPressed()
{
	return (m_keyboardState[DIK_S] & 0x80);
} // IsSPressed


bool Input::IsDPressed()
{
	return (m_keyboardState[DIK_D] & 0x80);
} // IsDPressed


bool Input::IsXPressed()
{
	return (m_keyboardState[DIK_X] & 0x80);
} // IsXPressed


bool Input::IsPgUpPressed()
{
	if (m_keyboardState[DIK_PGUP] & 0x80) return true;
	
	return false;
} // IsPgUpPressed


bool Input::IsPgDownPressed()
{
	if (m_keyboardState[DIK_PGDN] & 0x80) return true;
	
	return false;
} // IsPgDownPressed


bool Input::IsF1Toggled()
{
	if (m_keyboardState[DIK_F1] & 0x80)
	{
		if (m_F1_released)
		{
			m_F1_released = false;
			return true;
		}
	}
	else { m_F1_released = true; }
	
	return false;
} // IsF1Toggled


bool Input::IsF2Toggled()
{
	if (m_keyboardState[DIK_F2] & 0x80)
	{
		if (m_F2_released)
		{
			m_F2_released = false;
			return true;
		}
	}
	else { m_F2_released = true; }
	
	return false;
} // IsF2Toggled


bool Input::IsF3Toggled()
{
	if (m_keyboardState[DIK_F3] & 0x80)
	{
		if (m_F3_released)
		{
			m_F3_released = false;
			return true;
		}
	}
	else { m_F3_released = true; }
	
	return false;
} // IsF3Toggled


bool Input::IsF4Toggled()
{
	if (m_keyboardState[DIK_F4] & 0x80)
	{
		if (m_F3_released)
		{
			m_F3_released = false;
			return true;
		}
	}
	else { m_F3_released = true; }

	return false;
} // IsF4Toggled
/////////////////////////////////////////////////////////////////////


/* private */
/////////////////////////////////////////////////////////////////////

bool Input::ReadKeyboard()
{
	HRESULT result;

	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (FAILED(result))
	{
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
			m_keyboard->Acquire();
		else { return false; }
	}

	return true;
} // ReadKeyboard


bool Input::ReadMouse()
{
	HRESULT result;


	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if (FAILED(result))
	{
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
			m_mouse->Acquire();
		else return false;
	}

	return true;
} // ReadMouse


void Input::ProcessInput()
{
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	if (m_mouseX < 0) { m_mouseX = 0; }
	if (m_mouseY < 0) { m_mouseY = 0; }

	if (m_mouseX > EngineSettings::SCREEN_WIDTH) { m_mouseX = EngineSettings::SCREEN_WIDTH; }
	if (m_mouseY > EngineSettings::SCREEN_HEIGHT) { m_mouseY = EngineSettings::SCREEN_HEIGHT; }

	return;
} // ProcessInput
/////////////////////////////////////////////////////////////////////
