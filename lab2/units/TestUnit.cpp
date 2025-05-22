#include "TestUnit.h"

TestUnit::TestUnit() {

}

void TestUnit::init() {
	engine->getWin()->setCallback(WM_CHAR, [this](HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam) {
		const char source[2] = { static_cast<char>(wParam), 0};
		std::wstring dest(2, L'#');
		size_t size;
		mbstowcs_s(&size, &dest[0], 2, source, 2);
		MessageBox(NULL, dest.c_str(), L"Button pressed", MB_OK | MB_ICONERROR);
		printf("Button pressed %s", dest);
		});
}

void TestUnit::update() {
	DirectX::XMFLOAT3 a(1.0f, 0.0f, 0.0f);
	DirectX::XMFLOAT3 b(0.0f, 1.0f, 0.0f);
	DirectX::XMFLOAT3 c;
	DirectX::XMStoreFloat3(&c, DirectX::XMVector3Cross(
		DirectX::XMLoadFloat3(&a),
		DirectX::XMLoadFloat3(&b)));
}

TestUnit::~TestUnit() {

}
