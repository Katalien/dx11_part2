#include "input.h"
#include <fstream>

HRESULT Input::Init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight) {
    HRESULT hr = S_OK;
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;

    POINT point;
    float x = 0.0f;
    float y = 0.0f;

    deltaCursor.x = static_cast<LONG>(x);
    deltaCursor.y = static_cast<LONG>(y);
    prevCursor.x = static_cast<LONG>(x);
    prevCursor.y = static_cast<LONG>(y);
    curCursor.x = static_cast<LONG>(x);
    curCursor.y = static_cast<LONG>(y);
    return hr;

}

void Input::Resize(int screenWidth, int screenHeight) {
    m_screenWidth = screenWidth;
    m_screenHeight = screenHeight;
}

bool Input::Frame() {
    return ReadMouse();
}


bool Input::ReadMouse() {
    POINT cursor;
    
    if (GetCursorPos(&cursor) ) {
        ScreenToClient(hwnd, &cursor);
        std::ofstream outputFile("check_coords.txt");
        outputFile << "read mouse" << std::endl;
        if (cursor.x == curCursor.x && cursor.y == curCursor.y) {
            return true;
        }
        
        CountDelta(cursor);
        prevCursor = curCursor;
        curCursor = cursor;
        return true;
    }
    else {
        return false;
    }

}


XMFLOAT3 Input::IsMouseUsed() {
    XMFLOAT3 mouseDelta = XMFLOAT3(
        (curCursor.x - prevCursor.x) / 2,
        (curCursor.y - prevCursor.y) / 2,
        0.0f);

    BYTE keyState[256];
    GetKeyboardState(keyState);

    if (keyState[VK_LBUTTON] & 0x80)
        return mouseDelta;
    return XMFLOAT3(0.0f, 0.0f, 0.0f);
};



void Input::CountDelta(POINT const& cursor) {
    float deltaX = cursor.x - curCursor.x;
    float deltaY = cursor.y - curCursor.y;
    POINT delta = { deltaX, deltaY };
    deltaCursor = delta;
}

XMFLOAT3 Input::PointToXMFLOAT3(POINT const& cursor) {
    return { float(cursor.x), float(cursor.y), 0.0f };
}