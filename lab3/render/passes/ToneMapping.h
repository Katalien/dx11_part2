#pragma once
#include "PassBase.h"

class ToneMappingPass : public PassBase {
private:
	float AdaptationSpeed = 1.5f;

	Shader* toneMappingVertexShader;
	Shader* toneMappingPixelShader;
	ConstantBuffer<ToneMappingCB>* tonemapCB;
public:
	ToneMappingPass();

	virtual void init() override;
	virtual void process() override;

	virtual ~ToneMappingPass() override;
};