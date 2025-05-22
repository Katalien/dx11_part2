#include "render/Render.h"
#include "stb_image.h"

Texture::Texture(Render* rend) {
    render = rend;
}

void Texture::setActive(Shader* shader, int slot) {
    ASSERT(srv != nullptr);

    texture->SetPrivateData(WKPDID_D3DDebugObjectName, name.size(), name.c_str());

    switch (shader->getStage())
    {
    case Shader::Stage::Vertex:
        render->getD3DDeviceContext()->VSSetShaderResources(slot, 1, &srv);
        break;
    case Shader::Stage::Pixel:
        render->getD3DDeviceContext()->PSSetShaderResources(slot, 1, &srv);
        break;
    default:
        break;
    }
}

TextureManager::TextureManager(Render* rend) {
    render = rend;

    {
        D3D11_SAMPLER_DESC samplerDesc = {};
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
        samplerDesc.MaxAnisotropy = D3D11_MAX_MAXANISOTROPY;

        render->getD3DDevice()->CreateSamplerState(&samplerDesc, &LinearSampler);
    }
    {
        D3D11_SAMPLER_DESC samplerDesc = {};
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
        samplerDesc.MaxAnisotropy = D3D11_MAX_MAXANISOTROPY;

        render->getD3DDevice()->CreateSamplerState(&samplerDesc, &NearestSampler);
    }
}


void TextureManager::setSamplers(Shader* shader) {
    switch (shader->getStage())
    {
    case Shader::Stage::Vertex:
        render->getD3DDeviceContext()->VSSetSamplers(LINEAR_SAMPLER_SLOT, 1, &LinearSampler);
        render->getD3DDeviceContext()->VSSetSamplers(NEAREST_SAMPLER_SLOT, 1, &NearestSampler);
        break;
    case Shader::Stage::Pixel:
        render->getD3DDeviceContext()->PSSetSamplers(LINEAR_SAMPLER_SLOT, 1, &LinearSampler);
        render->getD3DDeviceContext()->PSSetSamplers(NEAREST_SAMPLER_SLOT, 1, &NearestSampler);
        break;
    default:
        break;
    }
}

Texture* TextureManager::addTexture(TextureDescriptor texDesc) {
    Texture* texture = new Texture(render);
    texture->w = texDesc.w;
    texture->h = texDesc.h;
    texture->name = texDesc.name;
    texture->mips = texDesc.mips;
    texture->format = texDesc.format;
    texture->isRenderTarget = texDesc.isRenderTarget;
    texture->isDepthStencil = texDesc.isDepthTexture;
    texture->isReadback = texDesc.isReadback;
    
    D3D11_TEXTURE2D_DESC desc;
    desc.Width = texDesc.w;
    desc.Height = texDesc.h;
    desc.MipLevels = texDesc.mips;
    desc.ArraySize = 1;
    desc.Format = texDesc.format;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = texDesc.isReadback ? D3D11_USAGE_STAGING : D3D11_USAGE_DEFAULT;
    desc.BindFlags = texDesc.isReadback ? 0 : D3D11_BIND_SHADER_RESOURCE;
    if (texDesc.isRenderTarget)
        desc.BindFlags = desc.BindFlags | D3D11_BIND_RENDER_TARGET;
    if (texDesc.isDepthTexture)
        desc.BindFlags = desc.BindFlags | D3D11_BIND_DEPTH_STENCIL;
    desc.CPUAccessFlags = texDesc.isReadback ? D3D11_CPU_ACCESS_READ : 0;
    desc.MiscFlags = 0;
    if (texDesc.genMipMaps)
    {
        desc.BindFlags = desc.BindFlags | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        desc.MiscFlags = desc.MiscFlags | D3D11_RESOURCE_MISC_GENERATE_MIPS;
    }

    HRESULT hr = S_OK;
    hr = render->getD3DDevice()->CreateTexture2D(&desc, nullptr, &texture->texture);
    ASSERT(hr == S_OK);
    if (!texDesc.isReadback)
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC srvdesc = {};
        srvdesc.Format = texDesc.format;
        if (texDesc.format == DXGI_FORMAT_R32_TYPELESS)
            srvdesc.Format = DXGI_FORMAT_R32_FLOAT;
        srvdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvdesc.Texture2D.MostDetailedMip = 0;
        srvdesc.Texture2D.MipLevels = texDesc.mips;

        hr = render->getD3DDevice()->CreateShaderResourceView(texture->texture, &srvdesc, &(texture->srv));
        ASSERT(hr == S_OK);
    }

    if (texDesc.pixels)
    {
        int pixelSize = 0;
        switch (texDesc.format)
        {
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R32_FLOAT:
        case DXGI_FORMAT_D32_FLOAT:
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
            pixelSize = 4;
            break;
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
            pixelSize = 16;
            break;
        default:
            break;
        }
        
        render->getD3DDeviceContext()->UpdateSubresource(texture->texture, 0, nullptr, texDesc.pixels, texDesc.w * pixelSize, texDesc.w * texDesc.h * pixelSize);
    }


    if (texDesc.mips > 1 && texDesc.genMipMaps) {
        render->getD3DDeviceContext()->GenerateMips(texture->srv);
    }

    texturePool.push_back(texture);

    return texture;

}

void TextureManager::resizeTexture(int w, int h, Texture* tex)
{
    D3D11_TEXTURE2D_DESC desc;
    tex->texture->GetDesc(&desc);

    ASSERT(desc.MipLevels == 1 && "NOT IMPLEMENTED FOR TEXTURES WITH MIPS");
    desc.Width = w;
    desc.Height = h;

    SAFE_RELEASE(tex->srv);
    SAFE_RELEASE(tex->texture);
    HRESULT hr = S_OK;
    hr = render->getD3DDevice()->CreateTexture2D(&desc, nullptr, &tex->texture);
    ASSERT(hr == S_OK);

    D3D11_SHADER_RESOURCE_VIEW_DESC srvdesc = {};
    srvdesc.Format = desc.Format;
    if (desc.Format == DXGI_FORMAT_R32_TYPELESS)
        srvdesc.Format = DXGI_FORMAT_R32_FLOAT;
    srvdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvdesc.Texture2D.MostDetailedMip = 0;
    srvdesc.Texture2D.MipLevels = desc.MipLevels;

    hr = render->getD3DDevice()->CreateShaderResourceView(tex->texture, &srvdesc, &(tex->srv));
    ASSERT(hr == S_OK);
}

void TextureManager::clearSlot(int slot)
{
    ID3D11ShaderResourceView* res = nullptr;
    render->getD3DDeviceContext()->VSSetShaderResources(slot, 1, &res);
    render->getD3DDeviceContext()->PSSetShaderResources(slot, 1, &res);
}

Texture* TextureManager::loadTexture(std::string filePath, bool isSrgb, bool genMipMaps) {
    int width = 0;
    int height = 0;
    int channels = 0;

    uint8_t *pixels = stbi_load(filePath.c_str(), &width, &height, &channels, 4);
    ASSERT(pixels != nullptr);

    int numOfMips = genMipMaps ? max(log2(min(width, height)) - 1, 1) : 1;

    uint8_t* pixelsWithMips = new uint8_t[width * height * 4 * sizeof(uint8_t) * 2];

    memcpy(pixelsWithMips, pixels, width * height * 4 * sizeof(uint8_t));

    stbi_image_free(pixels);

    TextureDescriptor texDesc = {};
    texDesc.name = filePath;
    texDesc.w = width;
    texDesc.h = height;
    texDesc.mips = numOfMips;
    texDesc.genMipMaps = genMipMaps;
    texDesc.format = isSrgb ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.isRenderTarget = false;
    texDesc.isDepthTexture = false;
    texDesc.isReadback = false;
    texDesc.pixels = pixelsWithMips;

    Texture* tex = addTexture(texDesc);

    delete[] pixelsWithMips;

    return tex;
}

Texture* TextureManager::convertBackbufferToTexture(ID3D11Texture2D* backbuffer)
{
    Texture* tex = new Texture(render);
    D3D11_TEXTURE2D_DESC desc;
    backbuffer->GetDesc(&desc);
    tex->w = desc.Width;
    tex->h = desc.Height;
    tex->name = "BackBuffer";
    tex->mips = 1;
    tex->format = desc.Format;
    tex->isRenderTarget = true;
    tex->isDepthStencil = false;
    tex->texture = backbuffer;
    tex->srv = nullptr;
    return tex;
}

std::vector<uint8_t> Texture::getPixels() {
    D3D11_MAPPED_SUBRESOURCE s = {};
    render->getD3DDeviceContext()->Map(texture, 0, D3D11_MAP_READ, 0, &s);
    std::vector<uint8_t> res;
    res.resize(h * s.RowPitch);
    memcpy(res.data(), s.pData, h * s.RowPitch);
    render->getD3DDeviceContext()->Unmap(texture, 0);
    return res;
}


void TextureManager::copyTexture(Texture* dest, Texture* src) {
    render->getD3DDeviceContext()->CopyResource(dest->texture, src->texture);
}


Texture::~Texture() {
    SAFE_RELEASE(srv);
    SAFE_RELEASE(texture);
}

TextureManager::~TextureManager() {
    SAFE_RELEASE(LinearSampler);
    SAFE_RELEASE(NearestSampler);

    for (auto texture : texturePool) {
        delete texture;
    }
    texturePool.clear();
}
