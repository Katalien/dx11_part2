#pragma once
#include "Dev.h"
#include "engine/Engine.h"

class Lab3 : public UnitBase {
private:
	const float minMetalness = 0.1f;
	const float maxMetalness = 0.9999f;

	const float minRoughness = 0.1f;
	const float maxRoughness = 0.9999f;

	static const char* imguiShadeModels[];

	std::vector<Mesh*> meshes;
	std::vector<Material*> materials;
	Geometry* sphereGeom;
	Texture* texture;
	SphereLight* light;
	Material* lightMaterial;

	static const char* textureNames[];
	static const char* textureFilePaths[];

	int selectedShadeModel = 5;
	int selectedTexture = 1;
public:
	Lab3();
	virtual void init() override;
	virtual void update() override;
	virtual ~Lab3() override;
};