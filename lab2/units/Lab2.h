#pragma once
#include "Dev.h"
#include "engine/Engine.h"

class Lab2 : public UnitBase {
	std::vector<Mesh*> meshes;
	std::vector<Material*> materials;
	Geometry* cubeGeom;
	Texture* texture;
	SphereLight* light;
public:
	Lab2();
	virtual void init() override;
	virtual void update() override;
	virtual ~Lab2() override;
};
