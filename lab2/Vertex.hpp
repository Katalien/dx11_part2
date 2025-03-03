#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <directxmath.h>

using namespace DirectX;

struct Vertex {
    XMFLOAT3 position; // 12 байт
    XMFLOAT4 color;    // 16 байт
//    XMFLOAT2 uv;       // 8 байт (TEXCOORD)
};