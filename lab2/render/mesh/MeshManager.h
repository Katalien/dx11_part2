#pragma once
#include "Dev.h"
#include "render/geometry/GeometryManager.h"
#include "render/material/MaterialManager.h"

class Mesh {
private:
	friend class MeshManager;
	Render* render;
	Geometry* geometry;
	Material* material;
	DirectX::XMFLOAT4X4 transform;
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT3 scale;
	bool isDirty = true;

public:
	Mesh(Render* rend);
	Geometry* getGeometry() { return geometry; }
	Material* getMaterial() { return material; }
	DirectX::XMFLOAT4X4 getTransform();
	DirectX::XMFLOAT3 getPosition() { return position; }
	DirectX::XMFLOAT3 getRotation() { return rotation; }
	DirectX::XMFLOAT3 getScale() { return scale; }

	void setPosition(DirectX::XMFLOAT3 pos) { isDirty = true; position = pos; }
	void setRotation(DirectX::XMFLOAT3 rot) { isDirty = true; rotation = rot; }
	void setScale(DirectX::XMFLOAT3 sc) { isDirty = true; scale = sc; }
	~Mesh() = default;
};

class MeshManager {
private:
	Render* render;
	std::vector<Mesh*> meshPool;
public:
	MeshManager(Render* rend);
	Mesh* addMesh(Geometry *geom, Material *mat);
	std::vector<Mesh*>& getMeshes() { return meshPool; }
	~MeshManager();
};