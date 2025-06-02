#pragma once
#include "Dev.h"
#include <chrono>

class Timer {
private:
    double MaxDTime = 5.0;
    std::chrono::duration<double> DefaultDTime = std::chrono::duration<double>(1.0 / 60);
    static const int LastDTimesAmount = 60;

    Engine* engine;

    std::chrono::duration<double> timeFromAppStart = std::chrono::duration<double>::zero();
    std::chrono::duration<double> dTime = std::chrono::duration<double>::zero();

    double dTimes[LastDTimesAmount] = {};
    int size = 0;
    int lastDTimeIndex = 0;
    int fps = 60;

public:
    Timer(Engine * eng);

    void update();

    int getFPS();
    double getTimeFromAppStart() { return timeFromAppStart.count(); }
    double getDeltaTime() { return dTime.count(); }

    ~Timer() = default;
};

