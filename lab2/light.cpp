#include "light.h"
#include <assert.h>
#include <string>
#define SAFE_RELEASE(a) if (a != NULL) { a->Release(); a = NULL; }


LightManager::LightManager() : m_pLightBuffer(nullptr) {

    /*m_lights[0] = { {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, 1.0f, {} };
    m_lights[1] = { {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}, 1.0f, {} };
    m_lights[2] = { {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, 1.0f, {} };*/

    float offset = 10.0f; // Расстояние между источниками

    //m_lights[0] = {
    //{-2.0f, 0.0f, 0.0f, 1.0f},
    //{10.0f, 0.0f, 0.0f, 1.0f}, 
    //10.0f,
    //{}
    //};

    //m_lights[1] = {
    //{2.0f, 0.0f, 0.0f, 1.0f},
    //{10.0f, 0.0f, 0.0f, 1.0f},  
    //    1.0f,
    //    {}
    //};

    ///*m_lights[2] = {
    //{0.0f, 3.0f, 0.0f, 1.0f},
    //{10.0f, 0.0f, 0.0f, 1.0f},  
    //    1.0f,
    //    {}
    //};*/

    m_lights[0] = { {2.7f, 0.0f, 0.0f, 1.0f}, {25.0f, 0.0f, 0.0f, 1.0f}, 1.0f, {} };
    m_lights[1] = { {0.0f, 2.7f, 0.0f, 1.0f}, {15.0f, 0.0f, 0.0f, 1.0f}, 1.0f, {} };
    m_lights[2] = { {0.0f, 0.0f, 2.7f, 1.0f}, {10.0f, 0.0f, 0.0f, 1.0f}, 1.0f, {} };

    m_intensityLevels[0] = 1.0f;
    m_intensityLevels[1] = 1.0f;
    m_intensityLevels[2] = 1.0f;
}

LightManager::~LightManager() {
    Release();
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

    //OutputDebugStringA(("Light 0 Position: (" +
      //  std::to_string(m_lights[0].Position.x) + ", " +
      //  std::to_string(m_lights[0].Position.y) + ", " +
      //  std::to_string(m_lights[0].Position.z) + ")\n").c_str());
#endif


    context->PSSetConstantBuffers(2, 1, &m_pLightBuffer);
}

void LightManager::SetLightIntensity(int index, float intensity) {
    if (index >= 0 && index < 3) {
        m_lights[index].Intensity = intensity;
    }
}

void LightManager::ToggleLightIntensity(int index) {
    if (index < 0 || index >= 3) return;


    static const float levels[] = { 1.0f, 10.0f, 100.0f };
    static int currentLevel = 0;

    currentLevel = (currentLevel + 1) % 3;
    m_lights[index].Intensity = levels[currentLevel];

    //#ifdef _DEBUG
    //    std::cout << "Light " << index << " intensity: " << m_lights[index].Intensity << std::endl;
    //#endif
}

void LightManager::Release() {
    SAFE_RELEASE(m_pLightBuffer);
}