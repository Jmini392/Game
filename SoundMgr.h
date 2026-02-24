#pragma once

#include "define.h"
#include "miniaudio.h"
#include <string>
#include <map>

class SoundMgr
{
	SINGLE(SoundMgr);

private:
	ma_engine m_engine;
	bool m_bInitialized;

	std::map<std::string, ma_sound*> m_mapSounds;

public:
	void Init();
	void Release();

	// 사운드 재생
	void Play(const std::string& key, const char* filePath, float volume = 1.0f, bool loop = false);
	void Stop(const std::string& key);
	void Pause(const std::string& key);
	void Resume(const std::string& key);
	void SetVolume(const std::string& key, float volume);
	bool IsPlaying(const std::string& key);

	// 전체 제어
	void StopAll();
	void PauseAll();
	void ResumeAll();

	// 효과음
	void PlaySFX(const char* filePath, float volume = 1.0f);
};