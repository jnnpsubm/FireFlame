#pragma once

namespace FireFlame {
class StopWatch
{
public:
	StopWatch();

	float TotalTime() const; // in seconds
	float DeltaTime() const; // in seconds

	void Reset(); // Call before use.
	void Start(); // Call when unpaused.
	void Stop();  // Call when paused.
	void Tick();  // Call every break.

private:
	double mSecondsPerCount;
	double mDeltaTime;

	__int64 mBaseTime;
	__int64 mPausedTime;
	__int64 mStopTime;
	__int64 mPrevTime;
	__int64 mCurrTime;

	bool mStopped;
};
} // end namespace