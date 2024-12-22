#pragma once

#include "Base.h"


BEGIN(Engine)
enum class eKeyState
{
	Down,
	Pressed,
	Up,
	None,
};

enum class eKeyCode
{
	Q, W, E, R, T, Y, U, I, O, P,
	A, S, D, F, G, H, J, K, L,
	Z, X, C, V, B, N, M,
	Left, Right, Down, Up,
	Space, LButton, MButton, RButton,
	LCtrl, LShift, RShift,
	NUMPAD0, NUMPAD1, NUMPAD2, NUMPAD3,
	Zero, One, Two, Three, Four,
	Enter, Delete, ESC, BACKSPACE,
	LAlt, Tab,
	F1, F2, F3, F4, F5, F6, F7, F8, F9,
	End,
};

class CKey_Manager : public CBase
{
public:
	struct Key
	{
		eKeyCode eKeyCode;
		eKeyState eState;
		bool bPressed;
	};

	void Initialize();
	void Update();

	bool GetKeyDown(eKeyCode _keyCode) { return vecKey[(UINT)_keyCode].eState == eKeyState::Down; }
	bool GetKeyUp(eKeyCode _keyCode) {	return	vecKey[(UINT)_keyCode].eState == eKeyState::Up; }
	bool GetKey(eKeyCode _keyCode) {	return	vecKey[(UINT)_keyCode].eState == eKeyState::Pressed || vecKey[(UINT)_keyCode].eState == eKeyState::Down;}
	bool GetKeyNone(eKeyCode _keyCode) { return vecKey[(UINT)_keyCode].eState == eKeyState::None || vecKey[(UINT)_keyCode].eState == eKeyState::Up;}
private:
	CKey_Manager() {};
	~CKey_Manager() = default;

	void createKeys();
	void updateKeys();
	void updateKey(Key& key);
	void updateKeyDown(Key& key);
	void updateKeyUp(Key& key);
	bool isKeyDown(eKeyCode code);
	void clearKeys();

private:
	vector<Key> vecKey;

public:
	static CKey_Manager* Create();
	void Free() override;
};

END
