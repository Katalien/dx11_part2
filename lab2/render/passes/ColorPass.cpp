#include "render/Render.h"
#include "ColorPass.h"

ColorPass::ColorPass() {

}

void ColorPass::init() {
	vertexShader = render->getShaderManager()->getShader("shaders/ColorPass.hlsl", Shader::Stage::Vertex);
	pixelShader = render->getShaderManager()->getShader("shaders/ColorPass.hlsl", Shader::Stage::Pixel);
	cb = new ConstantBuffer<PerMeshData>(render);
}

void ColorPass::process() {
	render->BeginEvent(L"ColorPass");

	// set output targets
	render->getHDRTarget()->setActive();
	render->getDepthBufferTarget()->setActive();
	render->getRenderTargetManager()->apply();

	// set shaders
	vertexShader->setActive();
	pixelShader->setActive();

	// set per frame + per pass shader data
	render->perFrameCB->setActive(vertexShader, PER_FRAME_CB_SLOT);
	render->perFrameCB->setActive(pixelShader, PER_FRAME_CB_SLOT);

	// set samplers
	render->getTextureManager()->setSamplers(vertexShader);
	render->getTextureManager()->setSamplers(pixelShader);

	auto meshes = render->getMeshManager()->getMeshes();

	render->getLightManager()->setActive(pixelShader, LIGHTS_CB_SLOT);

	for (auto mesh : meshes) {
		// set input layout
		render->getGeometryManager()->setLayout(mesh->getGeometry()->getVertexType());
		// set geometry
		mesh->getGeometry()->setActive();
		// set material
		mesh->getMaterial()->setActive(vertexShader, pixelShader);
		// set transform
		cb->getData().transform = mesh->getTransform();
		cb->setActive(vertexShader, PER_MESH_CB_SLOT);
		cb->setActive(pixelShader, PER_MESH_CB_SLOT);
		// draw
		render->getD3DDeviceContext()->DrawIndexed(mesh->getGeometry()->getIndexCount(), 0, 0);
	}

	render->EndEvent();
}

ColorPass::~ColorPass() {
	delete cb;
}

