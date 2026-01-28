#pragma once

class Timer
{
public:
	Timer();
	~Timer() {};

	float TotalTime() const; // 총 경과 시간(초)
	float DeltaTime() const; // 이전 프레임과 현재 프레임 사이의 시간 간격(초)

	void Reset(); // 타이머 리셋
	void Start(); // 타이머 시작
	void Stop();  // 타이머 정지
	void Tick();  // 타이머 틱(프레임 갱신)
private:
	double mSecondsPerCount; // 카운트당 초
	double mDeltaTime;       // 이전 프레임과 현재 프레임 사이의 시간 간격(초)

	// __int64: 64비트 정수형
	__int64 mBaseTime;      // 타이머 시작 시점
	__int64 mPausedTime;    // 일시 정지된 시간 누적
	__int64 mStopTime;      // 타이머가 정지된 시점
	__int64 mPrevTime;      // 이전 프레임의 시간
	__int64 mCurrTime;      // 현재 프레임의 시간

	bool mStopped;          // 타이머 정지 여부

};

