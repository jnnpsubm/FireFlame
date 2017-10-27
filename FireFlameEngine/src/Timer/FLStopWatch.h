#pragma once

namespace FireFlame {
class StopWatch{
public:
	StopWatch();

	float TotalTime() const; // in seconds
	float DeltaTime() const; // in seconds

	void Reset();  // Call before use.
	void Pause();  // Call when paused.
	void Resume(); // Call when unpaused.
	void Mark();   // Mark now and get delta time from previous mark.

private:
	double  mSecondsPerCount;
	double  mDeltaTime;

	__int64 mBaseTime;
	__int64 mPausedTime;
	__int64 mTotalPausedTime;
	__int64 mPrevMarkTime;
	__int64 mCurrMarkTime;

	bool    mPaused;
};
} // end namespace