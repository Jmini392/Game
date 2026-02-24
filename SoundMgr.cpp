#include "stdafx.h"
#include "SoundMgr.h"

SoundMgr::SoundMgr() : m_bInitialized(false)
{

}

SoundMgr::~SoundMgr()
{
	Release();
}

void SoundMgr::Init()
{
	ma_result result = ma_engine_init(NULL, &m_engine);
	if (result != MA_SUCCESS) {
		OutputDebugString(_T("SoundMgr: ma_engine_init failed!\n"));
		return;
	}
	m_bInitialized = true;
}

void SoundMgr::Release()
{
	StopAll();

	if (m_bInitialized) {
		ma_engine_uninit(&m_engine);
		m_bInitialized = false;
	}
}

void SoundMgr::Play(const std::string& key, const char* filePath, float volume, bool loop)
{
	if (!m_bInitialized) return;

	Stop(key);

	ma_sound* pSound = new ma_sound();
	ma_result result = ma_sound_init_from_file(
		&m_engine, filePath,
		MA_SOUND_FLAG_STREAM,
		NULL, NULL, pSound);

	if (result != MA_SUCCESS) {
		OutputDebugString(_T("SoundMgr: Sound load failed!\n"));
		delete pSound;
		return;
	}

	ma_sound_set_looping(pSound, loop ? MA_TRUE : MA_FALSE);
	ma_sound_set_volume(pSound, volume);
	ma_sound_start(pSound);

	m_mapSounds[key] = pSound;
}

void SoundMgr::Stop(const std::string& key)
{
	auto it = m_mapSounds.find(key);
	if (it != m_mapSounds.end()) {
		ma_sound_stop(it->second);
		ma_sound_uninit(it->second);
		delete it->second;
		m_mapSounds.erase(it);
	}
}

void SoundMgr::Pause(const std::string& key)
{
	auto it = m_mapSounds.find(key);
	if (it != m_mapSounds.end()) {
		ma_sound_stop(it->second);
	}
}

void SoundMgr::Resume(const std::string& key)
{
	auto it = m_mapSounds.find(key);
	if (it != m_mapSounds.end()) {
		ma_sound_start(it->second);
	}
}

void SoundMgr::SetVolume(const std::string& key, float volume)
{
	auto it = m_mapSounds.find(key);
	if (it != m_mapSounds.end()) {
		ma_sound_set_volume(it->second, volume);
	}
}

bool SoundMgr::IsPlaying(const std::string& key)
{
	auto it = m_mapSounds.find(key);
	if (it != m_mapSounds.end()) {
		return ma_sound_is_playing(it->second) == MA_TRUE;
	}
	return false;
}

void SoundMgr::StopAll()
{
	for (auto& pair : m_mapSounds) {
		ma_sound_stop(pair.second);
		ma_sound_uninit(pair.second);
		delete pair.second;
	}
	m_mapSounds.clear();
}

void SoundMgr::PauseAll()
{
	for (auto& pair : m_mapSounds) {
		ma_sound_stop(pair.second);
	}
}

void SoundMgr::ResumeAll()
{
	for (auto& pair : m_mapSounds) {
		ma_sound_start(pair.second);
	}
}

void SoundMgr::PlaySFX(const char* filePath, float volume)
{
	if (!m_bInitialized) return;
	ma_engine_play_sound(&m_engine, filePath, NULL);
}