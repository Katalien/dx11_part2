#pragma once
#include "Dev.h"
#include "engine/Engine.h"

class CameraInputUnit : public UnitBase {
private:
	float Speed = 3.0f;
	float Sencitivity = 60.0f;

	Camera* camera = nullptr;
	Input* input = nullptr;
public:
	CameraInputUnit();
	virtual void init() override;
	virtual void update() override;
	virtual ~CameraInputUnit() override;
};
