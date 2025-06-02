#include "engine/Engine.h"

#include "imgui/imgui_impl_dx11.h"

#include "passes/TestTriangle.h"
#include "passes/TestCube.h"
#include "passes/ColorPass.h"
#include "passes/SkyPass.h"
#include "passes/AvgBrightness.h"
#include "passes/ToneMapping.h"

void PassBase::setRender(Render* rend) {
    render = rend;
}

Render::Render(Engine* eng): engine(eng) {}

void Render::init(size_t w, size_t h) {
    width = w;
    height = h;

    bool success = true;
    success = success && createDXGIFactory();
    success = success && createAdapter();
    success = success && createDevice();
    success = success && createSwapChain(engine->getWin()->getHWnd());
    if (!success) {
        return;
    }
    
    shaderManager = new ShaderManager(this);
    geometryManager = new GeometryManager(this);
    materialManager = new MaterialManager(this);
    meshManager = new MeshManager(this);
    textureManager = new TextureManager(this);
    renderTargetManager = new RenderTargetManager(this);
    lightManager = new LightManager(this);

    success = success && createBackBuffer();
    if (!success) {
        return;
    }

    TextureDescriptor texDesc = {};
    texDesc.name = "Depth buffer";
    texDesc.w = backBufferTex->getWidth();
    texDesc.h = backBufferTex->getHeight();
    texDesc.mips = 1;
    texDesc.genMipMaps = false;
    texDesc.format = DXGI_FORMAT_R32_TYPELESS;
    texDesc.isRenderTarget = false;
    texDesc.isDepthTexture = true;
    texDesc.isReadback = false;
    texDesc.pixels = nullptr;

    depthBufferTex = textureManager->addTexture(texDesc);
    depthBufferDS = renderTargetManager->addDepthStencil(depthBufferTex);

    texDesc.name = "HDR";
    texDesc.format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    texDesc.isRenderTarget = true;
    texDesc.isDepthTexture = false;


    HDRTex = textureManager->addTexture(texDesc);
    HDRRT = renderTargetManager->addRenderTarget(HDRTex);

    //passes.push_back(new TestTriangle());
    //passes.push_back(new TestCube());
    passes.push_back(new ColorPass());
    passes.push_back(new SkyPass());
    passes.push_back(new AvgBrightnessPass());
    passes.push_back(new ToneMappingPass());

    for (auto& pass : passes) {
        pass->setRender(this);
        pass->init();
    }
    
    perFrameCB = new ConstantBuffer<PerFrameData>(this);

    resize(w, h);

    {
        D3D11_RASTERIZER_DESC desc = {};
        desc.FillMode = D3D11_FILL_SOLID;
        desc.CullMode = D3D11_CULL_BACK;
        desc.DepthClipEnable = TRUE;

        d3dDevice->CreateRasterizerState(&desc, &defaultRasterizerState);
    }

    ImGui_ImplDX11_Init(d3dDevice, d3dContext);
}

bool Render::createDXGIFactory()
{
    HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&dxgiFactory);
    ASSERT(hr == S_OK);
    return hr == S_OK;
}

bool Render::createAdapter()
{
    IDXGIAdapter* pAdapter = NULL;
    UINT adapterIdx = 0;

    while (SUCCEEDED(dxgiFactory->EnumAdapters(adapterIdx, &pAdapter))) {
        DXGI_ADAPTER_DESC desc;
        pAdapter->GetDesc(&desc);

        if (wcscmp(desc.Description, L"Microsoft Basic Render Driver") != 0) {
            dxgiAdapter = pAdapter;
            break;
        }

        pAdapter->Release();
        adapterIdx++;
    }
    ASSERT(dxgiAdapter != nullptr);
    return dxgiAdapter != nullptr;
}

bool Render::createDevice() {
    HRESULT hr;

    D3D_FEATURE_LEVEL level;
    D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_1 };
    UINT flags = 0;
#ifdef _DEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    hr = D3D11CreateDevice(dxgiAdapter, D3D_DRIVER_TYPE_UNKNOWN, NULL,
        flags, levels, 1, D3D11_SDK_VERSION, &d3dDevice, &level, &d3dContext);
    ASSERT(hr == S_OK);

    hr = d3dContext->QueryInterface(__uuidof(d3dAnnotation), reinterpret_cast<void**>(&d3dAnnotation));
    ASSERT(hr == S_OK);
    return hr == S_OK;
}

bool Render::createSwapChain(HWND hWnd) {
    HRESULT hr;

    DXGI_SWAP_CHAIN_DESC swapChainDesc = { 0 };
    swapChainDesc.BufferCount = 2;
    swapChainDesc.BufferDesc.Width = width;
    swapChainDesc.BufferDesc.Height = height;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = hWnd;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.Windowed = true;
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDesc.Flags = 0;

    hr = dxgiFactory->CreateSwapChain(d3dDevice, &swapChainDesc, &d3dSwapChain);
    ASSERT(hr == S_OK);
    return hr == S_OK;
}

bool Render::createBackBuffer() {
    ID3D11Texture2D* backBuffer;
    HRESULT hr = d3dSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
    ASSERT(hr == S_OK);
    backBufferTex = textureManager->convertBackbufferToTexture(backBuffer);
    if (backbufferRT == nullptr)
        backbufferRT = renderTargetManager->addRenderTarget(backBufferTex);
    else
        backbufferRT->updateTex(backBufferTex);
    return hr == S_OK;
}

void Render::BeginEvent(const wchar_t* eventName)
{
    d3dAnnotation->BeginEvent(eventName);
}

void Render::EndEvent()
{
    d3dAnnotation->EndEvent();
}

void Render::doRender() {
    BeginEvent(L"doRender");
    // clear backbuffer and depth buffer
    lightManager->update();
    backbufferRT->clear({ 0.0f, 0.0f, 0.0f, 1.0f });
    HDRRT->clear({ 0.0f, 0.0f, 0.0f, 1.0f });
    depthBufferDS->clear(1.0f);

    textureManager->clearSlot(TEXTURE_COLOR_SRV_SLOT);
    
    auto &data = perFrameCB->getData();
    data.viewProj = playerCamera.getVP();
    data.cameraPos = playerCamera.getPosition();

    for (auto& pass : passes) {
        pass->process();
        d3dContext->RSSetState(defaultRasterizerState);
    }

    // imgui render
    renderTargetManager->reset(); 
    backbufferRT->setActive();
    renderTargetManager->apply();
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    d3dSwapChain->Present(0, 0);
    EndEvent();
}

void Render::resize(size_t w, size_t h) {
    width = w;
    height = h;

    backbufferRT->release();
    delete backBufferTex;
    d3dSwapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    createBackBuffer();
    
    depthBufferDS->release();
    textureManager->resizeTexture(backBufferTex->getWidth(), backBufferTex->getHeight(), depthBufferTex);
    depthBufferDS->update();

    HDRRT->release();
    textureManager->resizeTexture(backBufferTex->getWidth(), backBufferTex->getHeight(), HDRTex);
    HDRRT->update();

    playerCamera.resize(width, height);
}

float Render::getDTime() { return engine->getTimer()->getDeltaTime(); }

Render::~Render() {
    ImGui_ImplDX11_Shutdown();

    delete perFrameCB;

    for (auto& pass : passes) {
        delete pass;
    }

    delete lightManager;
    delete renderTargetManager;
    delete textureManager;
    delete meshManager;
    delete materialManager;
    delete geometryManager;
    delete shaderManager;
    delete backBufferTex;
    SAFE_RELEASE(defaultRasterizerState);
    SAFE_RELEASE(d3dSwapChain);
    SAFE_RELEASE(d3dAnnotation);
    SAFE_RELEASE(d3dContext);
    SAFE_RELEASE(d3dDevice);
    SAFE_RELEASE(dxgiAdapter);
    SAFE_RELEASE(dxgiFactory);
}

