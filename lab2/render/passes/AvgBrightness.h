#pragma once
#include "PassBase.h"

class AvgBrightnessPass : public PassBase {
private:
	Shader* firstCopyVertexShader;
	Shader* firstCopyPixelShader;

	Shader* copyVertexShader;
	Shader* copyPixelShader;

	Texture* TempMonoTextures[10];
	RenderTarget* TempMonoTargets[10];

	Texture* avgBrightnessTex;
public:
	AvgBrightnessPass();

	virtual void init() override;
	virtual void process() override;

	virtual ~AvgBrightnessPass() override;
};