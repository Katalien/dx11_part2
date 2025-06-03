#include "render/Render.h"

Mesh::Mesh(Render* rend) {
    render = rend;
}

MeshManager::MeshManager(Render* rend) {
    render = rend;
}

DirectX::XMFLOAT4X4 Mesh::getTransform() {
    if (isDirty)
    {
        DirectX::XMMATRIX rotationM = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
        DirectX::XMMATRIX scaleM = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
        DirectX::XMMATRIX translM = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

        DirectX::XMStoreFloat4x4(&transform, DirectX::XMMatrixMultiply(DirectX::XMMatrixMultiply(scaleM, rotationM), translM));
    }

    isDirty = false;
    return transform;
}


Mesh* MeshManager::addMesh(Geometry * geom, Material* mat) {
    Mesh* mesh = new Mesh(render);

    mesh->geometry = geom;
    mesh->material = mat;
    mesh->position = DirectX::XMFLOAT3(0, 0, 0);
    mesh->rotation = DirectX::XMFLOAT3(0, 0, 0);
    mesh->scale = DirectX::XMFLOAT3(1, 1, 1);
    mesh->isDirty = true;

    meshPool.push_back(mesh);
    return mesh;
}

MeshManager::~MeshManager() {
    for (auto mesh : meshPool) {
        delete mesh;
    }
    meshPool.clear();
}

