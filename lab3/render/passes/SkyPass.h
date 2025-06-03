#pragma once
#include "PassBase.h"

class SkyPass : public PassBase {
private:
	Shader* skyPassVertexShader;
	Shader* skyPassPixelShader;
	Geometry* geometry;

	ID3D11RasterizerState* skyRasterizerState;
public:
	SkyPass();

	virtual void init() override;
	virtual void process() override;

	virtual ~SkyPass() override;
};