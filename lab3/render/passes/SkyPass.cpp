#include "render/Render.h"
#include "SkyPass.h"

SkyPass::SkyPass() {

}

void SkyPass::init() {
	skyPassVertexShader = render->getShaderManager()->getShader("shaders/SkyPass.hlsl", Shader::Stage::Vertex);
	skyPassPixelShader = render->getShaderManager()->getShader("shaders/SkyPass.hlsl", Shader::Stage::Pixel);

	geometry = render->getGeometryManager()->addSphere(10, 10);


	{
		D3D11_RASTERIZER_DESC desc = {};
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_NONE;
		desc.DepthClipEnable = TRUE;
		desc.DepthBias = -1;

		render->getD3DDevice()->CreateRasterizerState(&desc, &skyRasterizerState);
	}
}

void SkyPass::process() {
	if (!render->getSky()) {
		return;
	}
	render->BeginEvent(L"SkyPass");

	// set target 
	render->getRenderTargetManager()->reset();
	render->getHDRTarget()->setActive();
	render->getDepthBufferTarget()->setActive();
	render->getRenderTargetManager()->apply();

	// set shader
	skyPassVertexShader->setActive();
	skyPassPixelShader->setActive();

	// set samplers
	render->getTextureManager()->setSamplers(skyPassVertexShader);
	render->getTextureManager()->setSamplers(skyPassPixelShader);

	// set per frame + per pass shader data
	render->perFrameCB->setActive(skyPassVertexShader, PER_FRAME_CB_SLOT);
	render->perFrameCB->setActive(skyPassPixelShader, PER_FRAME_CB_SLOT);

	// set prev tex
	render->getSky()->setActive(skyPassPixelShader, TEXTURE_COLOR_SRV_SLOT);

	// set input layout
	render->getGeometryManager()->setLayout(geometry->getVertexType());
	// set geometry
	geometry->setActive();
	render->getD3DDeviceContext()->RSSetState(skyRasterizerState);
	// run shader
	render->getD3DDeviceContext()->DrawIndexed(geometry->getIndexCount(), 0, 0);

	render->EndEvent();
}

SkyPass::~SkyPass() {
	SAFE_RELEASE(skyRasterizerState);
}

