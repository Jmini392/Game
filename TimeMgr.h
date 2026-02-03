#pragma once
const ULONG MAX_SAMPLE_COUNT = 50; // 50회의 프레임 처리시간을 누적하여 평균.

class TimeMgr
{
	SINGLE(TimeMgr);
private:
	bool m_bHardwareHasPerformanceCounter;
	double m_dTimeScale;
	double m_dTimeElapsed;
	__int64 m_nCurrentTime;
	__int64 m_nLastTime;
	__int64 m_nPerformanceFrequency;

	double m_dFrameTime[MAX_SAMPLE_COUNT];
	ULONG m_nSampleCount;

	unsigned long m_nCurrentFrameRate;
	unsigned long m_nFramesPerSecond;
	double m_dFPSTimeElapsed;

	bool m_bStopped;
public:
	void Init();
	void Update(double dLockFPS = 0.0);
	void Reset();
	
	double getDT() { return m_dTimeElapsed; }
	float getfDT() { return (float)m_dTimeElapsed; }
};

