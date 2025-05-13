#include "light.h"
#include <assert.h>
#include <string>

LightManager::LightManager() : m_pLightBuffer(nullptr) {
    
    /*m_lights[0] = { {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, 1.0f, {} };
    m_lights[1] = { {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, 1.0f, {} };
    m_lights[2] = { {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, 1.0f, {} };*/

    float offset = 10.0f; // Расстояние между источниками

    m_lights[0] = {
    {-10.0f, 0.0f, 0.0f, 1.0f},
    {1.0f, 0.0f, 0.0f, 1.0f}, 
    10.0f,
    {}
    };

    m_lights[1] = {
    {0.0f, 0.0f, 0.0f, 1.0f},
    {1.0f, 0.0f, 0.0f, 1.0f},  
        1.0f,
        {}
    };

    m_lights[2] = {
    {0.0f, 3.0f, 0.0f, 1.0f},
    {1.0f, 0.0f, 0.0f, 1.0f},  
        1.0f,
        {}
    };

    m_intensityLevels[0] = 1.0f;
    m_intensityLevels[1] = 1.0f;
    m_intensityLevels[2] = 1.0f;
}

LightManager::~LightManager() {
    if (m_pLightBuffer) m_pLightBuffer->Release();
}

void LightManager::Init(ID3D11Device* device) {
    if (!device) return;

    D3D11_BUFFER_DESC desc = {};
    desc.ByteWidth = sizeof(PointLight) * 3;  
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    HRESULT hr = device->CreateBuffer(&desc, nullptr, &m_pLightBuffer);
    assert(SUCCEEDED(hr));
}

void LightManager::Update(ID3D11DeviceContext* context) {
    D3D11_MAPPED_SUBRESOURCE mapped;
    HRESULT hr = context->Map(m_pLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    assert(SUCCEEDED(hr));

    memcpy(mapped.pData, m_lights, sizeof(PointLight) * 3);
    context->Unmap(m_pLightBuffer, 0);

#ifdef _DEBUG
    
    OutputDebugStringA(("Light 0 Position: (" +
        std::to_string(m_lights[0].Position.x) + ", " +
        std::to_string(m_lights[0].Position.y) + ", " +
        std::to_string(m_lights[0].Position.z) + ")\n").c_str());
#endif

    
    context->PSSetConstantBuffers(2, 1, &m_pLightBuffer);
}

void LightManager::SetLightIntensity(int index, float intensity) {
    if (index >= 0 && index < 3) {
        m_lights[index].Intensity = intensity;
    }
}

void LightManager::ToggleLightIntensity(int index) {
    if (index == 0 ) {
        
        m_intensityLevels[index] *= 10.0f;
        if (m_intensityLevels[index] > 100.0f) {
            m_intensityLevels[index] = 1.0f;
        }

        
        m_lights[index].Intensity = m_intensityLevels[index];
    }
}