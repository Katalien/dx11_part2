#pragma once
#include "Dev.h"
#include "engine/Engine.h"

class CameraInputUnit : public UnitBase {
private:
	Camera* camera = nullptr;
	Input* input = nullptr;
public:
	float Speed = 10.0f;
	float Sencitivity = 60.0f;

	CameraInputUnit();
	virtual void init() override;
	virtual void update() override;
	virtual ~CameraInputUnit() override;
};
