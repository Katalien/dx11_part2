#ifndef HDR_H
#define HDR_H

#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <directxmath.h>
#include "BrightnessCalculator.h"



class HDR {
public:
    HDR();
    ~HDR();

    bool Init(ID3D11Device* device, UINT width, UINT height);
    void Render(ID3D11DeviceContext* context, ID3D11ShaderResourceView* sourceTexture, ID3D11RenderTargetView* targetRTV);
    ID3D11ShaderResourceView* GetHDRTexture() const;
    ID3D11RenderTargetView* GetHDRRTV() const; // Добавленный метод
    float m_deltaTime = 0.0f;
    void SetDeltaTime(float dt) { m_deltaTime = dt; }
    BrightnessCalculator m_brightnessCalc;
    ID3D11Buffer* m_pAdaptationBuffer;
    float m_adaptedLuminance = 1.0f;
    
    struct AdaptationBuffer {
        float DeltaTime;
        float AdaptationSpeed;
        float MinLum;
        float MaxLum;
    };

    bool Resize(ID3D11Device* device, UINT width, UINT height);

private:
    ID3D11Texture2D* m_pHDRTexture;
    ID3D11RenderTargetView* m_pHDRRTV;
    ID3D11ShaderResourceView* m_pHDRSRV;
    ID3D11PixelShader* m_pToneMappingPS;
    ID3D11VertexShader* m_pToneMappingVS;
    ID3D11Buffer* m_pQuadVB = nullptr;        
    ID3D11InputLayout* m_pInputLayout = nullptr; 
    ID3D11SamplerState* m_pSampler = nullptr; 
    UINT m_width;
    UINT m_height;
};

#endif // HDR_H
