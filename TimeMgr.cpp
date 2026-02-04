#include "stdafx.h"
#include "TimeMgr.h"
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

TimeMgr::TimeMgr()
{
}

TimeMgr::~TimeMgr()
{
}

void TimeMgr::Init()
{
	if (::QueryPerformanceFrequency((LARGE_INTEGER*)&m_nPerformanceFrequency))
	{
		m_bHardwareHasPerformanceCounter = TRUE;
		::QueryPerformanceCounter((LARGE_INTEGER*)&m_nLastTime);
		m_dTimeScale = 1.0 / m_nPerformanceFrequency;
	}
	else
	{
		m_bHardwareHasPerformanceCounter = FALSE;
		m_nLastTime = ::timeGetTime();
		m_dTimeScale = 0.001;
	}

	m_nSampleCount = 0;
	m_nCurrentFrameRate = 0;
	m_nFramesPerSecond = 0;
	m_dFPSTimeElapsed = 0.0;
	m_dTimeElapsed = 0.0;
}

void TimeMgr::Update(double dLockFPS)
{
	if (m_bHardwareHasPerformanceCounter)
	{
		::QueryPerformanceCounter((LARGE_INTEGER*)&m_nCurrentTime);
	}
	else
	{
		m_nCurrentTime = ::timeGetTime();
	}

	// 마지막으로 이 함수를 호출한 이후 경과한 시간을 계산
	double dTimeElapsed = (m_nCurrentTime - m_nLastTime) * m_dTimeScale;

	if (dLockFPS > 0.0)
	{
		//이 함수의 파라미터(fLockFPS)가 0보다 크면 이 시간만큼 호출한 함수를 기다리게 함.
		while (dTimeElapsed < (1.0 / dLockFPS))
		{
			if (m_bHardwareHasPerformanceCounter)
			{
				::QueryPerformanceCounter((LARGE_INTEGER*)&m_nCurrentTime);
			}
			else
			{
				m_nCurrentTime = ::timeGetTime();
			}
			// 마지막으로 이 함수를 호출한 이후 경과한 시간을 계산
			dTimeElapsed = (m_nCurrentTime - m_nLastTime) * m_dTimeScale;
		}
	}

	// 현재 시간을 m_nLastTime에 저장
	m_nLastTime = m_nCurrentTime;

	// 마지막 프레임 처리 시간과 현재 프레임 처리 시간의 차이가 1초보다 작으면, 현재 프레임 처리 시간을 m_fFrameTime[0]에 저장
	if (fabs(dTimeElapsed - m_dTimeElapsed) < 1.0)
	{
		::memmove(&m_dFrameTime[1], m_dFrameTime, (MAX_SAMPLE_COUNT - 1) * sizeof(double));
		m_dFrameTime[0] = dTimeElapsed;
		if (m_nSampleCount < MAX_SAMPLE_COUNT) m_nSampleCount++;
	}

	// 초당 프레임 수를 1 증가시키고 현재 프레임 처리 시간을 누적하여 저장
	m_nFramesPerSecond++;
	m_dFPSTimeElapsed += dTimeElapsed;
	if (m_dFPSTimeElapsed > 1.0)
	{
		m_nCurrentFrameRate = m_nFramesPerSecond;
		m_nFramesPerSecond = 0;
		m_dFPSTimeElapsed = 0.0;
	}

	// 누적된 프레임 처리 시간의 평균을 구하여 프레임 처리 시간을 구함.
	m_dTimeElapsed = 0.0;
	for (ULONG i = 0; i < m_nSampleCount; i++) m_dTimeElapsed += m_dFrameTime[i];
	if (m_nSampleCount > 0) m_dTimeElapsed /= m_nSampleCount;

#ifdef _DEBUG
	//디버그 모드에서 중단점 오래 걸면 시간이 말도 안되게 커질 때가 있음
	if (m_dTimeElapsed > (1. / 60.))
		m_dTimeElapsed = (1. / 60.);

#endif // _DEBUG
}

void TimeMgr::Reset()
{
	__int64 nPerformanceCounter;
	::QueryPerformanceCounter((LARGE_INTEGER*)&nPerformanceCounter);

	m_nLastTime = nPerformanceCounter;
	m_nCurrentTime = nPerformanceCounter;

	m_bStopped = false;
}