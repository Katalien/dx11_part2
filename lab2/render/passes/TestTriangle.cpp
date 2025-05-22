#include "render/Render.h"
#include "TestTriangle.h"

TestTriangle::TestTriangle() {

}

void TestTriangle::init() {
	vertexShader = render->getShaderManager()->getShader("shaders/TestTriangle.hlsl", Shader::Stage::Vertex);
	pixelShader = render->getShaderManager()->getShader("shaders/TestTriangle.hlsl", Shader::Stage::Pixel);
	constantBuffer = new ConstantBuffer<TriangleData>(render);
}
void TestTriangle::process() {
	vertexShader->setActive();
	pixelShader->setActive();
	auto &cb = constantBuffer->getData();

	cb.color1 = { 1.0f * rand() / RAND_MAX, 0, 0 };
	cb.color2 = { 0, 1.0f * rand() / RAND_MAX, 0 };
	cb.color3 = { 0, 0, 1.0f * rand() / RAND_MAX };
	render->perFrameCB->setActive(vertexShader, 1);
	render->perFrameCB->setActive(pixelShader, 1);
	constantBuffer->setActive(vertexShader, 0);
	constantBuffer->setActive(pixelShader, 0);
	render->getD3DDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	render->getD3DDeviceContext()->Draw(3, 0);
}

TestTriangle::~TestTriangle() {
	delete constantBuffer;
}

