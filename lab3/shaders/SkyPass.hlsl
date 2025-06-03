#include "shaders/shared.h"


cbuffer PerFrame : register(PER_FRAME_CB_SLOT)
{
	PerFrameData frameData;
}

TextureCube TextureSky : register(TEXTURE_COLOR_SRV_SLOT);

SamplerState LinearSampler : register(LINEAR_SAMPLER_SLOT);
SamplerState NearestSampler : register(NEAREST_SAMPLER_SLOT);

struct VSOut {
	float4 pos : SV_POSITION;
	float3 texCoord : TEX_COORD;
};

VSOut VSMain(VSDefaultIn input) {
	VSOut res;
	float3 worldPos = input.pos + frameData.cameraPos;
	res.pos = mul(frameData.viewProj, float4(worldPos, 1));
	res.texCoord = input.normal;
	res.pos.z = res.pos.w;
	return res;
}

float4 PSMain(VSOut input) : SV_TARGET{
	input.texCoord.z *= -1; // due to Opengl cubemap
	float3 color = TextureSky.Sample(LinearSampler, input.texCoord).xyz;
	return float4(color, 1);
}