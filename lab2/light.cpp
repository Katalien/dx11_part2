#include "light.h"
#include <assert.h>

#include <iostream>
#include <string>

LightManager::LightManager() {
    // Инициализация источников света
    m_lights[0] = { {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, 1.0f };
    m_lights[1] = { {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, 1.0f };
    m_lights[2] = { {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, 1.0f };

    // Инициализация уровней яркости
    m_intensityLevels[0] = 1.0f;
    m_intensityLevels[1] = 1.0f;
    m_intensityLevels[2] = 1.0f;
}

LightManager::~LightManager() {
    if (m_pLightBuffer) m_pLightBuffer->Release();
}

void LightManager::Init(ID3D11Device* device) {
    if (!device) {
        OutputDebugStringA("Error: Device is null.\n");
        return;
    }

    // Вычисление размера буфера с выравниванием
    UINT bufferSize = sizeof(PointLight) * 3; // Размер для 3 источников света
    bufferSize = (bufferSize + 15) & ~15; // Выравнивание до ближайшего числа, кратного 16

    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = bufferSize; // Используем выровненный размер
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    HRESULT hr = device->CreateBuffer(&desc, nullptr, &m_pLightBuffer);

    if (FAILED(hr)) {
        OutputDebugStringA("Failed to create buffer. HRESULT: ");
        OutputDebugStringA(std::to_string(hr).c_str());
        return;
    }

    assert(SUCCEEDED(hr));
}

void LightManager::Update(ID3D11DeviceContext* context) {
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = context->Map(m_pLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    assert(SUCCEEDED(hr));

    memcpy(mappedResource.pData, m_lights, sizeof(PointLight) * 3);
    context->Unmap(m_pLightBuffer, 0);

    context->PSSetConstantBuffers(1, 1, &m_pLightBuffer);
}

void LightManager::SetLightIntensity(int index, float intensity) {
    if (index >= 0 && index < 3) {
        m_lights[index].Intensity = intensity;
    }
}

void LightManager::ToggleLightIntensity(int index) {
    if (index >= 0 && index < 3) {
        // Переключение между уровнями яркости: 1, 10, 100
        if (m_intensityLevels[index] == 1.0f) {
            m_intensityLevels[index] = 10.0f;
        }
        else if (m_intensityLevels[index] == 10.0f) {
            m_intensityLevels[index] = 100.0f;
        }
        else {
            m_intensityLevels[index] = 1.0f;
        }

        // Применение новой яркости к источнику света
        m_lights[index].Intensity = m_intensityLevels[index];
    }
}