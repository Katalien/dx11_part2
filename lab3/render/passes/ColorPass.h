#pragma once
#include "PassBase.h"

class ColorPass : public PassBase {
private:
	Shader* vertexShader;
	Shader* pixelShader;
	ConstantBuffer<PerMeshData>* cb;
public:
	ColorPass();

	virtual void init() override;
	virtual void process() override;

	virtual ~ColorPass() override;
};