#pragma once
#include "Dev.h"
#include "shaders/shared.h"

class PassBase {
protected:
	Render* render;

public:
	PassBase() {}

	void setRender(Render* rend);
	virtual void init() = 0;
	virtual void process() = 0;

	virtual ~PassBase() {}
};

// forward declaration
class Shader;
class Geometry;