#include "shaders/shared.h"
Texture2D TextureColor : register(TEXTURE_COLOR_SRV_SLOT);

SamplerState LinearSampler : register(LINEAR_SAMPLER_SLOT);
SamplerState NearestSampler : register(NEAREST_SAMPLER_SLOT);

struct VSOut {
	float4 pos : SV_POSITION;
	float2 texCoord : TEX_COORD;
};

VSOut VSMain(uint index : SV_VertexID) {
	VSOut res;
	if (index == 0) {
		res.pos = float4(-1, 1, 0.1, 1);
		res.texCoord = float2(0, 0);
	}
	else if (index == 1 || index == 4) {
		res.pos = float4(1, 1, 0.1, 1);
		res.texCoord = float2(1, 0);
	}
	else if (index == 2 || index == 3) {
		res.pos = float4(-1, -1, 0.1, 1);
		res.texCoord = float2(0, 1);
	}
	else {
		res.pos = float4(1, -1, 0.1, 1);
		res.texCoord = float2(1, 1);
	}	return res;
}

float4 PSMain(VSOut input) : SV_TARGET{
	float4 color = TextureColor.Sample(LinearSampler, input.texCoord);
	return log(1 + 0.2126 * color.x + 0.7151 * color.y + 0.0722 * color.z);
}