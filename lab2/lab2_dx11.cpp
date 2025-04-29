#include <windows.h>
#include <directxcolors.h>
#include "framework.h"
#include "lab2_dx11.h"
#include "rendering.h"
#include "resource.h"

#define MAX_LOADSTRING 100
using namespace DirectX;

WCHAR szTitle[MAX_LOADSTRING];
HINSTANCE               g_hInst = nullptr;
UINT                    WindowWidth = 720;
UINT                    WindowHeight = 720;

HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

Renderer* pRenderer = nullptr;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    std::wstring dir;
    dir.resize(MAX_PATH + 1);
    GetCurrentDirectory(MAX_PATH + 1, &dir[0]);
    size_t configPos = dir.find(L"x64");
    if (configPos != std::wstring::npos)
    {
        dir.resize(configPos);
        dir += szTitle;
        SetCurrentDirectory(dir.c_str());
    }

    if (FAILED(InitWindow(hInstance, nCmdShow))) {
        return FALSE;
    }

    MSG msg = { 0 };
    while (WM_QUIT != msg.message) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (pRenderer->Frame()) {
            pRenderer->Render();
        }
    }

    pRenderer->Cleanup();

    return (int)msg.wParam;
}
HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = 0;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = nullptr;
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"WindowClass";
    wcex.hIconSm = 0;

    if (!RegisterClassEx(&wcex)) {
        return E_FAIL;
    }

    g_hInst = hInstance;
    HWND hWnd = CreateWindow(L"WindowClass", L"lab1 (Ольшанский, Комаров, Зинякова)",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr,
        nullptr, hInstance, nullptr);
    if (!hWnd) {
        return E_FAIL;
    }

    
    ShowWindow(hWnd, nCmdShow);
    SetForegroundWindow(hWnd);
    SetFocus(hWnd);
    UpdateWindow(hWnd);

    RECT rc;
    rc.left = 0;
    rc.right = WindowWidth;
    rc.top = 0;
    rc.bottom = WindowHeight;

    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, TRUE);
    MoveWindow(hWnd, 0, 0, rc.right - rc.left, rc.bottom - rc.top, TRUE);

    pRenderer = new Renderer();
    if (!pRenderer->Init(hInstance, hWnd)) {
        delete pRenderer;
        return FALSE;
    }

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_SIZE:
        if (pRenderer != nullptr) {
            RECT rc;
            GetClientRect(hWnd, &rc);
            pRenderer->Resize(rc.right - rc.left, rc.bottom - rc.top);
        }
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    case WM_MOUSEWHEEL: {
        short wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        if (pRenderer) {
            pRenderer->HandleMouseWheel(static_cast<float>(wheelDelta) / WHEEL_DELTA);
        }
        return 0;
    }

    case WM_KEYDOWN: {
        char key = static_cast<char>(wParam); // Получаем код клавиши
        if (pRenderer && pRenderer->GetCamera()) {
            pRenderer->GetCamera()->KeyPressed(key); // Передаём нажатие клавиши в камеру
        }
        break;


         // Получаем код клавиши
        if (pRenderer) {
            pRenderer->HandleKeyPress(key); // Передаём нажатие клавиши в рендерер
        }
        break;
    }


    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}