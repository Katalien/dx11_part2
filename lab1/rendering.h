#pragma once
#pragma comment(lib,"d3dcompiler")
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <directxmath.h>
#include <string>
#include "Vertex.hpp"
#include "camera.h"
#include "input.h"
#include "hdr.h"
#include "light.h"

using namespace DirectX;

struct WorldMatrixBuffer {
    XMMATRIX mWorldMatrix;
};

struct SceneMatrixBuffer {
    XMMATRIX mViewProjectionMatrix;
};

class Renderer {
public:

    Camera* GetCamera() const {
        return m_pCamera;
    }

    bool Init(HINSTANCE hInstance, HWND hWnd);
    bool Frame();
    bool Resize(UINT width, UINT height);
    void Cleanup();
    bool Render();

    void HandleMouseWheel(float delta) {
        if (m_pCamera) {
            m_pCamera->MouseMoved(0.0f, 0.0f, delta); // Передаём только прокрутку
        }
    }

private:


    HRESULT InitScene();
    HRESULT SetupBackBuffer();

    void ManageUserInput();
    
    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;
    IDXGISwapChain* m_pSwapChain = nullptr;

    ID3D11RenderTargetView* m_pBackBufferRTV = nullptr;
    UINT m_width = 0;
    UINT m_height = 0;
    ID3D11Buffer* m_pVertexBuffer = nullptr;
    ID3D11Buffer* m_pIndexBuffer = nullptr;
    ID3D11InputLayout* m_pInputLayout = nullptr;
    ID3D11VertexShader* m_pVertexShader = nullptr;
    ID3D11PixelShader* m_pPixelShader = nullptr;

    ID3D11Buffer* m_pWorldMatrixBuffer = nullptr;
    ID3D11Buffer* m_pSceneMatrixBuffer = nullptr;
    ID3D11RasterizerState* m_pRasterizerState = nullptr;

    Camera* m_pCamera = nullptr;
    Input* m_pInput = nullptr;

    LightManager m_lightManager;
    HDR m_hdr;
};