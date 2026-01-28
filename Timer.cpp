#include <windows.h>
#include "Timer.h"

Timer::Timer()
	: mSecondsPerCount(0.0), mDeltaTime(-1.0), mBaseTime(0),
	mPausedTime(0), mStopTime(0), mPrevTime(0), mCurrTime(0), mStopped(false)
{
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec); // 고해상도 성능 카운터 주파수 얻기
	mSecondsPerCount = 1.0 / static_cast<double>(countsPerSec);
}

float Timer::TotalTime() const
{
	if (mStopped)
	{
		return static_cast<float>(((mStopTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
	}
	else
	{
		return static_cast<float>(((mCurrTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
	}
}

float Timer::DeltaTime() const
{
	return static_cast<float>(mDeltaTime);
}

void Timer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime); // 현재 시간 얻기

	mBaseTime = currTime;
	mPrevTime = currTime;
	mStopTime = 0;
	mStopped = false;
}

void Timer::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime); // 현재 시간 얻기

	if (mStopped) // 정지 상태에서 시작할 때
	{
		mPausedTime += (startTime - mStopTime);
		mPrevTime = startTime;
		mStopTime = 0;
		mStopped = false;
	}
}

void Timer::Stop()
{
	if (!mStopped) // 실행 중일 때 정지
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime); // 현재 시간 얻기

		mStopTime = currTime;
		mStopped = true;
	}
}

void Timer::Tick()
{
	if (mStopped) // 정지 상태일 때는 시간이 흐르지 않음
	{
		mDeltaTime = 0.0;
		return;
	}

	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime); // 현재 시간 얻기
	mCurrTime = currTime;

	mDeltaTime = (mCurrTime - mPrevTime) * mSecondsPerCount; // 이전 프레임과 현재 프레임 사이의 시간 간격 계산
	mPrevTime = mCurrTime;

	if (mDeltaTime < 0.0) // 음수 방지
	{
		mDeltaTime = 0.0;
	}
}