#pragma once
#include "Dev.h"
#include "engine/Engine.h"

class TestUnit : public UnitBase {
public:
	TestUnit();
	virtual void init() override;
	virtual void update() override;
	virtual ~TestUnit() override;
};
