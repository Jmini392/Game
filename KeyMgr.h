#pragma once

//키 입력 이벤트 정의
enum class KEY_STATE {
	NONE,		/*이번 프레임에도 눌리지 않고, 현재 프레임에서도 눌리지 않음*/
	TAP,		/*이번 프레임에도 눌리지 않고, 현재 프레임에서 막 눌림*/
	HOLD,		/*이번 프레임에도 눌렸고, 현재 프레임에서 눌림*/
	AWAY,		/*이번 프레임에도 눌렸고, 현재 프레임에서 막 땠음*/
};

//지월 할 키 정의
enum class KEY {
	LEFT,
	RIGHT,
	UP,
	DOWN,
	Q,
	W,
	E,
	R,
	T,
	Y,
	U,
	I,
	O,
	P,
	A,
	S,
	D,
	F,
	G,
	H,
	J,
	K,
	L,
	Z,
	X,
	C,
	V,
	B,
	N,
	M,
	ALT,
	CTRL,
	LSHIFT,
	SPACE,
	ENTER,
	ESC,

	LAST,
};

//키 정보 구조체
struct KeyInfo {
	KEY_STATE	state;
	bool		prev;		//이전 프레임에서 키가 눌렸는지
};

class KeyMgr
{
	SINGLE(KeyMgr);
private:
	std::vector<KeyInfo> vecKey;

public:
	void Init();
	void Update();

	KEY_STATE GetKeyState(KEY key) { return vecKey[(int)key].state; }
};

