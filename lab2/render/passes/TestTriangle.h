#pragma once
#include "PassBase.h"

class TestTriangle : public PassBase {
private:
	Shader* vertexShader;
	Shader* pixelShader;
	ConstantBuffer<TriangleData>* constantBuffer;
public:
	TestTriangle();
	
	virtual void init() override;
	virtual void process() override;

	virtual ~TestTriangle() override;
};