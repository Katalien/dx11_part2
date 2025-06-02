#include "render/Render.h"

Geometry::Geometry(Render* rend) {
	render = rend;
}

void Geometry::setActive() {
    static const UINT stridesByType[VertexType_Count] = { sizeof(VertexDefault) }; // Исправлено
    static const UINT offset = 0;
    render->getD3DDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stridesByType[vertexType], &offset);
    render->getD3DDeviceContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    render->getD3DDeviceContext()->IASetPrimitiveTopology(topology);
}

Geometry::~Geometry() {
    SAFE_RELEASE(vertexBuffer);
    SAFE_RELEASE(indexBuffer);
}

GeometryManager::GeometryManager(Render* rend) {
	render = rend;
    // Use fake shader for each vertex type to generate input layout
    std::vector<D3D11_INPUT_ELEMENT_DESC> InputDesc[Geometry::VertexType_Count] = {
        // default vertex
        {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"NORMAL", 0,  DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
        },
    };
    std::string InputDescShader[Geometry::VertexType_Count] = {
        "shaders/inputLayout/default.hlsl"
    };

    for (size_t i = 0; i < Geometry::VertexType_Count; i++)
    {
        layouts[i] = render->getShaderManager()->getInputLayout(InputDesc[i].data(), InputDesc[i].size(), InputDescShader[i]);
    }
}

void GeometryManager::setLayout(Geometry::VertexType vertexType) {
    render->getD3DDeviceContext()->IASetInputLayout(layouts[vertexType]);
}

Geometry* GeometryManager::addGeometry(VertexDefault* vertices, size_t vertexCount, uint32_t* indices, size_t indexCount, D3D11_PRIMITIVE_TOPOLOGY topology) {
    Geometry* geometry = new Geometry(render);

    geometry->vertexType = Geometry::VertexType::Default;
    geometry->indexCount = indexCount;
    geometry->topology = topology;

    {
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = sizeof(VertexDefault) * vertexCount;
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA data;
        data.pSysMem = vertices;
        data.SysMemPitch = sizeof(VertexDefault) * vertexCount;
        data.SysMemSlicePitch = 0;

        HRESULT hr = render->getD3DDevice()->CreateBuffer(&desc, &data, &geometry->vertexBuffer);
        ASSERT(SUCCEEDED(hr) && "VERTEX BUFFER NOT CREATED");
    }

    {
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = sizeof(uint32_t) * indexCount;
        desc.Usage = D3D11_USAGE_IMMUTABLE;
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA data;
        data.pSysMem = indices;
        data.SysMemPitch = sizeof(uint32_t) * indexCount;
        data.SysMemSlicePitch = 0;

        HRESULT hr = render->getD3DDevice()->CreateBuffer(&desc, &data, &geometry->indexBuffer);
        ASSERT(SUCCEEDED(hr) && "INDEX BUFFER NOT CREATED");
    }

    geometryPool.push_back(geometry);

    return geometry;
}

Geometry* GeometryManager::addCube() {
    VertexDefault vertices[] = {
        // -x
            { DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
            { DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },
            { DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f) },
            { DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },
        // -z
            { DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
            { DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },
            { DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f), DirectX::XMFLOAT2(1.0f, 1.0f) },
            { DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },
        // +y
            { DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
            { DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },
            { DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f) },
            { DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },
        // -y
            { DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
            { DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },
            { DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f) },
            { DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },
        // +x
            { DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
            { DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },
            { DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f) },
            { DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },
        // +z
            { DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
            { DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },
            { DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(1.0f, 1.0f) },
            { DirectX::XMFLOAT3(1.0f, -1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(0.0f, 1.0f) }
    };

    uint32_t indices[] = { 
        0, 1, 3, 1, 2, 3,
        4, 5, 7, 5, 6, 7,
        8, 9, 11, 9, 10, 11,
        12, 13, 15, 13, 14, 15,
        16, 17, 19, 17, 18, 19, 
        20, 21, 23, 21, 22, 23
    };

    return addGeometry(vertices, 24, indices, 36);
}

Geometry* GeometryManager::addSphere(int n, int m) {
    std::vector<VertexDefault> vertices;
    std::vector<uint32_t> indices;

    vertices.reserve(n * m);
    vertices.reserve(2 * 3 * (n - 1) * (m - 1));

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            float theta = i * M_PI / (n - 1) - M_PI / 2.0f;
            float phi = j * 2 * M_PI / (m - 1);

            VertexDefault vert;
            vert.position.x = sinf(phi) * cosf(theta);
            vert.position.y = sinf(theta);
            vert.position.z = cosf(phi) * cosf(theta);

            vert.normal = vert.position;

            vert.position.x *= 0.5f;
            vert.position.y *= 0.5f;
            vert.position.z *= 0.5f;

            vert.uv.x = 1.0f - 1.0 * j / (m - 1);
            vert.uv.y = 1.0f - 1.0 * i / (n - 1);
            vertices.push_back(vert);

            if (i == n - 1) continue;

            indices.push_back(i * m + j);
            indices.push_back(i * m + (j + 1) % m);
            indices.push_back((i + 1) * m + j);

            indices.push_back((i + 1) * m + j);
            indices.push_back(i * m + (j + 1) % m);
            indices.push_back((i + 1) * m + (j + 1) % m);
        }
    }

    return addGeometry(vertices.data(), vertices.size(), indices.data(), indices.size());
}

GeometryManager::~GeometryManager() {

    for (size_t i = 0; i < Geometry::VertexType_Count; i++)
    {
        SAFE_RELEASE(layouts[i]);
    }

    for (auto geometry : geometryPool) {
        delete geometry;
    }
    geometryPool.clear();
}

