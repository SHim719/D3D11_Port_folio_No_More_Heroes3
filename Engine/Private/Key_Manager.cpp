#include "Key_Manager.h"

int ASCII[(UINT)eKeyCode::End] =
{
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P',
	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L',
	'Z', 'X', 'C', 'V', 'B', 'N', 'M',
	VK_LEFT, VK_RIGHT, VK_DOWN, VK_UP,
	VK_SPACE, VK_LBUTTON, VK_MBUTTON, VK_RBUTTON,
	VK_LCONTROL, VK_LSHIFT, VK_RSHIFT,
	VK_NUMPAD0, VK_NUMPAD1, VK_NUMPAD2, VK_NUMPAD3,
	'0', '1', '2', '3' ,'4', VK_RETURN, VK_DELETE, VK_ESCAPE, VK_BACK,
	VK_LMENU, VK_TAB,
	VK_F1, VK_F2, VK_F3, VK_F4, VK_F5, VK_F6, VK_F7, VK_F8, VK_F9,
};

void CKey_Manager::Initialize()
{
	createKeys();

}

void CKey_Manager::Update()
{
	updateKeys();
}

void CKey_Manager::createKeys()
{
	vecKey.reserve((size_t)eKeyCode::End);
	for (size_t i = 0; i < (size_t)eKeyCode::End; ++i)
	{
		Key key;
		key.eKeyCode = (eKeyCode)i;
		key.eState = eKeyState::None;
		key.bPressed = false;

		vecKey.push_back(key);
	}
}

void CKey_Manager::updateKeys()
{
	for_each(vecKey.begin(), vecKey.end(),
		[this](Key& Key) -> void
		{
			updateKey(Key);
		});
}

void CKey_Manager::updateKey(Key& key)
{
	if (GetFocus())
	{
		if (isKeyDown(key.eKeyCode))
		{
			updateKeyDown(key);
		}
		else
		{
			updateKeyUp(key);
		}

	}
	else
	{
		clearKeys();
	}
}

void CKey_Manager::updateKeyDown(Key& key)
{
	if (key.bPressed)
		key.eState = eKeyState::Pressed;
	else
		key.eState = eKeyState::Down;

	key.bPressed = true;
}

void CKey_Manager::updateKeyUp(Key& key)
{
	if (key.bPressed)
		key.eState = eKeyState::Up;
	else
		key.eState = eKeyState::None;

	key.bPressed = false;
}

bool CKey_Manager::isKeyDown(eKeyCode code)
{
	return GetAsyncKeyState(ASCII[(UINT)code]) & 0x8000;
}

void CKey_Manager::clearKeys()
{
	for (Key& key : vecKey)
	{
		if (key.eState == eKeyState::Down || key.eState == eKeyState::Pressed)
			key.eState = eKeyState::Up;
		else if (key.eState == eKeyState::Up)
			key.eState = eKeyState::None;
		key.bPressed = false;
	}
}

CKey_Manager* CKey_Manager::Create()
{
	CKey_Manager* pInstance = new CKey_Manager();

	pInstance->Initialize();

	return pInstance;
}

void CKey_Manager::Free()
{
	__super::Free();


}


