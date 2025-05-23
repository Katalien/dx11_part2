#include "shaders/shared.h"

cbuffer PerFrame : register(PER_FRAME_CB_SLOT)
{
	PerFrameData frameData;
}

cbuffer PerMesh : register(PER_MESH_CB_SLOT)
{
	PerMeshData meshData;
}

struct VSOut {
	float4 pos : SV_POSITION;
	float3 color : NIGGER;
};

VSOut VSMain(VSDefaultIn input) {
	VSOut res;
	res.pos = mul(frameData.viewProj, mul(meshData.transform, float4(input.pos, 1)));
	res.color = input.normal;
	return res;
}

float4 PSMain(VSOut input) : SV_TARGET{
	input.color.x = input.color.x < 0 ? 0.5 : input.color.x;
	input.color.y = input.color.y < 0 ? 0.5 : input.color.y;
	input.color.z = input.color.z < 0 ? 0.5 : input.color.z;

	return float4(input.color, 1);
}