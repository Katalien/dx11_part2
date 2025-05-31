#include "camera.h"
#include <algorithm>
#include <iostream>

HRESULT Camera::Init() {
    m_pointOfInterest = XMFLOAT3(0.0f, 0.0f, 0.0f);
    m_distanceToPoint = 10.0f;
    m_phi = -XM_PIDIV4;
    m_theta = XM_PIDIV4;

    m_mouseSensitivity = 0.01f; // Чувствительность мыши
    m_wheelSensitivity = 0.1f;  // Чувствительность колеса мыши

    return S_OK;
}

void Camera::Frame() {
    XMFLOAT3 pos = XMFLOAT3(cosf(m_theta) * cosf(m_phi), sinf(m_theta), cosf(m_theta) * sinf(m_phi));
    pos.x = pos.x * m_distanceToPoint + m_pointOfInterest.x;
    pos.y = pos.y * m_distanceToPoint + m_pointOfInterest.y;
    pos.z = pos.z * m_distanceToPoint + m_pointOfInterest.z;
    float upTheta = m_theta + XM_PIDIV2;
    XMFLOAT3 up = XMFLOAT3(cosf(upTheta) * cosf(m_phi), sinf(upTheta), cosf(upTheta) * sinf(m_phi));

    m_viewMatrix = DirectX::XMMatrixLookAtLH(
        DirectX::XMVectorSet(pos.x, pos.y, pos.z, 0.0f),
        DirectX::XMVectorSet(m_pointOfInterest.x, m_pointOfInterest.y, m_pointOfInterest.z, 0.0f),
        DirectX::XMVectorSet(up.x, up.y, up.z, 0.0f)
    );
}

void Camera::MouseMoved(float dx, float dy, float wheel) {
    // Отладочный вывод
    std::cout << "MouseMoved: dx=" << dx << ", dy=" << dy << ", wheel=" << wheel << std::endl;

    m_phi += dx * m_mouseSensitivity;
    m_theta += dy * m_mouseSensitivity;

    m_theta = min(max(m_theta, -XM_PIDIV2), XM_PIDIV2);

    m_distanceToPoint -= wheel * m_wheelSensitivity;
    m_distanceToPoint = max(m_distanceToPoint, 1.0f);
}

void Camera::SetMouseSensitivity(float sensitivity) {
    m_mouseSensitivity = sensitivity;
}

void Camera::SetWheelSensitivity(float sensitivity) {
    m_wheelSensitivity = sensitivity;
}

void Camera::KeyPressed(char key) {
    float moveSpeed = 0.1f;
    switch (key) {
    case 'W':
    case 'w':
        m_pointOfInterest.z += moveSpeed;
        break;
    case 'S':
    case 's':
        m_pointOfInterest.z -= moveSpeed;
        break;
    case 'A':
    case 'a':
        m_pointOfInterest.x -= moveSpeed;
        break;
    case 'D':
    case 'd':
        m_pointOfInterest.x += moveSpeed;
        break;
    case 'Q':
    case 'q':
        m_pointOfInterest.y -= moveSpeed;
        break;
    case 'E':
    case 'e':
        m_pointOfInterest.y += moveSpeed;
        break;
    }
}

/*void Camera::Release() {
    if (m_pContext) {
        m_pContext->Release();
        m_pContext = nullptr;
    }
}*/