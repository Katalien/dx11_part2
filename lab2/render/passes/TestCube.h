#pragma once
#include "PassBase.h"

class TestCube : public PassBase {
private:
	Shader* vertexShader;
	Shader* pixelShader;
	Geometry* cubeGeometry;
	ConstantBuffer<PerMeshData>* cubeCB;
public:
	TestCube();

	virtual void init() override;
	virtual void process() override;

	virtual ~TestCube() override;
};