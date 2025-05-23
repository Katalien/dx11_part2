#include "render/Render.h"
#include "AvgBrightness.h"

AvgBrightnessPass::AvgBrightnessPass() {

}

void AvgBrightnessPass::init() {
	firstCopyVertexShader = render->getShaderManager()->getShader("shaders/FirstCopyAvgBrightnessPass.hlsl", Shader::Stage::Vertex);
	firstCopyPixelShader = render->getShaderManager()->getShader("shaders/FirstCopyAvgBrightnessPass.hlsl", Shader::Stage::Pixel);

	copyVertexShader = render->getShaderManager()->getShader("shaders/CopyAvgBrightnessPass.hlsl", Shader::Stage::Vertex);
	copyPixelShader = render->getShaderManager()->getShader("shaders/CopyAvgBrightnessPass.hlsl", Shader::Stage::Pixel);


	TextureDescriptor texDesc = {};
	texDesc.mips = 1;
	texDesc.genMipMaps = false;
	texDesc.format = DXGI_FORMAT_R32_FLOAT;
	texDesc.isRenderTarget = true;
	texDesc.isDepthTexture = false;
	texDesc.isReadback = false;
	texDesc.pixels = nullptr;

	for (int i = 0; i < 10; i++)
	{
		texDesc.name = std::string("Avg Brighness tex size ") + std::to_string(1 << i);
		texDesc.w = 1 << i;
		texDesc.h = 1 << i;
		TempMonoTextures[i] = render->getTextureManager()->addTexture(texDesc);
		TempMonoTargets[i] = render->getRenderTargetManager()->addRenderTarget(TempMonoTextures[i]);
	}

	texDesc.name = "Avg Brightness Readback texture";
	texDesc.w = 1;
	texDesc.h = 1;
	texDesc.isRenderTarget = false;
	texDesc.isReadback = true;

	avgBrightnessTex = render->getTextureManager()->addTexture(texDesc);
}

void AvgBrightnessPass::process() {
	render->BeginEvent(L"AvgBrightnessPass");
	
	// find appropriate level
	uint32_t highLevel = min(9, max(1, log2(min(render->getHDRTexture()->getWidth(), render->getHDRTexture()->getHeight()))));

	{
		render->BeginEvent(L"Copy HDR brightness");
		// set target 
		render->getRenderTargetManager()->reset();
		TempMonoTargets[highLevel]->setActive();
		render->getRenderTargetManager()->apply();

		// set shader
		firstCopyVertexShader->setActive();
		firstCopyPixelShader->setActive();

		// set samplers
		render->getTextureManager()->setSamplers(firstCopyVertexShader);
		render->getTextureManager()->setSamplers(firstCopyPixelShader);

		// set prev tex
		render->getHDRTexture()->setActive(firstCopyPixelShader, TEXTURE_COLOR_SRV_SLOT);
		
		// run shader
		render->getD3DDeviceContext()->Draw(6, 0);

		render->EndEvent();
	}

	render->BeginEvent(L"Downsample");
	for (int destLevel = highLevel - 1; destLevel >= 0; destLevel--)
	{
		render->BeginEvent((std::to_wstring(destLevel + 1) + L"->" + std::to_wstring(destLevel)).c_str());

		// set target 
		render->getRenderTargetManager()->reset();
		TempMonoTargets[destLevel]->setActive();
		render->getRenderTargetManager()->apply();

		// set shader
		copyVertexShader->setActive();
		copyPixelShader->setActive();

		// set samplers
		render->getTextureManager()->setSamplers(copyVertexShader);
		render->getTextureManager()->setSamplers(copyPixelShader);

		// set prev tex
		TempMonoTextures[destLevel + 1]->setActive(copyPixelShader, TEXTURE_COLOR_SRV_SLOT);

		// run shader
		render->getD3DDeviceContext()->Draw(6, 0);

		render->EndEvent();
	}
	render->EndEvent();

	render->getTextureManager()->copyTexture(avgBrightnessTex, TempMonoTextures[0]);

	render->EndEvent();

	auto pixels = avgBrightnessTex->getPixels();
	float* pixelsAsFloats = reinterpret_cast<float*>(pixels.data());

	render->setCurAvgBrightness(pixelsAsFloats[0]);
}

AvgBrightnessPass::~AvgBrightnessPass() {
}

