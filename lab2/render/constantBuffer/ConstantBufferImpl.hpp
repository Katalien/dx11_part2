#pragma once

template <class Type>
ConstantBuffer<Type>::ConstantBuffer(Render* rend)
{
	render = rend;
}

template <class Type>
void ConstantBuffer<Type>::setActive(Shader* shader, uint32_t slot)
{
	if (constantBuffer == nullptr)
	{
		D3D11_BUFFER_DESC d3dDesc = {};
		d3dDesc.ByteWidth = sizeof(Type);
		d3dDesc.Usage = D3D11_USAGE_DEFAULT;
		d3dDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		d3dDesc.CPUAccessFlags = 0;
		d3dDesc.MiscFlags = 0;
		d3dDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA d3dData;
		d3dData.pSysMem = &data;
		d3dData.SysMemPitch = sizeof(Type);
		d3dData.SysMemSlicePitch = 0;

		render->getD3DDevice()->CreateBuffer(&d3dDesc, &d3dData, &constantBuffer);
		isDirty = false;
	}

	if (isDirty)
	{
		render->getD3DDeviceContext()->UpdateSubresource(constantBuffer, 0, nullptr, &data, 0, 0);
		isDirty = false;
	}

	switch (shader->getStage())
	{
	case Shader::Stage::Vertex:
		render->getD3DDeviceContext()->VSSetConstantBuffers(slot, 1, &constantBuffer);
		break;
	case Shader::Stage::Pixel:
		render->getD3DDeviceContext()->PSSetConstantBuffers(slot, 1, &constantBuffer);
		break;
	default:
		break;
	}
}

template <class Type>
ConstantBuffer<Type>::~ConstantBuffer()
{
	SAFE_RELEASE(constantBuffer);
}