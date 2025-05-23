#include "engine/Engine.h"

Win::Win(Engine *eng): engine(eng) {}

bool Win::init(HINSTANCE hInst)
{
	hInstance = hInst;
    // register window class
    {
        WNDCLASSEX wcex;

        wcex.cbSize = sizeof(WNDCLASSEX);

        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = sizeof(Win*);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
        wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcex.hIconSm = (HICON)LoadImage(NULL, IDI_APPLICATION, IMAGE_ICON,
            GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
        wcex.hIcon = (HICON)LoadImage(NULL, IDI_APPLICATION, IMAGE_ICON,
            GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), 0);
        wcex.lpszMenuName = nullptr;
        wcex.hInstance = hInstance;
        wcex.lpszClassName = className.c_str();
        wcex.lpfnWndProc = WndProc;

        if (!RegisterClassEx(&wcex))
        {
            MessageBox(NULL, L"Cannot register window", L"Window registration error", MB_OK | MB_ICONERROR);
            return false;
        }
    }

    // window creation
    {
        hWnd = CreateWindow(
            className.c_str(), L"GoodProj",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT,
            CW_USEDEFAULT, CW_USEDEFAULT,
            NULL, NULL, hInstance,
            reinterpret_cast<void*>(this));

        if (hWnd == nullptr) {
            MessageBox(NULL, L"Cannot create window", L"Window creation error", MB_OK | MB_ICONERROR);
            return false;
        }
    }

    ShowWindow(hWnd, SW_SHOWNORMAL);
    UpdateWindow(hWnd);
    return true;
}

LRESULT CALLBACK Win::WndProc(HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam)
{
    if (msgCode == WM_CREATE)
    {
        SetWindowLongPtr(hWnd, 0, reinterpret_cast<LONG_PTR>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams));
        return 0;
    }

    Win* win;
    win = reinterpret_cast<Win*>(GetWindowLongPtr(hWnd, 0));

    if (win == nullptr) {
        return DefWindowProc(hWnd, msgCode, wParam, lParam);
    }

    switch (msgCode)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        break;
    }

    auto callbacks = win->callbacks.find(msgCode);
    if (callbacks != win->callbacks.end()) {
        for (auto& callback : callbacks->second) {
            callback(hWnd, msgCode, wParam, lParam);
        }
    }

    return DefWindowProc(hWnd, msgCode, wParam, lParam);
}

void Win::run()
{
    MSG msg;

    while (true) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        // if no messages -> draw a frame
        {
            auto callbacksIter = callbacks.find(WM_PAINT);
            if (callbacksIter != callbacks.end()) {
                for (auto& callback : callbacksIter->second) {
                    callback(hWnd, WM_PAINT, 0, 0);
                }
            }
        }
    }
}

void Win::setCallback(UINT msg, WindowMsgCallback callback) {
    if (callbacks.find(msg) == callbacks.end()) {
        callbacks[msg] = std::vector<WindowMsgCallback>();
    }

    callbacks[msg].push_back(callback);
}

Win::~Win()
{

}
