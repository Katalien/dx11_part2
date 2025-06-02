#include "render/Render.h"

SphereLight::SphereLight(Render* rend) {
    render = rend;
}

DirectionalLight::DirectionalLight(Render* rend) {
    render = rend;
}


LightManager::LightManager(Render* rend) : lightsCB(rend) {
    render = rend;
}

SphereLight* LightManager::addSphereLight(DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 position, float radius) {
    ASSERT(sphereLightPool.size() < POINT_LIGHTS_MAX_AMOUNT);
    SphereLight light = SphereLight(render);
    light.setColor(color);
    light.setPosition(position);
    light.setRadius(radius);

    sphereLightPool.push_back(light);
    return &sphereLightPool[sphereLightPool.size() - 1];
}

DirectionalLight* LightManager::addDirectionalLight(DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 direction) {
    ASSERT(directionalLightPool.size() < DIR_LIGHTS_MAX_AMOUNT);
    DirectionalLight light = DirectionalLight(render);
    light.setColor(color);
    light.setDirection(direction);

    directionalLightPool.push_back(light);
    return &directionalLightPool[directionalLightPool.size() - 1];
}

void LightManager::update() {
    for (int i = 0; i < sphereLightPool.size() && i < POINT_LIGHTS_MAX_AMOUNT; i++) {
        if (sphereLightPool[i].isDirty) {
            lightsCB.getData().pointLights[i].color = sphereLightPool[i].color;
            lightsCB.getData().pointLights[i].radius = sphereLightPool[i].radius;
            lightsCB.getData().pointLights[i].position = sphereLightPool[i].position;
            sphereLightPool[i].isDirty = false;
        }
    }
    lightsCB.getData().pointAmount = sphereLightPool.size();

    for (int i = 0; i < directionalLightPool.size() && i < DIR_LIGHTS_MAX_AMOUNT; i++) {
        if (directionalLightPool[i].isDirty) {
            lightsCB.getData().dirLights[i].color = directionalLightPool[i].color;
            lightsCB.getData().dirLights[i].direction = directionalLightPool[i].direction;
            directionalLightPool[i].isDirty = false;
        }
    }
    lightsCB.getData().dirAmount = directionalLightPool.size();
}

void LightManager::setActive(Shader* sh, int slot) {
    lightsCB.setActive(sh, slot);
}

LightManager::~LightManager() {
    directionalLightPool.clear();
    sphereLightPool.clear();
}

