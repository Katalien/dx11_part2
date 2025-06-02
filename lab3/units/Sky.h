#pragma once
#include "Dev.h"
#include "engine/Engine.h"

class Sky : public UnitBase {
	std::vector<Mesh*> meshes;
	std::vector<Material*> materials;
	Geometry* sphereGeom;
	CubeTexture* texture;
	SphereLight* light;
public:
	Sky();
	virtual void init() override;
	virtual void update() override;
	virtual ~Sky() override;
};
