#pragma once
#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX;

class Camera {
public:


    HRESULT Init();
    void Release();
    void Frame();
    void MouseMoved(float dx, float dy, float wheel);
    void SetMouseSensitivity(float sensitivity);
    void SetWheelSensitivity(float sensitivity);
    void KeyPressed(char key);

    void GetBaseViewMatrix(XMMATRIX& viewMatrix) { viewMatrix = m_viewMatrix; };
private:
    XMMATRIX m_viewMatrix;
    XMFLOAT3 m_pointOfInterest;
    float m_distanceToPoint;
    float m_phi;
    float m_theta;
    float m_mouseSensitivity; // Чувствительность мыши
    float m_wheelSensitivity;
    ID3D11DeviceContext* m_pContext = nullptr;
};