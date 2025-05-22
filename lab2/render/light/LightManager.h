#pragma once
#include "Dev.h"
#include "render/constantBuffer/ConstantBuffer.h"

class SphereLight {
private:
	friend class LightManager;
	Render* render;
	bool isDirty = true;

	DirectX::XMFLOAT3 color = { 0, 0, 0 };

	DirectX::XMFLOAT3 position = { 0, 0, 0 };
	float radius = 0;
public:
	SphereLight(Render* rend);
	DirectX::XMFLOAT3 getColor() { return color; }
	void setColor(DirectX::XMFLOAT3 col) { isDirty = true; color = col; }

	DirectX::XMFLOAT3 getPosition() { return position; }
	void setPosition(DirectX::XMFLOAT3 pos) { isDirty = true; position = pos; }

	float getRadius() { return radius; }
	void setRadius(float r) { radius = r; }

	~SphereLight() = default;
};

class DirectionalLight {
private:
	friend class LightManager;
	Render* render;
	bool isDirty = true;

	DirectX::XMFLOAT3 color = { 0, 0, 0 };

	DirectX::XMFLOAT3 direction = { 0, 0, 1 };
public:
	DirectionalLight(Render* rend);
	DirectX::XMFLOAT3 getColor() { return color; }
	void setColor(DirectX::XMFLOAT3 col) { isDirty = true; color = col; }

	DirectX::XMFLOAT3 getDirection() { return direction; }
	void setDirection(DirectX::XMFLOAT3 dir) { isDirty = true; direction = dir; }

	~DirectionalLight() = default;
};

class LightManager {
private:
	Render* render;
	std::vector<SphereLight> sphereLightPool;
	std::vector<DirectionalLight> directionalLightPool;
	ConstantBuffer<LightsData> lightsCB;
public:
	LightManager(Render* rend);
	SphereLight* addSphereLight(DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 position, float radius);
	DirectionalLight* addDirectionalLight(DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 direction);
	void update();
	void setActive(Shader* sh, int slot);
	~LightManager();
};