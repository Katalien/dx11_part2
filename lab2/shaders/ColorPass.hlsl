#include "shaders/shared.h"

cbuffer PerFrame : register(PER_FRAME_CB_SLOT)
{
	PerFrameData frameData;
}

cbuffer PerMesh : register(PER_MESH_CB_SLOT)
{
	PerMeshData meshData;
}

cbuffer PerMaterial : register(PER_MATERIAL_CB_SLOT)
{
	PerMaterialData materialData;
}

cbuffer Lights : register(LIGHTS_CB_SLOT) {
	LightsData lightsData;
}

Texture2D TextureColor : register(TEXTURE_COLOR_SRV_SLOT);
Texture2D TextureAmbient : register(TEXTURE_AMBIENT_SRV_SLOT);
Texture2D TextureSpecular : register(TEXTURE_SPECULAR_SRV_SLOT);


SamplerState LinearSampler : register(LINEAR_SAMPLER_SLOT);
SamplerState NearestSampler : register(NEAREST_SAMPLER_SLOT);

struct VSOut {
	float4 pos : SV_POSITION;
	float3 worldPos : WORLD_POSITION;
	float3 normal : NORMAL;
	float2 texCoord : TEX_COORD;
};

VSOut VSMain(VSDefaultIn input) {
	VSOut res;
	res.worldPos = mul(meshData.transform, float4(input.pos, 1)).xyz;
	res.pos = mul(frameData.viewProj, float4(res.worldPos, 1));
	res.normal = input.normal;
	res.texCoord = input.texCoord;
	return res;
}

float3 Phong(float3 l, float3 lc, float3 n, float3 pos, float3 v, float3 Ka, float3 Kd, float3 Ks) {
	float3 res = lc * Ka;
	float nl = max(0, dot(n, l));
	if (nl > 0) {
		float3 r = reflect(-l, n);
		float rv = max(0, dot(r, -v));
		res += lc * Kd * nl;
		res += lc * Ks * pow(rv, materialData.ph);
	}

	return res;
}

float4 PSMain(VSOut input) : SV_TARGET{
	input.normal = normalize(input.normal);
	float3 view = normalize(input.worldPos - frameData.cameraPos);
	
	if (materialData.shadeModel == SHADE_MODEL_COLOR) {
		float4 color = materialData.color;
		if (materialData.usedTextures.x)
			color *= TextureColor.Sample(LinearSampler, input.texCoord);
		return color;
	} 
	
	if (materialData.shadeModel == SHADE_MODEL_PHONG) {
		float3 Ka = materialData.ambient;
		if (materialData.usedTextures.y)
			Ka *= TextureAmbient.Sample(LinearSampler, input.texCoord).xyz;
		
		float3 Kd = materialData.diffuse;
		if (materialData.usedTextures.x)
			Kd *= TextureColor.Sample(LinearSampler, input.texCoord).xyz;

		float3 Ks = materialData.specular;
		if (materialData.usedTextures.z)
			Ks *= TextureSpecular.Sample(LinearSampler, input.texCoord).xyz;

		float4 result = float4(0, 0, 0, 1);
		for (int i = 0; i < lightsData.pointAmount; i++) {
			float3 l = input.worldPos - lightsData.pointLights[i].position;
			float distSq = dot(l, l);
			if (distSq < lightsData.pointLights[i].radius * lightsData.pointLights[i].radius) {
				float3 lNorm = l / sqrt(distSq);
				result.xyz += Phong(lNorm, lightsData.pointLights[i].color, input.normal, input.worldPos, view, Ka, Kd, Ks);
			}
		}

		for (int i = 0; i < lightsData.dirAmount; i++) {
			float3 l = lightsData.dirLights[i].direction;
			float3 lNorm = normalize(l);
			result.xyz += Phong(lNorm, lightsData.pointLights[i].color, input.normal, input.worldPos, view, Ka, Kd, Ks);
		}

		return result;
	}

	return float4(1, 0, 1, 1);
}