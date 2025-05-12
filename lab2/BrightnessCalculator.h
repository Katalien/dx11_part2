#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <vector>

class BrightnessCalculator {
public:
    struct DownsampleTarget {
        Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
        UINT width = 0;
        UINT height = 0;
    };

    bool Init(ID3D11Device* device, UINT srcWidth, UINT srcHeight);
    void Calculate(ID3D11DeviceContext* context, ID3D11ShaderResourceView* srcSRV);
    ID3D11ShaderResourceView* GetResultSRV() const;
    void Release();

private:
    std::vector<DownsampleTarget> m_downsampleTargets;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_pFullscreenVS;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pDownsamplePS;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_pInputLayout;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSampler;

    bool CreateDownsampleTarget(
        ID3D11Device* device,
        UINT width,
        UINT height,
        DownsampleTarget& target
    );
};