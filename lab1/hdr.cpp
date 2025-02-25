#include "hdr.h"
#include <assert.h>
#include <d3d11.h>
#include <d3dcompiler.h>

HDR::HDR() : m_pHDRTexture(nullptr), m_pHDRRTV(nullptr), m_pHDRSRV(nullptr), m_pToneMappingPS(nullptr) {}

HDR::~HDR() {
    if (m_pHDRTexture) m_pHDRTexture->Release();
    if (m_pHDRRTV) m_pHDRRTV->Release();
    if (m_pHDRSRV) m_pHDRSRV->Release();
    if (m_pToneMappingPS) m_pToneMappingPS->Release();
}

bool HDR::Init(ID3D11Device* device, UINT width, UINT height) {
    // Создание HDR-текстуры
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    HRESULT hr = device->CreateTexture2D(&desc, nullptr, &m_pHDRTexture);
    if (FAILED(hr)) return false;

    hr = device->CreateRenderTargetView(m_pHDRTexture, nullptr, &m_pHDRRTV);
    if (FAILED(hr)) return false;

    hr = device->CreateShaderResourceView(m_pHDRTexture, nullptr, &m_pHDRSRV);
    if (FAILED(hr)) return false;

    // Загрузка шейдера Tone Mapping
    ID3D10Blob* pShaderBlob = nullptr;
    hr = D3DCompileFromFile(L"ToneMapping.hlsl", nullptr, nullptr, "main", "ps_5_0", 0, 0, &pShaderBlob, nullptr);
    if (FAILED(hr)) return false;

    hr = device->CreatePixelShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), nullptr, &m_pToneMappingPS);
    pShaderBlob->Release();
    if (FAILED(hr)) return false;

    return true;
}

void HDR::Render(ID3D11DeviceContext* context, ID3D11ShaderResourceView* sourceTexture) {
    // Применение Tone Mapping
    context->PSSetShader(m_pToneMappingPS, nullptr, 0);
    context->PSSetShaderResources(0, 1, &sourceTexture);
    context->Draw(4, 0);
}

ID3D11ShaderResourceView* HDR::GetHDRTexture() const {
    return m_pHDRSRV;
}

ID3D11RenderTargetView* HDR::GetHDRRTV() const {
    return m_pHDRRTV; // Возвращаем RTV для HDR-текстуры
}