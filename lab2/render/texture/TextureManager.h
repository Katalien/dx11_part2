#pragma once
#include "Dev.h"
#include "render/constantBuffer/ConstantBuffer.h"

class Texture {
private:
	friend class TextureManager;
	friend class RenderTargetManager;
	friend class RenderTarget;
	friend class DepthStencil;

	Render* render;

	std::string name;
	int w;
	int h;
	int mips;
	DXGI_FORMAT format;
	bool isRenderTarget = false;
	bool isDepthStencil = false;
	bool isReadback = false;
	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* srv;
public:
	Texture(Render* rend);

	void setActive(Shader* sh, int slot);
	int getWidth() { return w; }
	int getHeight() { return h; }
	std::string getName() { return name; }
	DXGI_FORMAT getFormat() { return format; }
	bool getIsRenderTarget() { return isRenderTarget; }
	bool getIsDepthStencil() { return isDepthStencil; }
	std::vector<uint8_t> getPixels();

	~Texture();
};

struct TextureDescriptor {
public:
	std::string name = "Dummy";
	int w = 1;
	int h = 1;
	int mips = 1;
	bool genMipMaps = false;
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
	bool isRenderTarget = false;
	bool isDepthTexture = false;
	bool isReadback = false;
	uint8_t* pixels = nullptr;
};

class TextureManager {
private:
	Render* render;
	std::vector<Texture*> texturePool;
	ID3D11SamplerState* LinearSampler;
	ID3D11SamplerState* NearestSampler;
public:
	TextureManager(Render* rend);
	void setSamplers(Shader* sh);
//	void setTextureActive(Texture* mat, Shader* vertex_sh = nullptr, Shader* pixel_sh = nullptr);
	Texture* addTexture(TextureDescriptor desc);
	Texture* loadTexture(std::string filePath, bool isSrgb = true, bool genMipMaps = false);
	void resizeTexture(int w, int h, Texture* tex);

	void clearSlot(int slot);

	void copyTexture(Texture* dest, Texture* src);

	Texture* convertBackbufferToTexture(ID3D11Texture2D* backbuffer);

	~TextureManager();
};