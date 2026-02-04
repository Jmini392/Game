#include "stdafx.h"
#include "KeyMgr.h"

int arrVK[(int)KEY::LAST] =
{
	VK_LEFT,
	VK_RIGHT,
	VK_UP,
	VK_DOWN,
	'Q',
	'W',
	'E',
	'R',
	'T',
	'Y',
	'U',
	'I',
	'O',
	'P',
	'A',
	'S',
	'D',
	'F',
	'G',
	'H',
	'J',
	'K',
	'L',
	'Z',
	'X',
	'C',
	'V',
	'B',
	'N',
	'M',
	VK_MENU,
	VK_CONTROL,
	VK_LSHIFT,
	VK_SPACE,
	VK_RETURN,
	VK_ESCAPE,
};

KeyMgr::KeyMgr()
{

}

KeyMgr::~KeyMgr()
{

}

void KeyMgr::Init()
{
	for (int i = 0; i < (int)KEY::LAST; ++i) {
		vecKey.emplace_back(KeyInfo{ KEY_STATE::NONE, false });
	}
}

void KeyMgr::Update()
{
	//윈도우 창을 포커싱하냐? (창을 내렸는데도 키 입력을 받으면 안된다)
	HWND hWnd = GetFocus();

	if (nullptr != hWnd) {
		//모든 키에 대해서
		for (int i = 0; i < (int)KEY::LAST; ++i) {
			// 키가 눌렸는지?
			if (GetAsyncKeyState(arrVK[i]) & 0x8000) {
				if (vecKey[i].prev) {
					vecKey[i].state = KEY_STATE::HOLD;
				}
				else {
					vecKey[i].state = KEY_STATE::TAP;
				}
				vecKey[i].prev = true;
			}
			else {
				if (vecKey[i].prev) {
					vecKey[i].state = KEY_STATE::AWAY;
				}
				else {
					vecKey[i].state = KEY_STATE::NONE;
				}
				vecKey[i].prev = false;
			}
		}
	}
	else {
		for (int i = 0; i < (int)KEY::LAST; ++i) {
			vecKey[i].prev = false;
			if (KEY_STATE::TAP == vecKey[i].state || KEY_STATE::HOLD == vecKey[i].state) {
				vecKey[i].state = KEY_STATE::AWAY;
			}
			else if (KEY_STATE::AWAY == vecKey[i].state) {
				vecKey[i].state = KEY_STATE::NONE;
			}
		}
	}
}
