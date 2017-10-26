#pragma once

class EngineUser {
public:
	void Update(float time_elapsed);
	void Draw(float time_elapsed);
};

struct stEngineUser {
	void operator()(float time_elapsed) {

	}
};

inline void Update_OutClassScope(void* usrdata, float time_elapsed) {

}