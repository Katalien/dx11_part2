#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <directxmath.h>

using namespace DirectX;

struct Vertex {
    float x, y, z;
    COLORREF color;
};
