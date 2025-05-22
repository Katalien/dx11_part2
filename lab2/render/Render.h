#pragma once
#include "Dev.h"
#include "constantBuffer/ConstantBuffer.h"
#include "shaderManager/ShaderManager.h"
#include "passes/PassBase.h"
#include "camera/Camera.h"
#include "geometry/GeometryManager.h"
#include "material/MaterialManager.h"
#include "mesh/MeshManager.h"
#include "texture/TextureManager.h"
#include "renderTarget/RenderTargetManager.h"
#include "light/LightManager.h"

class Render {
private:
	Engine* engine = nullptr;

	ShaderManager* shaderManager = nullptr;
	GeometryManager* geometryManager = nullptr;
	MaterialManager* materialManager = nullptr;
	MeshManager* meshManager = nullptr;
	TextureManager* textureManager = nullptr;
	RenderTargetManager* renderTargetManager = nullptr;
	LightManager* lightManager = nullptr;

	IDXGIFactory* dxgiFactory = nullptr;
	IDXGIAdapter* dxgiAdapter = nullptr;
	ID3D11Device* d3dDevice = nullptr;
	ID3D11DeviceContext* d3dContext = nullptr;
	ID3DUserDefinedAnnotation* d3dAnnotation = nullptr;
	IDXGISwapChain* d3dSwapChain = nullptr;

	size_t width = 0;
	size_t height = 0;
	RenderTarget* backbufferRT = nullptr;
	Texture* backBufferTex = nullptr;
	
	DepthStencil* depthBufferDS = nullptr;
	Texture* depthBufferTex = nullptr;

	RenderTarget* HDRRT = nullptr;
	Texture* HDRTex = nullptr;

	float curAvgBrightness = 1;
	float adaptedAvgBrightness = 1;

	std::vector<PassBase*> passes;

	bool createDXGIFactory();
	bool createAdapter();
	bool createDevice();
	bool createSwapChain(HWND hWnd);
	bool createBackBuffer();
public:
	Camera playerCamera;
	Render(Engine* eng);
	void init(size_t w, size_t h);
	void doRender();
	void resize(size_t w, size_t h);

	void BeginEvent(const wchar_t* eventName);
	void EndEvent();

	ID3D11Device* getD3DDevice() { return d3dDevice; }
	ID3D11DeviceContext* getD3DDeviceContext() { return d3dContext; }
	ShaderManager* getShaderManager() { return shaderManager; }
	GeometryManager* getGeometryManager() { return geometryManager; }
	MaterialManager* getMaterialManager() { return materialManager; }
	MeshManager* getMeshManager() { return meshManager; }
	TextureManager* getTextureManager() { return textureManager; }
	RenderTargetManager* getRenderTargetManager() { return renderTargetManager; }
	LightManager* getLightManager() { return lightManager; }

	Texture* getBackBufferTexture() { return backBufferTex; }
	Texture* getDepthBufferTexture() { return depthBufferTex; }
	Texture* getHDRTexture() { return HDRTex; }

	RenderTarget* getBackBufferTarget() { return backbufferRT; }
	RenderTarget* getHDRTarget() { return HDRRT; }
	DepthStencil* getDepthBufferTarget() { return depthBufferDS; }

	float getCurAvgBrigtness() { return curAvgBrightness; }
	void setCurAvgBrightness(float val) { curAvgBrightness = val; }
	float getAdaptedAvgBrigtness() { return adaptedAvgBrightness; }
	void setAdaptedAvgBrightness(float val) { adaptedAvgBrightness = val; }

	float getDTime();

	ConstantBuffer<PerFrameData>* perFrameCB;
	~Render();
};

#include "constantBuffer/ConstantBufferImpl.hpp"