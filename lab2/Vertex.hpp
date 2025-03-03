#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <directxmath.h>

using namespace DirectX;

struct Vertex {
    XMFLOAT3 position; // 12 ����
    XMFLOAT4 color;    // 16 ����
//    XMFLOAT2 uv;       // 8 ���� (TEXCOORD)
};