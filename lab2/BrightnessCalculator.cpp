#include "BrightnessCalculator.h"
#include <d3dcompiler.h>
#include <algorithm>

#pragma comment(lib, "d3dcompiler.lib")

bool BrightnessCalculator::Init(ID3D11Device* device, UINT srcWidth, UINT srcHeight) {
    HRESULT hr;

    // cepochka
    UINT width = srcWidth;
    UINT height = srcHeight;

    while (width > 1 || height > 1) {
        width = std::max<UINT>(width / 2, 1);
        height = std::max<UINT>(height / 2, 1);

        DownsampleLevel level;
        if (!CreateDownsampleTarget(device, width, height, level.avg)) return false;
        if (!CreateDownsampleTarget(device, width, height, level.min)) return false;
        if (!CreateDownsampleTarget(device, width, height, level.max)) return false;

        m_levels.push_back(level);
    }

    // shaders
    ComPtr<ID3DBlob> vsBlob, psBlob, errorBlob;

    // Vertex shader
    hr = D3DCompileFromFile(L"FullscreenVS.hlsl", nullptr, nullptr, "main",
        "vs_5_0", 0, 0, &vsBlob, &errorBlob);
    if (FAILED(hr)) return false;

    hr = device->CreateVertexShader(vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        nullptr, &m_pFullscreenVS);
    if (FAILED(hr)) return false;

    // Pixel shader 1st level
    hr = D3DCompileFromFile(L"BrightnessPS.hlsl", nullptr, nullptr, "main",
        "ps_5_0", 0, 0, &psBlob, &errorBlob);
    if (FAILED(hr)) return false;

    hr = device->CreatePixelShader(psBlob->GetBufferPointer(),
        psBlob->GetBufferSize(),
        nullptr, &m_pBrightnessPS);
    if (FAILED(hr)) return false;

    // Pixel shader downsample
    hr = D3DCompileFromFile(L"DownsamplePS.hlsl", nullptr, nullptr, "main",
        "ps_5_0", 0, 0, &psBlob, &errorBlob);
    if (FAILED(hr)) return false;

    hr = device->CreatePixelShader(psBlob->GetBufferPointer(),
        psBlob->GetBufferSize(),
        nullptr, &m_pDownsamplePS);
    if (FAILED(hr)) return false;

    // sample
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

    hr = device->CreateSamplerState(&sampDesc, &m_pSamplerLinear);
    if (FAILED(hr)) return false;

    // sample min
    sampDesc.Filter = D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR;
    hr = device->CreateSamplerState(&sampDesc, &m_pSamplerMin);
    if (FAILED(hr)) return false;

    // sample max
    sampDesc.Filter = D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR;
    hr = device->CreateSamplerState(&sampDesc, &m_pSamplerMax);

    return SUCCEEDED(hr);
}

bool BrightnessCalculator::CreateDownsampleTarget(ID3D11Device* device, UINT width, UINT height, DownsampleTarget& target) {
    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R32_FLOAT;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    HRESULT hr = device->CreateTexture2D(&texDesc, nullptr, &target.texture);
    if (FAILED(hr)) return false;

    // RTV
    hr = device->CreateRenderTargetView(target.texture.Get(), nullptr, &target.rtv);
    if (FAILED(hr)) return false;

    // SRV
    hr = device->CreateShaderResourceView(target.texture.Get(), nullptr, &target.srv);
    if (FAILED(hr)) return false;

    target.width = width;
    target.height = height;

    return true;
}

void BrightnessCalculator::Calculate(ID3D11DeviceContext* context, ID3D11ShaderResourceView* srcSRV) {
    context->VSSetShader(m_pFullscreenVS.Get(), nullptr, 0);
    context->IASetInputLayout(nullptr);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // 
    ID3D11SamplerState* samplers[] = { m_pSamplerLinear.Get(),
                                      m_pSamplerMin.Get(),
                                      m_pSamplerMax.Get() };
    context->PSSetSamplers(0, 3, samplers);

    // go by levels
    for (int i = m_levels.size() - 1; i >= 0; --i) {
        const auto& level = m_levels[i];

        // rtv
        ID3D11RenderTargetView* rtvs[] = { level.avg.rtv.Get(),
                                          level.min.rtv.Get(),
                                          level.max.rtv.Get() };
        context->OMSetRenderTargets(3, rtvs, nullptr);

        
        const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
        context->ClearRenderTargetView(level.avg.rtv.Get(), clearColor);
        context->ClearRenderTargetView(level.min.rtv.Get(), clearColor);
        context->ClearRenderTargetView(level.max.rtv.Get(), clearColor);

        
        D3D11_VIEWPORT viewport = {};
        viewport.Width = static_cast<float>(level.avg.width);
        viewport.Height = static_cast<float>(level.avg.height);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        context->RSSetViewports(1, &viewport);

        
        if (i == m_levels.size() - 1) {
            // 1st level
            context->PSSetShader(m_pBrightnessPS.Get(), nullptr, 0);
            ID3D11ShaderResourceView* srvs[] = { srcSRV, srcSRV, srcSRV };
            context->PSSetShaderResources(0, 3, srvs);
        }
        else {
            // != 1st level
            context->PSSetShader(m_pDownsamplePS.Get(), nullptr, 0);
            ID3D11ShaderResourceView* srvs[] = {
                m_levels[i + 1].avg.srv.Get(),
                m_levels[i + 1].min.srv.Get(),
                m_levels[i + 1].max.srv.Get()
            };
            context->PSSetShaderResources(0, 3, srvs);
        }

        
        context->Draw(4, 0);
    }

   
    ID3D11RenderTargetView* nullRTVs[3] = { nullptr, nullptr, nullptr };
    context->OMSetRenderTargets(3, nullRTVs, nullptr);

    ID3D11ShaderResourceView* nullSRVs[3] = { nullptr, nullptr, nullptr };
    context->PSSetShaderResources(0, 3, nullSRVs);
}

void BrightnessCalculator::Release() {
    m_levels.clear();
    m_pFullscreenVS.Reset();
    m_pBrightnessPS.Reset();
    m_pDownsamplePS.Reset();
    m_pSamplerLinear.Reset();
    m_pSamplerMin.Reset();
    m_pSamplerMax.Reset();
}