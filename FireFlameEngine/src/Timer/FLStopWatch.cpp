#include <windows.h>
#include "FLStopWatch.h"

namespace FireFlame {
StopWatch::StopWatch()
	: mSecondsPerCount(0.0), mBaseTime(0),
	  mPrevMarkTime(0), mCurrMarkTime(0), mDeltaTime(-1.0),
	  mPausedTime(0), mTotalPausedTime(0), mPaused(false){
	  __int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	mSecondsPerCount = 1.0 / (double)countsPerSec;
}

// Returns the total time elapsed since Reset() was called, NOT counting any
// time when the clock is paused.
float StopWatch::TotalTime()const{
	// If we are stopped, do not count the time that has passed since we stopped.
	// Moreover, if we previously already had a pause, the distance 
	// mStopTime - mBaseTime includes paused time, which we do not want to count.
	// To correct this, we can subtract the paused time from mStopTime:  
	//
	//                     |<--total paused time-->|
	// ----*---------------*-----------------------*--------------*-------------*------> time
	//  mBaseTime      mPausedTime             resumeTime      mStopTime    mCurrTime
	if (mPaused){
		return (float)(((mPausedTime - mTotalPausedTime) - mBaseTime)*mSecondsPerCount);
	}

	// The distance mCurrTime - mBaseTime includes paused time,
	// which we do not want to count.  To correct this, we can subtract 
	// the paused time from mCurrTime:  
	//
	//  (mCurrTime - mPausedTime) - mBaseTime 
	//
	//                     |<--total paused time-->|
	// ----*---------------*-----------------------*--------------*------> time
	//  mBaseTime     mPausedTime              resumeTime      mCurrTime
	else{
		return (float)(((mCurrMarkTime - mTotalPausedTime) - mBaseTime)*mSecondsPerCount);
	}
}

float StopWatch::DeltaTime() const{
	return (float)mDeltaTime;
}

void StopWatch::Reset(){
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	mBaseTime = currTime;
	mPrevMarkTime = currTime;
	mPausedTime = 0;
	mPaused = false;
}

void StopWatch::Resume(){
	__int64 resumeTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&resumeTime);
	// Accumulate the time elapsed between stop and start pairs.
	//
	//                      |<-------d------->|
	// ----*----------------*-----------------*------------> time
	//  mBaseTime       mPausedTime        resumeTime     
	if (mPaused){
		mTotalPausedTime += (resumeTime - mPausedTime);
		mPrevMarkTime = resumeTime;
		mPausedTime = 0;
		mPaused = false;
	}
}

void StopWatch::Pause(){
	if (!mPaused){
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		mPausedTime = currTime;
		mPaused = true;
	}
}

void StopWatch::Mark(){
	if (mPaused){
		mDeltaTime = 0.0;
		return;
	}

    // todo : temporary fix, laptop too hot.............need to fix fps
    while (true) {
        __int64 currTime;
        QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
        mDeltaTime = (currTime - mPrevMarkTime)*mSecondsPerCount;
        if (mDeltaTime < 0.008)
            Sleep(0);
        else
            break;
    }

	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	mCurrMarkTime = currTime;

	// Time difference between this frame and the previous.
	mDeltaTime = (mCurrMarkTime - mPrevMarkTime)*mSecondsPerCount;

	// Prepare for next frame.
	mPrevMarkTime = mCurrMarkTime;

	// Force nonnegative.  The DXSDK's CDXUTTimer mentions that if the 
	// processor goes into a power save mode or we get shuffled to another
	// processor, then mDeltaTime can be negative.
	if (mDeltaTime < 0.0){
		mDeltaTime = 0.0;
	}
}
} // end namespace
