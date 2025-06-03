#include "shaders/shared.h"

cbuffer Tonemapping : register(TONEMAPPING_CB_SLOT)
{
	ToneMappingCB passCB;
}

Texture2D TextureHDR : register(TEXTURE_COLOR_SRV_SLOT);

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


static const float A = 0.1;  // Shoulder Strength
static const float B = 0.50; // Linear Strength
static const float C = 0.1;  // Linear Angle
static const float D = 0.20; // Toe Strength
static const float E = 0.02; // Toe Numerator
static const float F = 0.30; // Toe Denominator
							 // Note: E/F = Toe Angle
static const float W = 11.2; // Linear White Point Value

float3 Uncharted2Tonemap(float3 x)
{
	return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

float4 PSMain(VSOut input) : SV_TARGET{
	float3 color = TextureHDR.Sample(LinearSampler, input.texCoord).xyz;
//	return float4(color, 1);
	
	float avgBrightness = exp(passCB.avgBrightnessLog) - 1;

	float keyValue = 1.03 - 2 / (2 + log10(avgBrightness + 1));

	float E = keyValue / avgBrightness * passCB.sceneExposure;

	
	float3 curr = Uncharted2Tonemap(E * color);

	float3 whiteScale = 1.0f / Uncharted2Tonemap(W);

	return float4(curr * whiteScale, 1);
}