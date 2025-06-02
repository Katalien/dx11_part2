#pragma once
#include "Dev.h"

class Shader;

template <class Type>
class ConstantBuffer
{
private:
	Render* render = nullptr;
	Type data;
	bool isDirty = true;
	ID3D11Buffer* constantBuffer = nullptr;
public:
	ConstantBuffer(Render* render);

	Type& getData() { isDirty = true; return data; }
	
	void setActive(Shader* shader, uint32_t slot);

	~ConstantBuffer();
};