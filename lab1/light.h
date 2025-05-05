#ifndef LIGHT_H
#define LIGHT_H

#include <DirectXMath.h>
#include <d3d11.h>

struct PointLight {
    DirectX::XMFLOAT4 Position; // Позиция (16 байт)
    DirectX::XMFLOAT4 Color;    // Цвет (16 байт)
    float Intensity;
};

class LightManager {
public:
    LightManager();
    ~LightManager();

    void Init(ID3D11Device* device);
    void Update(ID3D11DeviceContext* context);
    void SetLightIntensity(int index, float intensity);

private:
    ID3D11Buffer* m_pLightBuffer;
    PointLight m_lights[3]; // Три источника света
};

#endif // LIGHT_H
