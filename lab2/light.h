// Light.h
#ifndef LIGHT_H
#define LIGHT_H

#include <DirectXMath.h>
#include <d3d11.h>

struct PointLight {
    DirectX::XMFLOAT4 Position;   // 16 bytes
    DirectX::XMFLOAT4 Color;      // 16 bytes
    float Intensity;              // 4 bytes
    DirectX::XMFLOAT3 Padding;    // 12 bytes (выравнивание до 48 байт)
};

class LightManager {
public:
    LightManager();
    ~LightManager();

    void Init(ID3D11Device* device);
    void Update(ID3D11DeviceContext* context);
    void SetLightIntensity(int index, float intensity);
    void ToggleLightIntensity(int index);

    ID3D11Buffer* GetLightBuffer() const { return m_pLightBuffer; }

    void Release();

private:
    ID3D11Buffer* m_pLightBuffer;
    PointLight m_lights[3];
    float m_intensityLevels[3];
};

#endif // LIGHT_H