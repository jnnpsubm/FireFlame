#include <string>
#include "FLRenderer.h"
#include "FLD3DUtils.h"
#include "Exception\FLException.h"
#include "Timer\FLStopWatch.h"

namespace FireFlame {
void Renderer::Update(const StopWatch& gt) {
	mUpdateFunc(gt.DeltaTime());
}
void Renderer::Draw(const StopWatch& gt) {
	mDrawFunc(gt.DeltaTime());
}
void Renderer::FlushCommandQueue() {
	// Advance the fence value to mark commands up to this fence point.
	mCurrentFence++;

	// Add an instruction to the command queue to set a new fence point.  Because we 
	// are on the GPU timeline, the new fence point won't be set until the GPU finishes
	// processing all the commands prior to this Signal().
	ThrowIfFailed(mCommandQueue->Signal(mFence.Get(), mCurrentFence));

	// Wait until the GPU has completed commands up to this fence point.
	if (mFence->GetCompletedValue() < mCurrentFence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

		// Fire event when GPU hits current fence.  
		ThrowIfFailed(mFence->SetEventOnCompletion(mCurrentFence, eventHandle));

		// Wait until the GPU hits current fence event is fired.
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}
} // end namespace