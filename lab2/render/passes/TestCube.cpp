#include "render/Render.h"
#include "TestCube.h"

TestCube::TestCube() {

}

void TestCube::init() {
	vertexShader = render->getShaderManager()->getShader("shaders/TestCube.hlsl", Shader::Stage::Vertex);
	pixelShader = render->getShaderManager()->getShader("shaders/TestCube.hlsl", Shader::Stage::Pixel);
	cubeCB = new ConstantBuffer<PerMeshData>(render);
	cubeGeometry = render->getGeometryManager()->addCube();

	DirectX::XMStoreFloat4x4(&cubeCB->getData().transform, DirectX::XMMatrixRotationX(45));
}
void TestCube::process() {
	render->BeginEvent(L"TestCube");
	// update CB
	auto& cb = cubeCB->getData();
	DirectX::XMMATRIX matrix = DirectX::XMLoadFloat4x4(&cb.transform);
	matrix = DirectX::XMMatrixMultiply(matrix, DirectX::XMMatrixRotationY(0.001));
	DirectX::XMStoreFloat4x4(&cb.transform, matrix);

	// set shaders
	vertexShader->setActive();
	pixelShader->setActive();
	// set per frame + per pass shader data
	render->perFrameCB->setActive(vertexShader, PER_FRAME_CB_SLOT);
	render->perFrameCB->setActive(pixelShader, PER_FRAME_CB_SLOT);

	// set input layout
	render->getGeometryManager()->setLayout(cubeGeometry->getVertexType());
	// set geometry
	cubeGeometry->setActive();
	// set geometry cb
	cubeCB->setActive(vertexShader, PER_MESH_CB_SLOT);
	cubeCB->setActive(pixelShader, PER_MESH_CB_SLOT);
	render->getD3DDeviceContext()->DrawIndexed(cubeGeometry->getIndexCount(), 0, 0);
	render->EndEvent();
}

TestCube::~TestCube() {
	delete cubeCB;
}

