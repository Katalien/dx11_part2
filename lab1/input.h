
#define DIRECTINPUT_VERSION 0x0800

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <dinput.h>
#include <directxmath.h>
#include <windows.h>

using namespace DirectX;

class Input {
public:

    HRESULT Init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight);
    void Realese();
    bool Frame();

    XMFLOAT3 IsMouseUsed();
    void Resize(int screenWidth, int screenHeight);

private:
    //bool ReadKeyboard();
    bool ReadMouse();
    void CountDelta(POINT const& cursor);
    XMFLOAT3 PointToXMFLOAT3(POINT const& cursor);

    HWND hwnd;
    POINT curCursor;
    POINT deltaCursor;
    POINT prevCursor;

    IDirectInput8* m_directInput = nullptr;
    IDirectInputDevice8* m_keyboard = nullptr;
    IDirectInputDevice8* m_mouse = nullptr;

    unsigned char m_keyboardState[256];
    DIMOUSESTATE m_mouseState = {};

    int m_screenWidth = 0, m_screenHeight = 0;
};