#pragma once

#define SINGLE(type) public:\
	static type* Instance()\
	{\
	static type mgr;\
	return &mgr;\
	}\
	private:\
		type();\
		~type();

#define KEY_CHECK(key, state) KeyMgr::Instance()->GetKeyState(key) == state
#define KEY_HOLD(key) KEY_CHECK(key, KEY_STATE::HOLD)
#define KEY_AWAY(key) KEY_CHECK(key, KEY_STATE::AWAY)
#define KEY_TAP(key) KEY_CHECK(key, KEY_STATE::TAP)
#define KEY_NONE(key) KEY_CHECK(key, KEY_STATE::NONE)


enum class GROUP_TYPE {
	DEFAULT,
	TEST,
	PLAY,

	END = 32,
};

enum class SCENE_TYPE {
	TEST,
	PLAY,

	END,
};