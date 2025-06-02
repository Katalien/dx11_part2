#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <directxmath.h>

using namespace DirectX;

struct Vertex {
    XMFLOAT3 position; // ������� (x, y, z)
    XMFLOAT4 color;    // ���� (r, g, b, a)
};