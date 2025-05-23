#pragma once
#include "Dev.h"

class Shader {
public:
	enum Stage {
		Vertex = 0,
		Pixel,

		Stage_Count
	};

private:
	friend class ShaderManager;
	Render* render;
	std::string name = "";
	union D3D
	{
		ID3D11VertexShader* vertexShader;
		ID3D11PixelShader* pixelShader;
	} d3d;
	Stage stage = Vertex;
	std::map<std::string, std::string> defines = {};
public:
	Shader(Render* rend);

	Stage getStage() { return stage; }

	void setActive();

	~Shader();
};

class ShaderManager {
private:
	Render* render = nullptr;
	std::vector<Shader*> shadersPool;
public:
	ShaderManager(Render* render);

	Shader* getShader(std::string path, Shader::Stage stage, std::map<std::string, std::string> defines = {});
	ID3D11InputLayout* getInputLayout(D3D11_INPUT_ELEMENT_DESC *inputDesc, size_t inputDescAmount, std::string path, std::map<std::string, std::string> defines = {});

	~ShaderManager();
};