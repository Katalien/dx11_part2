#pragma once
#include "Dev.h"
using WindowMsgCallback = std::function<void(HWND, UINT, WPARAM, LPARAM)>;

class Win {
private:
	Engine* engine;
	// handle of current process instance
	HINSTANCE hInstance;
	// handle of window (we have one window)
	HWND hWnd;

	std::wstring className = L"GoodProj";

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam);

	std::map<UINT, std::vector<WindowMsgCallback>> callbacks;
public:
	Win(Engine *eng);
	
	// creates and opens window
	bool init(HINSTANCE hInstance);
	
	void setCallback(UINT msg, WindowMsgCallback callback);

	// runs message loop
	void run();
	
	HWND getHWnd() { return hWnd; }

	~Win();
};
