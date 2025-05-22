#pragma once
#include "Dev.h"
#include "render/constantBuffer/ConstantBuffer.h"
#include "render/texture/TextureManager.h"

class RenderTarget {
private:
	friend class RenderTargetManager;
	Render* render;
	Texture* texture;
	ID3D11RenderTargetView* rtv = nullptr;
public:
	RenderTarget(Render* rend);
	void setActive();
	void update();
	void updateTex(Texture* tex);
	void release();
	void clear(DirectX::XMFLOAT4 color);
	~RenderTarget();
};

class DepthStencil {
private:
	friend class RenderTargetManager;
	Render* render;
	Texture* texture;
	ID3D11DepthStencilView* dsv;
public:
	DepthStencil(Render* rend);
	void setActive();
	void update();
	void release();
	void clear(float val);
	~DepthStencil();
};

class RenderTargetManager {
private:
	Render* render;
	std::vector<RenderTarget*> renderTargetPool;
	std::vector<DepthStencil*> depthStencilPool;
	RenderTarget* curRenderTarget = nullptr;
	DepthStencil* curDepthStencil = nullptr;
public:
	RenderTargetManager(Render* rend);
	void setRenderTargetActive(RenderTarget* rt);
	void setDepthStencilActive(DepthStencil* ds);
	void reset() { curRenderTarget = nullptr; curDepthStencil = nullptr; }
	void apply();
	RenderTarget* addRenderTarget(Texture* tex);
	DepthStencil* addDepthStencil(Texture* tex);
	~RenderTargetManager();
};