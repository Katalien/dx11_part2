#include "render/Render.h"

Shader::Shader(Render* rend)
{
	render = rend;
	d3d.vertexShader = nullptr;
}

void Shader::setActive()
{
	switch (stage)
	{
	case Shader::Vertex:
		render->getD3DDeviceContext()->VSSetShader(d3d.vertexShader, nullptr, 0);
		break;
	case Shader::Pixel:
		render->getD3DDeviceContext()->PSSetShader(d3d.pixelShader, nullptr, 0);
		break;
	default:
		break;
	}
}

Shader::~Shader()
{
	switch (stage)
	{
	case Shader::Vertex:
		SAFE_RELEASE(d3d.vertexShader);
		break;
	case Shader::Pixel:
		SAFE_RELEASE(d3d.pixelShader);
		break;
	default:
		break;
	}
}

ShaderManager::ShaderManager(Render* rend) {
	render = rend;
}

class D3DInclude : public ID3DInclude {
public:
	HRESULT Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes) {
		FILE* F = nullptr;
		LPVOID pData = nullptr;

		fopen_s(&F, pFileName, "rb");
		if (F == nullptr)
		{
			return E_FAIL;
		}

		fseek(F, 0, SEEK_END);
		*pBytes = ftell(F);
		pData = malloc(*pBytes);
		fseek(F, 0, SEEK_SET);
		fread(pData, 1, *pBytes, F);
		fclose(F);
		*ppData = pData;

		return S_OK;
	}

	HRESULT Close(LPCVOID pData) {
		free(const_cast<void*>(pData));

		return S_OK;
	}
};

Shader* ShaderManager::getShader(std::string path, Shader::Stage stage, std::map<std::string, std::string> defines)
{
	static const char* d3dTarget[Shader::Stage::Stage_Count] =
	{
		"vs_5_0",
		"ps_5_0"
	};

	static const char* d3dEntryPoint[Shader::Stage::Stage_Count] =
	{
		"VSMain",
		"PSMain"
	};
	static D3DInclude incl;

	for (auto& shader : shadersPool)
	{
		if (shader->name == path
			&& shader->stage == stage
			&& shader->defines.size() == defines.size()
			&& std::equal(shader->defines.begin(), shader->defines.end(), defines.begin()))
		{
			return shader;
		}
	}

	Shader* shader = new Shader(render);
	shader->name = path;
	shader->stage = stage;
	shader->defines = defines;

	ID3D10Blob* shaderBuffer = nullptr;
	ID3D10Blob* errorBuffer = nullptr;
	int flags = 0;
#ifdef _DEBUG
	flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	std::vector<D3D_SHADER_MACRO> macros;

	int i = 0;
	macros.resize(defines.size() + 1);
	for (auto& define : defines)
	{
		macros[i].Name = define.first.c_str();
		macros[i].Definition = define.second.c_str();
		i++;
	}
	macros[i].Name = nullptr;
	macros[i].Definition = nullptr;

	std::wstring wpath(path.begin(), path.end());
	HRESULT hr = D3DCompileFromFile(wpath.c_str(), macros.data(), &incl, d3dEntryPoint[stage], d3dTarget[stage], flags, 0, &shaderBuffer, &errorBuffer);

	if (errorBuffer != nullptr)
	{
		if (FAILED(hr))
		{
			OutputDebugStringA(reinterpret_cast<const char *>(errorBuffer->GetBufferPointer()));
			OutputDebugStringA("\n");
			ASSERT(0 && "SHADER ERROR");
		}
		SAFE_RELEASE(errorBuffer);
	}

	switch (stage)
	{
	case Shader::Vertex:
		render->getD3DDevice()->CreateVertexShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), nullptr, &shader->d3d.vertexShader);
		shader->d3d.vertexShader->SetPrivateData(WKPDID_D3DDebugObjectName, path.size(), path.data());
		break;
	case Shader::Pixel:
		render->getD3DDevice()->CreatePixelShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), nullptr, &shader->d3d.pixelShader);
		shader->d3d.pixelShader->SetPrivateData(WKPDID_D3DDebugObjectName, path.size(), path.data());
		break;
	default:
		ASSERT(0 && "UNKNOWN SHADER TYPE");
		break;
	}

	SAFE_RELEASE(shaderBuffer);

	shadersPool.push_back(shader);

	return shader;
}

ID3D11InputLayout* ShaderManager::getInputLayout(D3D11_INPUT_ELEMENT_DESC* inputDesc, size_t inputDescAmount, std::string path, std::map<std::string, std::string> defines)
{
	static D3DInclude incl;
	ID3D10Blob* shaderBuffer = nullptr;
	ID3D10Blob* errorBuffer = nullptr;
	int flags = 0;
	std::vector<D3D_SHADER_MACRO> macros;

	int i = 0;
	macros.resize(defines.size() + 1);
	for (auto& define : defines)
	{
		macros[i].Name = define.first.c_str();
		macros[i].Definition = define.second.c_str();
		i++;
	}
	macros[i].Name = nullptr;
	macros[i].Definition = nullptr;

	std::wstring wpath(path.begin(), path.end());
	HRESULT hr = D3DCompileFromFile(wpath.c_str(), macros.data(), &incl, "VSMain", "vs_5_0", flags, 0, &shaderBuffer, &errorBuffer);

	if (errorBuffer != nullptr)
	{
		if (FAILED(hr))
		{
			OutputDebugStringA(reinterpret_cast<const char*>(errorBuffer->GetBufferPointer()));
			OutputDebugStringA("\n");
			ASSERT(0 && "SHADER ERROR");
		}
		SAFE_RELEASE(errorBuffer);
	}

	ID3D11InputLayout* inputLayout = nullptr;
	hr = render->getD3DDevice()->CreateInputLayout(inputDesc, inputDescAmount, shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), &inputLayout);
	ASSERT(SUCCEEDED(hr) && "INPUT LAYOUT CANNOT BE CREATED");
	SAFE_RELEASE(shaderBuffer);

	return inputLayout;
}


ShaderManager::~ShaderManager() {
	for (auto shader : shadersPool) {
		delete shader;
	}
	shadersPool.clear();
}

