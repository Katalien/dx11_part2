#include "engine\Engine.h"
#include "Timer.h"

Timer::Timer(Engine* eng) : engine(eng)
{
}

void Timer::update() {
	auto currTime = std::chrono::high_resolution_clock::now().time_since_epoch();
	dTime = currTime - timeFromAppStart;
	if (dTime.count() > MaxDTime) {
		dTime = std::chrono::duration<double>(DefaultDTime);
	}
	dTimes[lastDTimeIndex] = dTime.count();
	lastDTimeIndex = (lastDTimeIndex + 1) % LastDTimesAmount;
	size = min(size + 1, LastDTimesAmount);

	double sum = 0;
	for (int i = 0; i < size; i++) {
		sum += dTimes[i];
	}

	fps = int(sum / size);

	timeFromAppStart = currTime;
}

int Timer::getFPS() {
	return fps;
}
