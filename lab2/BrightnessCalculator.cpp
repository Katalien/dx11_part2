#include "BrightnessCalculator.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <vector>
#include <algorithm>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace Microsoft::WRL;

bool BrightnessCalculator::Init(ID3D11Device* device, UINT srcWidth, UINT srcHeight) {
    // Создание цепочки текстур для downsampling
    UINT currentWidth = srcWidth;
    UINT currentHeight = srcHeight;

    while (currentWidth > 1 || currentHeight > 1) {
        currentWidth = std::max<UINT>(currentWidth / 2, 1);
        currentHeight = std::max<UINT>(currentHeight / 2, 1);

        DownsampleTarget target;
        if (!CreateDownsampleTarget(device, currentWidth, currentHeight, target)) {
            return false;
        }
        m_downsampleTargets.push_back(target);
    }

    // Компиляция шейдеров
    ComPtr<ID3DBlob> vsBlob;
    ComPtr<ID3DBlob> psBlob;
    ComPtr<ID3DBlob> errorBlob;

    // Вершинный шейдер
    HRESULT hr = D3DCompileFromFile(
        L"FullscreenVS.hlsl",
        nullptr,
        nullptr,
        "main",
        "vs_5_0",
        0,
        0,
        &vsBlob,
        &errorBlob
    );

    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        }
        return false;
    }

    hr = device->CreateVertexShader(
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        nullptr,
        &m_pFullscreenVS
    );

    // Пиксельный шейдер
    hr = D3DCompileFromFile(
        L"DownsamplePS.hlsl",
        nullptr,
        nullptr,
        "main",
        "ps_5_0",
        0,
        0,
        &psBlob,
        &errorBlob
    );

    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        }
        return false;
    }

    hr = device->CreatePixelShader(
        psBlob->GetBufferPointer(),
        psBlob->GetBufferSize(),
        nullptr,
        &m_pDownsamplePS
    );

    // Создание input layout
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    hr = device->CreateInputLayout(
        layout,
        ARRAYSIZE(layout),
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        &m_pInputLayout
    );

    // Создание сэмплера
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    hr = device->CreateSamplerState(&sampDesc, &m_pSampler);

    return SUCCEEDED(hr);
}

void BrightnessCalculator::Calculate(ID3D11DeviceContext* context, ID3D11ShaderResourceView* srcSRV) {
    context->VSSetShader(m_pFullscreenVS.Get(), nullptr, 0);
    context->PSSetShader(m_pDownsamplePS.Get(), nullptr, 0);
    context->IASetInputLayout(m_pInputLayout.Get());
    context->PSSetSamplers(0, 1, m_pSampler.GetAddressOf());

    ID3D11ShaderResourceView* currentSRV = srcSRV;

    for (auto& target : m_downsampleTargets) {
        // Отвязываем предыдущие ресурсы
        ID3D11RenderTargetView* nullRTV = nullptr;
        ID3D11ShaderResourceView* nullSRVs[1] = { nullptr };
        context->OMSetRenderTargets(1, &nullRTV, nullptr);
        context->PSSetShaderResources(0, 1, nullSRVs);

        // Очищаем цель рендеринга
        const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
        context->ClearRenderTargetView(target.rtv.Get(), clearColor);

        // Устанавливаем новые ресурсы
        context->OMSetRenderTargets(1, target.rtv.GetAddressOf(), nullptr);
        context->PSSetShaderResources(0, 1, &currentSRV);

        // Настройка вьюпорта
        D3D11_VIEWPORT viewport = {};
        viewport.Width = static_cast<float>(target.width);
        viewport.Height = static_cast<float>(target.height);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        context->RSSetViewports(1, &viewport);

        // Отрисовка полноэкранного треугольника
        context->Draw(3, 0);

        // Переключаемся на результат текущего этапа
        currentSRV = target.srv.Get();
    }

    // Финализация - отвязываем все ресурсы
    ID3D11RenderTargetView* finalNullRTV = nullptr;
    ID3D11ShaderResourceView* finalNullSRVs[1] = { nullptr };
    context->OMSetRenderTargets(1, &finalNullRTV, nullptr);
    context->PSSetShaderResources(0, 1, finalNullSRVs);
}

ID3D11ShaderResourceView* BrightnessCalculator::GetResultSRV() const {
    if (m_downsampleTargets.empty()) return nullptr;
    return m_downsampleTargets.back().srv.Get();
}

bool BrightnessCalculator::CreateDownsampleTarget(
    ID3D11Device* device,
    UINT width,
    UINT height,
    DownsampleTarget& target
) {
    // Создание текстуры
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

    // Создание RTV
    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.Format = texDesc.Format;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    hr = device->CreateRenderTargetView(
        target.texture.Get(),
        &rtvDesc,
        &target.rtv
    );
    if (FAILED(hr)) return false;

    // Создание SRV
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = texDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    hr = device->CreateShaderResourceView(
        target.texture.Get(),
        &srvDesc,
        &target.srv
    );

    target.width = width;
    target.height = height;

    return SUCCEEDED(hr);
}

void BrightnessCalculator::Release() {
    for (auto& target : m_downsampleTargets) {
        target.texture.Reset();
        target.rtv.Reset();
        target.srv.Reset();
    }
    m_downsampleTargets.clear();

    m_pFullscreenVS.Reset();
    m_pDownsamplePS.Reset();
    m_pInputLayout.Reset();
    m_pSampler.Reset();
}