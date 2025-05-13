#include "hdr.h"
#include <assert.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi.h>
#include <directxmath.h>
#define SAFE_RELEASE(a) if (a != NULL) { a->Release(); a = NULL; }
using namespace DirectX;

HDR::HDR() : m_pHDRTexture(nullptr), m_pHDRRTV(nullptr), m_pHDRSRV(nullptr), m_pToneMappingPS(nullptr) {}

HDR::~HDR() {
    if (m_pHDRTexture) m_pHDRTexture->Release();
    if (m_pHDRRTV) m_pHDRRTV->Release();
    if (m_pHDRSRV) m_pHDRSRV->Release();
    if (m_pToneMappingPS) m_pToneMappingPS->Release();
    if (m_pToneMappingVS) m_pToneMappingVS->Release();
}

struct SimpleVertex {
    XMFLOAT3 Pos;
    XMFLOAT2 Tex;
};

struct AdaptationBuffer {
    float DeltaTime;
    float AdaptationSpeed;
    XMFLOAT2 Padding;
};

bool HDR::Init(ID3D11Device* device, UINT width, UINT height) {
    HRESULT hr;

    
    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DEFAULT;
    textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    m_width = width;
    m_height = height;

    hr = device->CreateTexture2D(&textureDesc, nullptr, &m_pHDRTexture);
    if (FAILED(hr)) return false;

    
    hr = device->CreateRenderTargetView(m_pHDRTexture, nullptr, &m_pHDRRTV);
    if (FAILED(hr)) return false;

    
    hr = device->CreateShaderResourceView(m_pHDRTexture, nullptr, &m_pHDRSRV);
    if (FAILED(hr)) return false;

    
    if (!m_brightnessCalc.Init(device, width, height)) {
        return false;
    }

    
    SimpleVertex vertices[] = {
        { XMFLOAT3(-1.0f, 1.0f, 0.0f),  XMFLOAT2(0.0f, 0.0f) },
        { XMFLOAT3(1.0f, 1.0f, 0.0f),   XMFLOAT2(1.0f, 0.0f) },
        { XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
        { XMFLOAT3(1.0f, -1.0f, 0.0f),  XMFLOAT2(1.0f, 1.0f) }
    };

    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.Usage = D3D11_USAGE_IMMUTABLE;
    vbDesc.ByteWidth = sizeof(vertices);
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA initData = { vertices };
    hr = device->CreateBuffer(&vbDesc, &initData, &m_pQuadVB);
    if (FAILED(hr)) return false;

    
    ID3DBlob* vsBlob = nullptr;
    hr = D3DCompileFromFile(L"ToneMappingVS.hlsl", nullptr, nullptr,
        "main", "vs_5_0", 0, 0, &vsBlob, nullptr);
    if (SUCCEEDED(hr)) {
        hr = device->CreateVertexShader(vsBlob->GetBufferPointer(),
            vsBlob->GetBufferSize(),
            nullptr,
            &m_pToneMappingVS);
    }
    if (FAILED(hr)) return false;

    ID3DBlob* psBlob = nullptr;
    hr = D3DCompileFromFile(L"ToneMappingPS.hlsl", nullptr, nullptr,
        "main", "ps_5_0", 0, 0, &psBlob, nullptr);
    if (SUCCEEDED(hr)) {
        hr = device->CreatePixelShader(psBlob->GetBufferPointer(),
            psBlob->GetBufferSize(),
            nullptr,
            &m_pToneMappingPS);
    }
    if (FAILED(hr)) return false;

    
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    hr = device->CreateInputLayout(layout, 2,
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        &m_pInputLayout);
    if (FAILED(hr)) return false;

    // sample
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

    hr = device->CreateSamplerState(&sampDesc, &m_pSampler);
    if (FAILED(hr)) return false;

    // buffffer adaptation
    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.ByteWidth = sizeof(AdaptationBuffer);
    cbDesc.Usage = D3D11_USAGE_DYNAMIC;
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    hr = device->CreateBuffer(&cbDesc, nullptr, &m_pAdaptationBuffer);
    if (FAILED(hr)) return false;

    SAFE_RELEASE(vsBlob);
    SAFE_RELEASE(psBlob);

    return true;
}

void HDR::Render(
    ID3D11DeviceContext* context,
    ID3D11ShaderResourceView* sourceTexture,
    ID3D11RenderTargetView* targetRTV
) {
    // avg yarkost
    m_brightnessCalc.Calculate(context, sourceTexture);

    
    const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    context->ClearRenderTargetView(targetRTV, clearColor);
    context->OMSetRenderTargets(1, &targetRTV, nullptr);

    
    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(m_width);
    viewport.Height = static_cast<float>(m_height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    context->RSSetViewports(1, &viewport);

    // Update buffer adaptation
    AdaptationBuffer adaptData;
    /*float safeDelta = (m_deltaTime > 0.1f) ? 0.1f : m_deltaTime;
    adaptData.DeltaTime = safeDelta;*/
    adaptData.DeltaTime = m_deltaTime;
    adaptData.AdaptationSpeed = 0.1f;

    D3D11_MAPPED_SUBRESOURCE mapped;
    context->Map(m_pAdaptationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    memcpy(mapped.pData, &adaptData, sizeof(AdaptationBuffer));
    context->Unmap(m_pAdaptationBuffer, 0);

    // Shaders shaders shares
    context->VSSetShader(m_pToneMappingVS, nullptr, 0);
    context->PSSetShader(m_pToneMappingPS, nullptr, 0);

    // privyaz resorces
    ID3D11ShaderResourceView* srvs[] = { sourceTexture, m_brightnessCalc.GetResultSRV() };
    context->PSSetShaderResources(0, 2, srvs);
    context->PSSetSamplers(0, 1, &m_pSampler);
    context->PSSetConstantBuffers(0, 1, &m_pAdaptationBuffer);

    // vetrex bufffffferrrrrr
    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &m_pQuadVB, &stride, &offset);
    context->IASetInputLayout(m_pInputLayout);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // draw yo
    context->Draw(4, 0);

    // otvyaz resorse
    ID3D11ShaderResourceView* nullSRVs[2] = { nullptr, nullptr };
    context->PSSetShaderResources(0, 2, nullSRVs);
}

ID3D11ShaderResourceView* HDR::GetHDRTexture() const {
    return m_pHDRSRV;
}

ID3D11RenderTargetView* HDR::GetHDRRTV() const {
    return m_pHDRRTV;
}