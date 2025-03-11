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

//struct SceneConstants {
//    XMFLOAT4 avgLuminance;
//    XMFLOAT3 padding;
//};

//struct TransformBuffer {
//    XMMATRIX worldMatrix;         // 64 bytes
//    XMMATRIX viewProjectionMatrix; // 64 bytes
//    XMMATRIX worldViewProj;       // 64 bytes (если нужно)
//}; // Всего 192 bytes
#pragma pack(push, 16)
struct TransformBuffer {
    XMMATRIX worldViewProj;  // Матрица вида-проекции (view * projection)
    XMMATRIX worldMatrix;    // Мировая матрица объекта
};
#pragma pack(pop)

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
    void HandleKeyPress(char key);


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

    ID3D11Buffer* m_pTransformBuffer;

    ID3D11RasterizerState* m_pRasterizerState = nullptr;

    Camera* m_pCamera = nullptr;
    Input* m_pInput = nullptr;

    LightManager m_lightManager;
    HDR m_hdr;
};