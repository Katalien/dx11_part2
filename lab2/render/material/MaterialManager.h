#pragma once
#include "Dev.h"
#include "render/constantBuffer/ConstantBuffer.h"
#include "render/texture/TextureManager.h"

enum MaterialTextureType
{
	MaterialTextureType_Color = 0,
	MaterialTextureType_Ambient,
	MaterialTextureType_Specular,

	MaterialTextureType_COUNT
};

// ensure order
static_assert(MaterialTextureType_Color == TEXTURE_COLOR_SRV_SLOT, "texture order Mismatch");
static_assert(MaterialTextureType_Ambient == TEXTURE_AMBIENT_SRV_SLOT, "texture order Mismatch");
static_assert(MaterialTextureType_Specular == TEXTURE_SPECULAR_SRV_SLOT, "texture order Mismatch");

class Material {
private:
	friend class MaterialManager;
	Render* render;
	PerMaterialData data;
	Texture* usedTextures[MaterialTextureType_COUNT] = {nullptr};
public:
	Material(Render* rend);
	void setActive(Shader* vertex_sh, Shader* pixel_sh = nullptr);
	PerMaterialData& getData() { return data; }
	void setTexture(Texture* texture, MaterialTextureType type);
	~Material() = default;
};

class MaterialManager {
private:
	Render* render;
	std::vector<Material*> materialPool;
	ConstantBuffer<PerMaterialData> cb;
public:
	MaterialManager(Render* rend);
	void setMaterialActive(Material* mat, Shader* vertex_sh = nullptr, Shader* pixel_sh = nullptr);
	Material* addMaterial();
	~MaterialManager();
};