#include "engine/Engine.h"
#include "Input.h"

Input::Input(Engine* eng) : engine(eng)
{
    engine->getWin()->setCallback(WM_MOUSEWHEEL, [this](HWND hWnd, UINT msgCode, WPARAM wParam, LPARAM lParam) {
        int newZ = (INT)(SHORT)HIWORD(wParam);
        Mdz = newZ - Mz;
        Mz = newZ;
    });
}

void Input::update() {
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(engine->getWin()->getHWnd(), &pt);
    Mdx = pt.x - Mx;
    Mdy = pt.y - My;
    Mx = pt.x;
    My = pt.y;

    memcpy(oldKeys, keys, KEYBOARD_SIZE);
    GetKeyboardState(keys);
    for (INT i = 0; i < KEYBOARD_SIZE; i++)
    {
        keys[i] >>= 7;
        keysClick[i] = !oldKeys[i] && keys[i];
    }

}