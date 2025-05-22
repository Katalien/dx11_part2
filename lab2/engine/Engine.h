#pragma once

#include "Dev.h"
#include "win/Win.h"
#include "render/Render.h"
#include "timer/Timer.h"
#include "input/Input.h"
#include "units/UnitBase.h"

class Engine {
private:
	Win* win;
	Timer* timer;
	Input* input;
	Render* render;
	std::vector<UnitBase*> units;
public:

	Engine();

	void init(HINSTANCE hInstance);

	void run();

	void addUnit(UnitBase* unit);

	Win* getWin() { return win; }
	Timer* getTimer() { return timer; }
	Input* getInput() { return input; }
	Render* getRender() { return render; }

	void frame();
	void resize(UINT width, UINT height);

	~Engine();
};