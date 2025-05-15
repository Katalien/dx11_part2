#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <vector>

using Microsoft::WRL::ComPtr;

struct DownsampleTarget {
    ComPtr<ID3D11Texture2D> texture;
    ComPtr<ID3D11RenderTargetView> rtv;
    ComPtr<ID3D11ShaderResourceView> srv;
    UINT width;
    UINT height;
};

struct DownsampleLevel {
    DownsampleTarget avg;
    DownsampleTarget min;
    DownsampleTarget max;
};

class BrightnessCalculator {
public:
    bool Init(ID3D11Device* device, UINT srcWidth, UINT srcHeight);
    void Calculate(ID3D11DeviceContext* context, ID3D11ShaderResourceView* srcSRV);
    void Release();

    ID3D11ShaderResourceView* GetAvgSRV() const { return m_levels.empty() ? nullptr : m_levels[0].avg.srv.Get(); }
    ID3D11ShaderResourceView* GetMinSRV() const { return m_levels.empty() ? nullptr : m_levels[0].min.srv.Get(); }
    ID3D11ShaderResourceView* GetMaxSRV() const { return m_levels.empty() ? nullptr : m_levels[0].max.srv.Get(); }

private:
    bool CreateDownsampleTarget(ID3D11Device* device, UINT width, UINT height, DownsampleTarget& target);

    std::vector<DownsampleLevel> m_levels;

    ComPtr<ID3D11VertexShader> m_pFullscreenVS;
    ComPtr<ID3D11PixelShader> m_pBrightnessPS;
    ComPtr<ID3D11PixelShader> m_pDownsamplePS;
    ComPtr<ID3D11SamplerState> m_pSamplerLinear;
    ComPtr<ID3D11SamplerState> m_pSamplerMin;
    ComPtr<ID3D11SamplerState> m_pSamplerMax;
};