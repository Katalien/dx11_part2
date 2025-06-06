#include "engine/Engine.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

void UnitBase::setEngine(Engine* eng) {
	engine = eng;
}


Engine::Engine() {
	win = new Win(this);
	timer = new Timer(this);
	input = new Input(this);
	render = new Render(this);
}

void Engine::addUnit(UnitBase* unit) {
	unit->setEngine(this);
	units.push_back(unit);
}

void Engine::init(HINSTANCE hInstance) {
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	 // Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	win->init(hInstance);
	// on every WM_PAINT message call Engine::frame method
	win->setCallback(WM_PAINT, [this](HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam) {
		frame();
		});

	win->setCallback(WM_SIZE, [this](HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam) {
		UINT width = LOWORD(lParam);
		UINT height = HIWORD(lParam);
		resize(width, height);
		});

	RECT rc;
	GetClientRect(win->getHWnd(), &rc);
	render->init(rc.right - rc.left, rc.bottom - rc.top);

	for (size_t i = 0; i < units.size(); i++)
		units[i]->init();
}

void Engine::run() {
	win->run();
}

// update every unit
void Engine::frame() {
	timer->update();
	input->update();

	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	for (auto unit : units) {
		unit->update();
	}

	render->doRender();
//	input->Mdz = 0;
}

void Engine::resize(UINT width, UINT height) {
	render->resize(width, height);
}

Engine::~Engine() {
	for (auto unit : units) {
		delete unit;
	}
	units.clear();
	delete render;
	delete input;
	delete timer;
	delete win;
	ImGui::DestroyContext();
}
