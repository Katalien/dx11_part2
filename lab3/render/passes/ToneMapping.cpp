#include "render/Render.h"
#include "ToneMapping.h"

ToneMappingPass::ToneMappingPass() {

}

void ToneMappingPass::init() {
	toneMappingVertexShader = render->getShaderManager()->getShader("shaders/ToneMappingPass.hlsl", Shader::Stage::Vertex);
	toneMappingPixelShader = render->getShaderManager()->getShader("shaders/ToneMappingPass.hlsl", Shader::Stage::Pixel);

	tonemapCB = new ConstantBuffer<ToneMappingCB>(render);
}

void ToneMappingPass::process() {
	render->BeginEvent(L"ToneMappingPass");
	
	// update CB data
	auto& cb = tonemapCB->getData();
	// eye adaptetion
	cb.avgBrightnessLog = render->getAdaptedAvgBrigtness() + (render->getCurAvgBrigtness() - render->getAdaptedAvgBrigtness()) * (1 - exp(-render->getDTime() * AdaptationSpeed));
	cb.sceneExposure = 8;
	render->setAdaptedAvgBrightness(cb.avgBrightnessLog);

	// set target 
	render->getRenderTargetManager()->reset();
	render->getBackBufferTarget()->setActive();
	render->getRenderTargetManager()->apply();

	// set shader
	toneMappingVertexShader->setActive();
	toneMappingPixelShader->setActive();

	// set samplers
	render->getTextureManager()->setSamplers(toneMappingVertexShader);
	render->getTextureManager()->setSamplers(toneMappingPixelShader);

	// set cb
	tonemapCB->setActive(toneMappingPixelShader, TONEMAPPING_CB_SLOT);

	// set prev tex
	render->getHDRTexture()->setActive(toneMappingPixelShader, TEXTURE_COLOR_SRV_SLOT);

	// run shader
	render->getD3DDeviceContext()->Draw(6, 0);

	render->EndEvent();
}

ToneMappingPass::~ToneMappingPass() {
	delete tonemapCB;
}

