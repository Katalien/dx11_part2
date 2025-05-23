#pragma once
#include "Dev.h"

struct VertexDefault
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 uv;
};

class Geometry {
public:
	enum VertexType {
		Default = 0,

		VertexType_Count
	};
private:
	friend class GeometryManager;
	Render* render;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	VertexType vertexType = Default;
	D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	size_t indexCount;
public:
	Geometry(Render* rend);
	void setActive();
	size_t getIndexCount() { return indexCount; }
	VertexType getVertexType() { return vertexType; }
	~Geometry();
};

class GeometryManager {
private:
	Render* render;
	std::vector<Geometry*> geometryPool;
	ID3D11InputLayout* layouts[Geometry::VertexType::VertexType_Count];
public:
	GeometryManager(Render* rend);
	void setLayout(Geometry::VertexType vertexType);
	Geometry* addGeometry(VertexDefault* vertices, size_t vertexCount, uint32_t* indices, size_t indexCount, D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Geometry* addCube();
	~GeometryManager();
};