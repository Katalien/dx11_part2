#pragma once

#include "Dev.h"

class UnitBase {
protected:
	Engine* engine;
public:
	UnitBase() = default;

	void setEngine(Engine* eng);
	virtual void init() = 0;
	virtual void update() = 0;

	virtual ~UnitBase() {}
};
