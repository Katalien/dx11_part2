#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <directxmath.h>

using namespace DirectX;

struct Vertex {
    XMFLOAT3 position;
    XMFLOAT4 color;
    XMFLOAT2 uv;
    XMFLOAT3 normal;  // Добавляем нормаль
};