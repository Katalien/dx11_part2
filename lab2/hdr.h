#ifndef HDR_H
#define HDR_H

#include <d3d11.h>

class HDR {
public:
    HDR();
    ~HDR();

    bool Init(ID3D11Device* device, UINT width, UINT height);
    void Render(ID3D11DeviceContext* context, ID3D11ShaderResourceView* sourceTexture);
    ID3D11ShaderResourceView* GetHDRTexture() const;
    ID3D11RenderTargetView* GetHDRRTV() const; // Добавленный метод

private:
    ID3D11Texture2D* m_pHDRTexture;
    ID3D11RenderTargetView* m_pHDRRTV;
    ID3D11ShaderResourceView* m_pHDRSRV;
    ID3D11PixelShader* m_pToneMappingPS;
    ID3D11VertexShader* m_pToneMappingVS;

};

#endif // HDR_H
