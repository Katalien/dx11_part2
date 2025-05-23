#include "render/Render.h"

RenderTarget::RenderTarget(Render* rend) {
    render = rend;
}

void RenderTarget::setActive() {
    render->getRenderTargetManager()->setRenderTargetActive(this);
}

void RenderTarget::update() {
    SAFE_RELEASE(rtv);
    render->getD3DDevice()->CreateRenderTargetView(texture->texture, nullptr, &rtv);
}

void RenderTarget::release() {
    SAFE_RELEASE(rtv);
}

void RenderTarget::updateTex(Texture* tex) {
    SAFE_RELEASE(rtv);
    texture = tex;
    render->getD3DDevice()->CreateRenderTargetView(texture->texture, nullptr, &rtv);
}


void RenderTarget::clear(DirectX::XMFLOAT4 color) {
    render->getD3DDeviceContext()->ClearRenderTargetView(rtv, &color.x);
}

RenderTarget::~RenderTarget(void) {
   SAFE_RELEASE(rtv);
}

DepthStencil::DepthStencil(Render* rend) {
    render = rend;
}

void DepthStencil::setActive() {
    render->getRenderTargetManager()->setDepthStencilActive(this);
}

void DepthStencil::release() {
    SAFE_RELEASE(dsv);
}

void DepthStencil::update() {
    SAFE_RELEASE(dsv);
    D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};
    desc.Format = DXGI_FORMAT_D32_FLOAT;
    desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    desc.Texture2D.MipSlice = 0;
    render->getD3DDevice()->CreateDepthStencilView(texture->texture, &desc, &dsv);
}

void DepthStencil::clear(float val) {
    render->getD3DDeviceContext()->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, val, 0);
}

DepthStencil::~DepthStencil(void) {
    SAFE_RELEASE(dsv);
}

RenderTargetManager::RenderTargetManager(Render* rend) {
    render = rend;
}

void RenderTargetManager::setRenderTargetActive(RenderTarget *rt) {
    curRenderTarget = rt;
}

void RenderTargetManager::setDepthStencilActive(DepthStencil* ds) {
    curDepthStencil = ds;
}

void RenderTargetManager::apply() {
    if (curRenderTarget != nullptr)
    {
        curRenderTarget->rtv->SetPrivateData(WKPDID_D3DDebugObjectName, curRenderTarget->texture->getName().size(), curRenderTarget->texture->getName().c_str());
        curRenderTarget->texture->texture->SetPrivateData(WKPDID_D3DDebugObjectName, curRenderTarget->texture->getName().size(), curRenderTarget->texture->getName().c_str());

        D3D11_VIEWPORT viewport[1];
        viewport[0].TopLeftX = 0;
        viewport[0].TopLeftY = 0;
        viewport[0].MaxDepth = 1;
        viewport[0].MinDepth = 0;
        viewport[0].Height = curRenderTarget->texture->getHeight();
        viewport[0].Width = curRenderTarget->texture->getWidth();
        render->getD3DDeviceContext()->RSSetViewports(1, viewport);

        D3D11_RECT scissor[1];
        scissor[0].left = 0;
        scissor[0].top = 0;
        scissor[0].bottom = curRenderTarget->texture->getHeight();
        scissor[0].right = curRenderTarget->texture->getWidth();
        render->getD3DDeviceContext()->RSSetScissorRects(1, scissor);

    }

    if (curDepthStencil != nullptr)
    {
        curDepthStencil->dsv->SetPrivateData(WKPDID_D3DDebugObjectName, curDepthStencil->texture->getName().size(), curDepthStencil->texture->getName().c_str());
        curDepthStencil->texture->texture->SetPrivateData(WKPDID_D3DDebugObjectName, curDepthStencil->texture->getName().size(), curDepthStencil->texture->getName().c_str());
    }

    if (curRenderTarget != nullptr && curDepthStencil != nullptr)
    {
        render->getD3DDeviceContext()->OMSetRenderTargets(1, &(curRenderTarget->rtv), curDepthStencil->dsv);
    }
    else if (curRenderTarget == nullptr && curDepthStencil != nullptr)
    {
        render->getD3DDeviceContext()->OMSetRenderTargets(0, nullptr, curDepthStencil->dsv);
    }
    else if (curRenderTarget != nullptr && curDepthStencil == nullptr)
    {
        render->getD3DDeviceContext()->OMSetRenderTargets(1, &(curRenderTarget->rtv), nullptr);
    }
    else if (curRenderTarget == nullptr && curDepthStencil == nullptr)
    {
        render->getD3DDeviceContext()->OMSetRenderTargets(0, nullptr, nullptr);
    }
}

RenderTarget* RenderTargetManager::addRenderTarget(Texture* tex) {
    RenderTarget* rt = new RenderTarget(render);
    rt->texture = tex;
    rt->update();

    renderTargetPool.push_back(rt);

    return rt;
}


DepthStencil* RenderTargetManager::addDepthStencil(Texture* tex) {
    DepthStencil* ds = new DepthStencil(render);
    ds->texture = tex;
    ds->update();

    depthStencilPool.push_back(ds);

    return ds;
}

RenderTargetManager::~RenderTargetManager() {
    for (auto rt : renderTargetPool) {
        delete rt;
    }
    renderTargetPool.clear();

    for (auto ds : depthStencilPool) {
        delete ds;
    }
    depthStencilPool.clear();
    
    curRenderTarget = nullptr;
    curDepthStencil = nullptr;
}

