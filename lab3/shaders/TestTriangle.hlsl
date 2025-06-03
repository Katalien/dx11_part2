#include "shaders/shared.h"

cbuffer TriangleColor : register(b0)
{
	TriangleData globalData;
}

cbuffer PerFrame : register(b1)
{
	PerFrameData frameData;
}

struct VSOut {
	float4 pos : SV_POSITION;
	float3 color : NIGGER;
};

VSOut VSMain(uint index : SV_VertexID) {
	VSOut res;
	if (index == 0) {
		res.pos = float4(0, 0.5, 0.1, 1);
		res.color = globalData.color1;
	}
	else if (index == 1) {
		res.pos = float4(0.5, -0.5, 0.1, 1);
		res.color = globalData.color2;
	}
	else {
		res.pos = float4(-0.3, 0.1, 0.1, 1);
		res.color = globalData.color3;
	}

	res.pos = mul(frameData.viewProj, res.pos);

	return res;
}

float4 PSMain(VSOut input) : SV_TARGET {
	return float4(input.color, 1);
}