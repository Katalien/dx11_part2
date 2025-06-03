#include "render/Render.h"

Material::Material(Render* rend) {
    render = rend;
}

void Material::setActive(Shader* vert_sh, Shader* pixel_sh) {
    for (int type = 0; type < MaterialTextureType_COUNT; type++)
        if (usedTextures[type] != nullptr) {
            usedTextures[type]->setActive(vert_sh, SHADER_SRV_SLOT(type));
            usedTextures[type]->setActive(pixel_sh, SHADER_SRV_SLOT(type));
            reinterpret_cast<uint32_t *>(&(data.usedTextures))[type] = true;
        }
        else
        {
            reinterpret_cast<uint32_t*>(&(data.usedTextures))[type] = false;
        }
    
    render->getMaterialManager()->setMaterialActive(this, vert_sh, pixel_sh);
}

MaterialManager::MaterialManager(Render* rend) : cb(rend) {
    render = rend;
}

void MaterialManager::setMaterialActive(Material *mat, Shader* vert_sh, Shader* pixel_sh) {
    cb.getData() = mat->getData();
    if (vert_sh)
        cb.setActive(vert_sh, PER_MATERIAL_CB_SLOT);
    if (pixel_sh)
        cb.setActive(pixel_sh, PER_MATERIAL_CB_SLOT);
}

void Material::setTexture(Texture* texture, MaterialTextureType type) {
    usedTextures[type] = texture;
}

Material* MaterialManager::addMaterial() {
    Material* material = new Material(render);

    material->getData().color = DirectX::XMFLOAT4(1, 0, 1, 1);
    material->getData().shadeModel = SHADE_MODEL_COLOR;

    materialPool.push_back(material);

    return material;
}

MaterialManager::~MaterialManager() {
    for (auto material : materialPool) {
        delete material;
    }
    materialPool.clear();
}

